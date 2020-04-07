/* 
 * MIT License
 * z80.ro
*/
#ifndef WORKERMOUSE_H
#define WORKERMOUSE_H

#include <QtCore>
#include <QThread>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <linux/input.h>
#include "sys/select.h"
#include <fcntl.h>
#include <unistd.h>

#define KCTRL  0x01
#define KALT   0x02
#define KSHIFT 0x04

class workermouse : public QThread {
    Q_OBJECT

    void run();

    QString device;
    uint8_t new_device_was_set;
    const char *ar[0x300];
    struct input_event e[64];
    int f, read_size;
    QScreen *screen0;
    QPoint pos0;
    QScreen *screen1;
    QPoint pos1;

    quint32 special_keys_state;
    quint32 buttons_state;

    bool device_changed;

signals:
    void return_read(quint32 button_state, QPoint position);

public:
    void set_device(QString device);
    void setup();
    void restart_capture();
};

#endif // WORKERMOUSE_H
