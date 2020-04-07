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

    pmap.load(":/header.png");

    this->resize(1920, 92);
    this->setAutoFillBackground(true);
    current_y = 500;
    this->move(0, current_y);

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

    add_value = 0;
    extra_offset = 0;
    total_width = 0;

    connect(&tmrfin, SIGNAL(timeout()), this, SLOT(timer_fin_timeout()));
    tmrfin.setInterval(7);
    tmrfin.start();

}

void transparent::return_read(const char *s, quint32 value, quint32 special_keys_state) {
    int i;

    // Handle only the key-presses for the remaining keys
    if (value == 1 && start_capturing) {
        if (qi == MAX_QUEUE_ITEMS) {
            for (i=0; i<MAX_QUEUE_ITEMS-1; i++) queue[i] = queue[i+1];
            qi = MAX_QUEUE_ITEMS-1;
        }
        queue[qi].state_ctrl = (special_keys_state & KCTRL) != 0;
        queue[qi].state_alt = (special_keys_state & KALT) != 0;
        queue[qi].state_shift = (special_keys_state & KSHIFT) != 0;
        queue[qi].width = 70;
        if (strlen(s) > 2) {
            if (strncmp(s, "LEFT", 4) && strncmp(s, "RIGHT", 5) && strncmp(s, "UP", 2) && strncmp(s, "DOWN", 4)) {
                queue[qi].width = 173;
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
    if (start_fading) {
        opacity -= 0.01f;
        if (opacity < 0) opacity = 0;
    }

    if (!reposition) {
        if (queue[0].s != nullptr) {
            if (extra_offset - total_width > 0) {
                add_value += 1;
                //if (add_value > 1000) add_value = 1000;
            }
            extra_offset += 1;
            if (extra_offset > 20000) extra_offset = 20000;
            if (isHidden()) show();
            update();
            //qDebug("Oooops, timer too fast");
        }
    }
}

void transparent::display_special_keys(int pos, QPainter *p, QPen pen, int i) {
    int posy, rect_height;

    posy = 68;
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
    QFont font("Arial", 24);
    QFont fontmedium("Arial", 16);
    QFont fontsmall("Arial", 10);
    int pos, i;
    bool is_arrow = false;

    static QPixmap *fb = nullptr;
    if (!fb) fb = new QPixmap(1920, 92);
    QPainter p;

    pos = 1000-extra_offset;

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
                pos += queue[i].add;            // adjust the position
                // only draw it if it's visible
                if (pos+queue[i].width >= 0) {
                    if (strlen(queue[i].s) <= 2) {
                        p.drawImage(pos, 2, QImage(":/key_on_keyboard.png"));
                    } else if (!strncmp(queue[i].s, "LEFT", 4)) {
                        is_arrow = true;
                        p.drawImage(pos, 2, QImage(":/key_left.png"));
                    } else if (!strncmp(queue[i].s, "RIGHT", 5)) {
                        is_arrow = true;
                        p.drawImage(pos, 2, QImage(":/key_right.png"));
                    } else if (!strncmp(queue[i].s, "UP", 2)) {
                        is_arrow = true;
                        p.drawImage(pos, 2, QImage(":/key_up.png"));
                    } else if (!strncmp(queue[i].s, "DOWN", 4)) {
                        is_arrow = true;
                        p.drawImage(pos, 2, QImage(":/key_down.png"));
                    } else {
                        is_arrow = false;
                        p.drawImage(pos, 2, QImage(":/keylarge_on_keyboard.png"));
                    }

                    pen.setColor(QColor(230, 230, 230));
                    p.setPen(pen);
                    if (strlen(queue[i].s) == 1) {
                        p.setFont(font);
                        p.drawText(pos+16, 32, queue[i].s);
                    } else {
                        if (!is_arrow) {
                            p.setFont(fontmedium);
                            p.drawText(pos+10, 25, queue[i].s);
                        }
                    }

                    fontsmall.setBold(true);
                    p.setFont(fontsmall);

                    display_special_keys(pos, &p, pen, i);
                }

                pos += queue[i].width;
            }
        }
        p.end();
    //}

    // Spit it on the screen
    scrp.begin(this);
    scrp.setOpacity(opacity);
    scrp.setRenderHint(QPainter::Antialiasing);
    //scrp.setOpacity(0.5f);
    scrp.drawPixmap(0, 0, 1920, 92, pmap);
    scrp.drawPixmap(0, 0, 1920, 92, *fb);
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
        //qDebug("Mouse start Y: %d\n", started_to_move_y);
        reposition = true;
    }
}

void transparent::mouseReleaseEvent(QMouseEvent* /* event */) {
    reposition = false;
    started_to_move_y = 0;
}

void transparent::mouseMoveEvent(QMouseEvent *event) {

    if (reposition) {
        current_y = event->screenPos().y() - started_to_move_y;
        //qDebug("relative_y: %d, event_pos: %d", event->y(), event->screenPos().y());
        /*
        tmp = event->pos().y() - started_to_move_y;
        current_y += tmp;
        qDebug("current_y: %d, event_pos: %d, screenposy: %f", current_y, event->pos().y(), event->screenPos().y());
        */
        if (current_y < 0) current_y = 0;
        if (current_y + 92 > QApplication::desktop()->size().height()) current_y = QApplication::desktop()->size().height() - 92;
        this->move(0, current_y);
        //qDebug("current_y: %d, event_pos: %d\n\nf", current_y, event->pos().y());
        update();
    }
}

