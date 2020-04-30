/*
 * MIT License
 * z80.ro
*/
#include "transparent.h"

transparent::transparent(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    transparent::setMouseTracking(true);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_ShowWithoutActivating);

    setStyleSheet("background-color: rgba(20,20,20,20)");

    //setParent(0);

    pmap.load(":/images/header.png");

    on_screen_width = 1920;
    on_screen_height = 92;
    current_x = 0;
    height_suffix = "";
    height_radio = 2;       // by default it's large

    this->resize(on_screen_width, on_screen_height);
    this->setAutoFillBackground(true);
    current_y = 500;
    this->move(current_x, current_y);

    // init framebuffer
    fb = new QPixmap(on_screen_width, on_screen_height);

    start_capturing = true;
    reposition = false;
    start_fading = false;

    for (int i=0; i<MAX_QUEUE_ITEMS; i++) {
        queue[i].s = nullptr;
        queue[i].state_alt = false;
        queue[i].state_shift = false;
        queue[i].state_ctrl = false;
        queue[i].width = 0;
        queue[i].add = 0;
    }

    qi = 0;

    add_value = 0;
    extra_offset = 0;
    total_width = 0;

    scroll_speed = 2;
    is_adaptive_speed = true;
    adaptive_speed_factor = 5;

    connect(&tmrfin, SIGNAL(timeout()), this, SLOT(timer_fin_timeout()));
    tmrfin.setInterval(10);
    tmrfin.start();
}

void transparent::set_height_suffix(QString s, int radio) {
    height_suffix = s;
    height_radio = radio;
    switch(height_radio) {
    case 0:
        on_screen_height = 72;
        break;
    case 1:
        on_screen_height = 82;
        break;
    case 2:
    default:
        on_screen_height = 92;
        break;
    }
    this->resize(on_screen_width, on_screen_height);
    if (fb) delete(fb);
    fb = new QPixmap(on_screen_width, on_screen_height);
}

void transparent::set_width(int w) {
    on_screen_width = w;
    this->resize(on_screen_width, on_screen_height);
    if (fb) delete(fb);
    fb = new QPixmap(on_screen_width, on_screen_height);
}

void transparent::set_adaptive_factor(int f) {
    adaptive_speed_factor = f;
}

void transparent::set_pos_x(int x) {
    current_x = x;
    this->move(current_x, current_y);
}

void transparent::change_speed(int value) {
    this->scroll_speed = value;
}

void transparent::adaptive_speed(bool value) {
    this->is_adaptive_speed = value;
}

void transparent::return_read(const char *s, quint32 value, quint32 special_keys_state) {
    int i;

    // Handle only the key-presses for the remaining keys
    if (value == 1 && start_capturing) {

        //qDebug("Received %s", s);

        if (qi > 0 && queue[0].width == 0) {
            for (i=0; i<MAX_QUEUE_ITEMS-1; i++) queue[i] = queue[i+1];
            qi--;
        }

        if (qi == MAX_QUEUE_ITEMS) {
            for (i=0; i<MAX_QUEUE_ITEMS-1; i++) queue[i] = queue[i+1];
            qi = MAX_QUEUE_ITEMS-1;
        }
        queue[qi].state_ctrl = (special_keys_state & KCTRL) != 0;
        queue[qi].state_alt = (special_keys_state & KALT) != 0;
        queue[qi].state_shift = (special_keys_state & KSHIFT) != 0;
        switch(height_radio) {
        case 0:
            queue[qi].width = 50;
            break;
        case 1:
            queue[qi].width = 60;
            break;
        case 2:
        default:
            queue[qi].width = 70;
            break;
        }
        if (strlen(s) > 2) {
            if (    !(
                    (!strncmp(s, "LEFT", 4) && strlen(s) == 4)  ||
                    (!strncmp(s, "RIGHT", 5) && strlen(s) == 5) ||
                    (!strncmp(s, "UP", 2) && strlen(s) == 2)    ||
                    (!strncmp(s, "DOWN", 4) && strlen(s) == 4) )
                ) {

                switch(height_radio) {
                case 0:
                    queue[qi].width = 110;
                    break;
                case 1:
                    queue[qi].width = 142;
                    break;
                case 2:
                default:
                    queue[qi].width = 173;
                    break;
                }
            }
        }

        if ((extra_offset - total_width) > 0) queue[qi].add = add_value;
        else queue[qi].add = 0;

        total_width += queue[qi].width + queue[qi].add;
        //qDebug("Added %s, extra_offset: %d, total_width: %d, width: %d, add: %d, extra_offset-total_w: %d\n", s, extra_offset, total_width, queue[qi].width, queue[qi].add, extra_offset - total_width);

        queue[qi].s = s;
        add_value = 0;
        qi++;
/*
        for (i=0; i<qi; i++) {
            qDebug("%s, ",queue[i].s);
        }
*/
        //extra_offset = 500;
        opacity = 1.0f;
        show();
    }
    //update();

}

