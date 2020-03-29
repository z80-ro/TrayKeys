/* 
 * MIT License
 * z80.ro
*/
#ifndef WORKER_H
#define WORKER_H

#include <QtCore>
#include <QThread>
#include <QDir>
#include <QString>
#include <QStringList>
#include <linux/input.h>
#include "sys/select.h"
#include <fcntl.h>
#include <unistd.h>

#define KCTRL  0x01
#define KALT   0x02
#define KSHIFT 0x04

class worker : public QThread {
    Q_OBJECT

    void run();

    QString device;
    uint8_t new_device_was_set;
    const char *ar[0x300];
    struct input_event e[64];
    int f, read_size;

    quint32 special_keys_state;

    bool translate(uint32_t code);
    bool device_changed;

signals:
    void return_read(const char *s, quint32 value, quint32 special_keys_state);

public:
    void set_strings();
    void set_device(QString device);
    void setup();
    void restart_capture();
};

#endif // WORKER_H
