/* 
 * MIT License
 * z80.ro
*/
#ifndef TRANSPARENT_MOUSE_H
#define TRANSPARENT_MOUSE_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QMouseEvent>
#include "worker.h"

#define KCTRL  0x01
#define KALT   0x02
#define KSHIFT 0x04

struct mouse_state {
    bool state_left;
    bool state_middle;
    bool state_right;
    bool wheel_up;
    bool wheel_down;
};

class transparent_mouse : public QWidget {
    Q_OBJECT

    QPixmap pmap;
    QPixmap wheel_up;
    QPixmap wheel_down;
    QPixmap special_back;

    QTimer tmrfin;
    qint32 extra_offset;
    qint32 add_value;
    qint32 total_width;

    QPoint pos;

    qint32 current_y;
    qint32 started_to_move_y;
    bool reposition;

    bool display_on;
    qint32 display_counter;
    qint32 wheel_counter;

    bool start_fading;
    qreal opacity;

    bool display_reverse_scroll;


private slots:
    void return_read(quint32 value, QPoint position);
    void return_special_keys(quint32 state);
    void timer_fin_timeout();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

public:
    explicit transparent_mouse(QWidget *parent = nullptr);
    void reverse_scroll(bool);

    mouse_state buttons_state;
    mouse_state blip_state;

    qint32 special_keys_state;
    int qi = 0;
    bool start_capturing;

};

#endif // TRANSPARENT_MOUSE_H