void transparent::empty_buffer() {
    hide();
    qi = 0;
    for (int i=0; i<MAX_QUEUE_ITEMS; i++) {;
        queue[i].s = NULL;
        queue[i].state_alt = false;
        queue[i].state_shift = false;
        queue[i].state_ctrl = false;
        queue[i].width = 0;
        queue[i].add = 0;
    }
    extra_offset = 0;
    add_value = 0;
    total_width = 0;
    start_fading = false;
    reposition = false;
    opacity = 1.0f;
}

void transparent::timer_fin_timeout() {
    int i, active_items=0, adaptive_increment=0;

    if (start_fading) {
        opacity -= 0.01f;
        if (opacity < 0) opacity = 0;
    }

    if (!reposition) {
        if (queue[0].s != nullptr) {
            if (extra_offset - total_width > 0) {
                add_value += scroll_speed;
                //if (add_value > 1000) add_value = 1000;
            }
            if (this->is_adaptive_speed) {
                for (i=0; i<MAX_QUEUE_ITEMS-1; i++) if (queue[i].width > 0) active_items++;
                //adaptive_increment = scroll_speed * ((active_items * 30) / MAX_QUEUE_ITEMS);
                adaptive_increment = (active_items * adaptive_speed_factor) / (MAX_QUEUE_ITEMS >> 4);
                //qDebug("Active items: %d, adaptive_increment: %d, extra_offset: %d, total_width: %d", active_items, adaptive_increment, extra_offset, total_width);
            }
            extra_offset += this->scroll_speed + adaptive_increment;
            if (extra_offset > 20000) extra_offset = 20000;
            if (isHidden()) show();
            update();
            //qDebug("Oooops, timer too fast");
        }
    }
}

void transparent::display_special_keys(int pos, QPainter *p, QPen pen, int i) {
    int posy, rect_height;

    switch(height_radio) {
    case 0:
        posy = 48;
        break;
    case 1:
        posy = 58;
        break;
    case 2:
    default:
        posy = 68;
        break;
    }
    rect_height = 0;

    if (queue[i].state_ctrl || queue[i].state_alt || queue[i].state_shift) {

        if (queue[i].state_ctrl) rect_height += 12;
        if (queue[i].state_alt) rect_height += 12;
        if (queue[i].state_shift) rect_height += 12;
        p->fillRect(pos, posy-10, 55, rect_height, QColor(50, 50, 50));

        if (queue[i].state_ctrl) {
            pen.setColor(QColor(255,255,255));
            p->setPen(pen);
            p->drawText(pos+12, posy, "CTRL");
            posy += 11;
        }
        if (queue[i].state_alt) {
            pen.setColor(QColor(255,64,64));
            p->setPen(pen);
            p->drawText(pos+17, posy, "ALT");
            posy += 11;
        }
        if (queue[i].state_shift) {
            pen.setColor(QColor(255,255,0));
            p->setPen(pen);
            p->drawText(pos+10, posy, "SHIFT");
            posy += 11;
        }
    }
}

