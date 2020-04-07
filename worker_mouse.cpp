/* 
 * MIT License
 * z80.ro
*/
#include "worker_mouse.h"

void workermouse::run() {
    int selres;
    fd_set set;
    struct timeval timeout;

    device_changed = true;
    f = -1;

    while (1) {
        if (device_changed) {
            device_changed = false;
            if (f != -1) close(f);
            f = open("/dev/input/"+device.toLocal8Bit(), O_RDONLY);
            qDebug() << "New device opened: " << "/dev/input/"+device.toLocal8Bit() << " - result: " << f;
        }
        if (f != -1) {
            FD_ZERO(&set);
            FD_SET(f, &set);
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000;   // every one tenth of a second

            selres = select(f+1, &set, nullptr, nullptr, &timeout);
            if (selres > 0) {
                if ((read_size = read(f, e, sizeof(struct input_event))) == sizeof(struct input_event)) {

                    pos0 = QCursor::pos(screen0);
                    //pos1 = QCursor::pos(screen1);

                    //qDebug("Position: %d, %d", pos0.x(), pos0.y());

                    if (e[0].type == 1) {
                        // if button was presed
                        if (e[0].value == 1) {
                            if (e[0].code == BTN_LEFT) {
                                buttons_state |= 0b0001;
                            }
                            if (e[0].code == BTN_RIGHT) {
                                buttons_state |= 0b0010;
                            }
                            if (e[0].code == BTN_MIDDLE) {
                                buttons_state |= 0b0100;
                            }
                        // if button was released
                        } else {
                            if (e[0].code == BTN_LEFT) {
                                buttons_state &= 0b11111110;
                            }
                            if (e[0].code == BTN_RIGHT) {
                                buttons_state &= 0b11111101;
                            }
                            if (e[0].code == BTN_MIDDLE) {
                                buttons_state &= 0b11111011;
                            }
                        }
                        emit return_read(buttons_state, pos0);
                    }

                    if (e[0].type == 2) {
                        if (e[0].code == REL_WHEEL) {
                            if (e[0].value & 0xFF0000) {
                                buttons_state = (buttons_state & 0b0111) | 0b1000;
                            } else buttons_state = (buttons_state & 0b0111) | 0b10000;
                        }
                    emit return_read(buttons_state, pos0);
                    buttons_state &= 0b0111;
                    }

                    //qDebug("%x, %x, %x", e[0].type, e[0].code, e[0].value);

                };
            }
        }
    }
}

void workermouse::setup() {
    screen0 = QApplication::screens().at(0);
    //screen1 = QApplication::screens().at(1);
    buttons_state = 0;
}

void workermouse::restart_capture() {
    device_changed = true;
}

void workermouse::set_device(QString device) {
    this->device = device;
    this->new_device_was_set = 1;
}


