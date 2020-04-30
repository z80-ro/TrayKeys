/* 
 * MIT License
 * z80.ro
*/
#include "transparent_mouse.h"

transparent_mouse::transparent_mouse(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    transparent_mouse::setMouseTracking(true);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_ShowWithoutActivating);

    setStyleSheet("background-color: rgba(20,20,20,20)");

    //setParent(0);

    pmap.load(":/images/mouse_back.png");
    wheel_up.load(":/images/wheel_up.png");
    wheel_down.load(":/images/wheel_down.png");
    special_back.load(":/images/special_background.png");

    this->resize(50, 61);
    this->setAutoFillBackground(true);
    this->move(0, 0);

    start_capturing = true;
    reposition = false;
    start_fading = false;
    display_reverse_scroll = false;

    buttons_state.state_left = false;
    buttons_state.state_middle = false;
    buttons_state.state_right = false;
    buttons_state.wheel_down = false;
    buttons_state.wheel_up = false;
    blip_state = buttons_state;

    add_value = 0;
    extra_offset = 0;
    total_width = 0;
    pos = QPoint(0, 0);

    connect(&tmrfin, SIGNAL(timeout()), this, SLOT(timer_fin_timeout()));
    tmrfin.setInterval(7);
    tmrfin.start();
    display_on = false;
    display_counter = 0;
    wheel_counter = 0;

    special_keys_state = 0;

}

void transparent_mouse::reverse_scroll(bool value) {
    display_reverse_scroll = value;
}

void transparent_mouse::return_special_keys(quint32 sks) {
    special_keys_state = sks;
    //qDebug("Special keys state: %X", sks);
    if (!isHidden()) {
        update();
    }
}

void transparent_mouse::return_read(quint32 btn_state, QPoint pos) {
    int tmp_x, tmp_y;
    //qDebug("Received event: %x", btn_state);

    if (start_capturing) {

        if (btn_state & 0b0001) {
            buttons_state.state_left = true;
        } else buttons_state.state_left = false;

        if (btn_state & 0b0010) {
            buttons_state.state_right = true;
        } else buttons_state.state_right = false;

        if (btn_state & 0b0100) {
            buttons_state.state_middle = true;
        } else buttons_state.state_middle = false;

        if (btn_state & 0b1000) {
            buttons_state.wheel_down = true;
            buttons_state.wheel_up = false;
        }

        if (btn_state & 0b10000) {
            buttons_state.wheel_up = true;
            buttons_state.wheel_down = false;
        }

        if (btn_state & 0x1F) {
            display_on = true;
            display_counter = 0;
            blip_state = buttons_state;     // latch the state
        }

        if (btn_state & 0x18) {
            wheel_counter = 0;
        }

        this->pos = pos;

        tmp_x = pos.x()-50;
        tmp_y = pos.y()-71;
        if (tmp_x < 0) {
            tmp_x = pos.x()+30;
        }
        if (tmp_y < 0) {
            tmp_y = pos.y()+30;
        }
        this->move(tmp_x, tmp_y);

        if (btn_state & 0x1F) {
            opacity = 0.75f;
            show();
            update();
        }
    } else {
        buttons_state.state_left = false;
        buttons_state.state_middle = false;
        buttons_state.state_right = false;
        buttons_state.wheel_down = false;
        buttons_state.wheel_up = false;
        blip_state = buttons_state;
    }
}


void transparent_mouse::timer_fin_timeout() {

    if (wheel_counter > 50) {
        buttons_state.wheel_down = false;
        buttons_state.wheel_up = false;
        blip_state.wheel_down = false;
        blip_state.wheel_up = false;
        update();
    }

    if (buttons_state.state_left || buttons_state.state_middle || buttons_state.state_right) {
        display_counter = 0;
    } else {
        if (display_counter > 50) {
            opacity -= 0.01f;
            if (opacity < 0) {
                opacity = 0.0f;
                hide();
                display_on = false;
            }
            update();
        }
    }

    if (display_on) {
        display_counter++;
        wheel_counter++;
    }
}


void transparent_mouse::paintEvent(QPaintEvent* /* event */) {
    QPainter scrp;
    QPen pen;
    QFont font("Arial", 15);
    QFont fontsmall("Arial", 10);

    //qDebug("Paint event");

    static QPixmap *fb = nullptr;

    if (!fb) fb = new QPixmap(50, 61);

    fb->fill(Qt::transparent);

    QPainter p;

    p.begin(fb);
    p.setRenderHint(QPainter::Antialiasing);
    font.setBold(true);
    p.setFont(font);

    if (blip_state.state_left) {
        pen.setColor(QColor(255,0,0));
        p.drawText(2, 56, "L");
    }
    if (blip_state.state_middle) {
        pen.setColor(QColor(0,255,0));
        p.drawText(16, 56, "M");
    }
    if (blip_state.state_right) {
        pen.setColor(QColor(0,0,255));
        p.drawText(34, 56, "R");
    }

    if (blip_state.wheel_up) {
        if (display_reverse_scroll) {
            p.drawPixmap(19, 48, 11, 13, wheel_down);
        } else {
            p.drawPixmap(19, 36, 11, 13, wheel_up);
        }
    }

    if (blip_state.wheel_down) {
        if (display_reverse_scroll) {
            p.drawPixmap(19, 36, 11, 13, wheel_up);
        } else {
            p.drawPixmap(19, 48, 11, 13, wheel_down);
        }
    }

    fontsmall.setBold(true);
    p.setFont(fontsmall);
    if (special_keys_state & KALT) {
        pen.setColor(QColor(255,255,255));
        p.setPen(pen);
        p.drawText(12, 11, "ALT");
    }
    if (special_keys_state & KCTRL) {
        pen.setColor(QColor(0,255,255));
        p.setPen(pen);
        p.drawText(7, 23, "CTRL");
    }
    if (special_keys_state & KSHIFT) {
        pen.setColor(QColor(255,255,0));
        p.setPen(pen);
        p.drawText(5, 34, "SHIFT");
    }

    //pen.setColor(QColor(255,0,0));
    //p.setPen(pen);
    //p.drawLine(0, 0, 50, 25);
    p.end();

    // Spit it on the screen
    scrp.begin(this);
    scrp.setOpacity(opacity);
    scrp.setRenderHint(QPainter::Antialiasing);
    //scrp.setOpacity(0.5f);
    if (special_keys_state & (KALT|KSHIFT|KCTRL)) {
        scrp.drawPixmap(0, 0, 50, 36, special_back);
    }
    scrp.drawPixmap(0, 36, 50, 25, pmap);
    scrp.drawPixmap(0, 0, 50, 61, *fb);
    //scrp.drawPixmap(0, 0, *fb, offset_in_pixmap, 0, fb->width()-offset_in_pixmap, fb->height());
    scrp.end();

//    if (pos < 0 || (start_fading && opacity == 0)) {
//        empty_buffer();
//    }

}

void transparent_mouse::mousePressEvent(QMouseEvent* /* event */) {
}

void transparent_mouse::mouseReleaseEvent(QMouseEvent* /* event */) {
}

void transparent_mouse::mouseMoveEvent(QMouseEvent* /* event */) {
}