void transparent::paintEvent(QPaintEvent* /* event */) {
    QPainter scrp;
    QPen pen;

    QFont fontsmall0("Arial", 7);
    QFont fontsmall1("Arial", 8);
    QFont fontsmall2("Arial", 10);

    QFont fontlarge0("Arial", 16);
    QFont fontlarge1("Arial", 20);
    QFont fontlarge2("Arial", 24);

    QFont fontmed0("Arial", 8);
    QFont fontmed1("Arial", 12);
    QFont fontmed2("Arial", 16);

    int pos, i;
    bool is_arrow = false;

    QPainter p;

    if (on_screen_width > 100) pos = (on_screen_width - 100) - extra_offset;
    else pos = on_screen_width - extra_offset;

    //if (has_changed) {
        fb->fill(Qt::transparent);
        p.begin(fb);
        p.setRenderHint(QPainter::Antialiasing);
        //pen.setColor(QColor(255,0,0));
        //p.setPen(pen);
        //p.drawLine(0, 0, 1920, 92);

        //font.setBold(true);
        for (i=0; i<MAX_QUEUE_ITEMS; i++) {
            if (queue[i].s != nullptr) {
                if (pos + queue[i].add + queue[i].width < 0) {
                    if (queue[i].width > 0) {
                        //qDebug("item %d, add: %d, width: %d, extra_offset: %d, pos: %d", i, queue[i].add, queue[i].width, extra_offset, pos);
                        extra_offset -= queue[i].add + queue[i].width;
                        pos += queue[i].add + queue[i].width;
                        total_width -= queue[i].add + queue[i].width;

                        queue[i].add = 0;
                        queue[i].width = 0;
                    }

                } else if (pos + queue[i].add >= on_screen_width) {
                    continue;

                } else {
                    pos += queue[i].add;            // adjust the position
                    // only draw it if it's visible
                    if (pos+queue[i].width >= 0) {
                        if (strlen(queue[i].s) <= 2) {
                            p.drawImage(pos, 2, QImage(":/images/key_on_keyboard"+height_suffix+".png"));
                        } else if (!strncmp(queue[i].s, "LEFT", 4) && strlen(queue[i].s) == 4) {
                            is_arrow = true;
                            p.drawImage(pos, 2, QImage(":/images/key_left"+height_suffix+".png"));
                        } else if (!strncmp(queue[i].s, "RIGHT", 5) && strlen(queue[i].s) == 5) {
                            is_arrow = true;
                            p.drawImage(pos, 2, QImage(":/images/key_right"+height_suffix+".png"));
                        } else if (!strncmp(queue[i].s, "UP", 2) && strlen(queue[i].s) == 2) {
                            is_arrow = true;
                            p.drawImage(pos, 2, QImage(":/images/key_up"+height_suffix+".png"));
                        } else if (!strncmp(queue[i].s, "DOWN", 4) && strlen(queue[i].s) == 4) {
                            is_arrow = true;
                            p.drawImage(pos, 2, QImage(":/images/key_down"+height_suffix+".png"));
                        } else {
                            is_arrow = false;
                            p.drawImage(pos, 2, QImage(":/images/keylarge_on_keyboard"+height_suffix+".png"));
                        }

                        pen.setColor(QColor(230, 230, 230));
                        p.setPen(pen);
                        // If it is one character only
                        if (strlen(queue[i].s) == 1) {
                            switch(height_radio) {
                            case 0:
                                p.setFont(fontlarge0);
                                p.drawText(pos+9, 22, queue[i].s);
                                break;
                            case 1:
                                p.setFont(fontlarge1);
                                p.drawText(pos+12, 28, queue[i].s);
                                break;
                            case 2:
                            default:
                                p.setFont(fontlarge2);
                                p.drawText(pos+16, 32, queue[i].s);
                                break;
                            }
                        // if it's multi-character
                        } else {
                            if (!is_arrow) {
                                switch(height_radio) {
                                case 0:
                                    p.setFont(fontmed0);
                                    p.drawText(pos+10, 18, queue[i].s);
                                    break;
                                case 1:
                                    p.setFont(fontmed1);
                                    p.drawText(pos+10, 21, queue[i].s);
                                    break;
                                case 2:
                                default:
                                    p.setFont(fontmed2);
                                    p.drawText(pos+10, 25, queue[i].s);
                                    break;
                                }
                            }
                        }

                        fontsmall.setBold(true);
                        switch(height_radio) {
                        case 0:
                            p.setFont(fontsmall0);
                            break;
                        case 1:
                            p.setFont(fontsmall1);
                            break;
                        case 2:
                        default:
                            p.setFont(fontsmall2);
                            break;
                        }
                        display_special_keys(pos, &p, pen, i);
                    }

                    pos += queue[i].width;
                }
            }
        }
        p.end();
    //}

    // Spit it on the screen
    scrp.begin(this);
    scrp.setOpacity(opacity);
    scrp.setRenderHint(QPainter::Antialiasing);
    //scrp.setOpacity(0.5f);
    scrp.drawPixmap(0, 0, on_screen_width, on_screen_height, pmap);
    scrp.drawPixmap(0, 0, on_screen_width, on_screen_height, *fb);
    //scrp.drawPixmap(0, 0, *fb, offset_in_pixmap, 0, fb->width()-offset_in_pixmap, fb->height());
    scrp.end();

    if (pos < 0 || (start_fading && opacity == 0)) {
        empty_buffer();
    }

}

void transparent::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        //empty_buffer();
        start_fading = true;
    } else if (event->button() == Qt::LeftButton) {
        started_to_move_y = event->pos().y();
        started_to_move_x = event->pos().x();
        //qDebug("Mouse start Y: %d\n", started_to_move_y);
        reposition = true;
    }
}

void transparent::mouseReleaseEvent(QMouseEvent* /* event */) {
    reposition = false;
    started_to_move_y = 0;
    started_to_move_x = 0;
}

void transparent::mouseMoveEvent(QMouseEvent *event) {

    if (reposition) {
        current_y = event->screenPos().y() - started_to_move_y;
        current_x = event->screenPos().x() - started_to_move_x;
        //qDebug("relative_y: %d, event_pos: %d", event->y(), event->screenPos().y());
        /*
        tmp = event->pos().y() - started_to_move_y;
        current_y += tmp;
        qDebug("current_y: %d, event_pos: %d, screenposy: %f", current_y, event->pos().y(), event->screenPos().y());
        */

        if (current_y < 0) current_y = 0;
        if (current_y + on_screen_height > QApplication::desktop()->size().height()) current_y = QApplication::desktop()->size().height() - on_screen_height;

        if (current_x < 0) current_x = 0;
        if (current_x + on_screen_width > QApplication::desktop()->size().width()) current_x = QApplication::desktop()->size().width() - on_screen_width;

        this->move(current_x, current_y);
        //qDebug("current_y: %d, event_pos: %d\n\nf", current_y, event->pos().y());
        update();
    }
}

