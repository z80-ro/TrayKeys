/* 
 * MIT License
 * z80.ro
*/
#ifndef TRANSPARENT_H
#define TRANSPARENT_H

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QMouseEvent>
#include "worker.h"

#define MAX_QUEUE_ITEMS 1000

struct keypress {
    const char *s;
    bool state_ctrl;
    bool state_alt;
    bool state_shift;
    qint32 width;
    qint32 add;
};

class transparent : public QWidget {
    Q_OBJECT

    QPixmap pmap;
    QTimer tmrfin;
    qint32 extra_offset;
    qint32 add_value;
    qint32 total_width;
    int on_screen_width;
    int on_screen_height;

    QFont font;
    QFont fontmedium;
    QFont fontsmall;

    int current_x;
    qint32 current_y;
    qint32 started_to_move_x;
    qint32 started_to_move_y;
    bool reposition;

    bool start_fading;
    qreal opacity;

    int scroll_speed;
    bool is_adaptive_speed;
    int adaptive_speed_factor;

    QPixmap *fb;

    QString height_suffix;
    int height_radio;

    void empty_buffer();
    void display_special_keys(int pos, QPainter *p, QPen pen, int i);

private slots:
    void return_read(const char *s, quint32 value, quint32 special_keys_state);
    void timer_fin_timeout();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

public:
    explicit transparent(QWidget *parent = nullptr);
    void change_speed(int);
    void adaptive_speed(bool);
    void set_width(int);
    void set_pos_x(int);
    void set_adaptive_factor(int);
    void set_height_suffix(QString, int);

    //const char *queue[10];
    keypress queue[MAX_QUEUE_ITEMS];
    int qi = 0;
    bool start_capturing;

};

#endif // TRANSPARENT_H
