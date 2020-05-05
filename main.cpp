/* 
 * MIT License
 * z80.ro
*/
#include "mainwindow.h"
#include "worker.h"
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QSystemTrayIcon>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QSystemTrayIcon systrayicon;
    QIcon icon;
    QMenu menu;
    QAction *a1, *a2;
    QActionGroup *startstopgroup, *startstopgroupkeyboard, *eventgroup, *mouseeventgroup;

    MainWindow w;
    icon.addFile(":/images/icon_km_active.png");

    // Find out what the keyboard devices are
    QString cmd1 = "/bin/sh -c \"grep -B6 120013 /proc/bus/input/devices\"";
    QProcess *process = new QProcess;
    process->start(cmd1);
    process->waitForBytesWritten();
    process->waitForFinished();
    QList<QByteArray> ttmp = process->readAll().split('\n');
    for (QList<QByteArray>::iterator j = ttmp.begin(); j != ttmp.end(); j++) {
        if ((*j).contains("Handler")) w.result.append((*j).split(char(' ')).at(3));
        if ((*j).contains("Name=")) w.names.append((*j).split(char('"')).at(1));
    }

    // Find out what the mouse devices are
    cmd1 = "/bin/sh -c \"grep -B4 mouse /proc/bus/input/devices\"";
    process->start(cmd1);
    process->waitForBytesWritten();
    process->waitForFinished();
    ttmp = process->readAll().split('\n');
    for (QList<QByteArray>::iterator j = ttmp.begin(); j != ttmp.end(); j++) {
        if ((*j).contains("Handler")) w.mouseresult.append((*j).split(char(' ')).at(2));
        if ((*j).contains("Name=")) w.mousenames.append((*j).split(char('"')).at(1));
    }

    const QString menu_properties = "Properties";
    const QString menu_quit = "Quit";

    // Add the keyboard event selector
    eventgroup = new QActionGroup(0);
    for (int i=0; i<w.result.count(); i++) {
        a1 = menu.addAction(w.result.at(i)+" - "+w.names.at(i)+" (keyboard)");
        a1->setCheckable(true);
        if (i == 0) {
            a1->setChecked(true);
            w.index_of_active_device = i;
        } else a1->setChecked(false);
        eventgroup->addAction(a1);
    }

    // Add the start/stop actions
    const QString menu_start_keyboard = "Start capturing keyboard";
    const QString menu_stop_keyboard = "Stop capturing keyboard";
    startstopgroupkeyboard = new QActionGroup(0);
    a1 = menu.addAction(menu_start_keyboard);
    a1->setCheckable(true);
    a1->setChecked(true);
    startstopgroupkeyboard->addAction(a1);
    a1 = menu.addAction(menu_stop_keyboard);
    a1->setCheckable(true);
    a1->setChecked(false);
    startstopgroupkeyboard->addAction(a1);
    menu.addSeparator();

    //menu.addSeparator();

    // Add the mouse event selector
    mouseeventgroup = new QActionGroup(0);
    for (int i=0; i<w.mouseresult.count(); i++) {
        a2 = menu.addAction(w.mouseresult.at(i)+" - "+w.mousenames.at(i)+" (mouse)");
        a2->setCheckable(true);
        if (i == 0) {
            a2->setChecked(true);
            w.index_of_active_mousedevice = i;
        } else a2->setChecked(false);
        mouseeventgroup->addAction(a2);
    }
    //menu.addSeparator();

    // Add the start/stop actions
    const QString menu_start = "Start capturing mouse";
    const QString menu_stop = "Stop capturing mouse";
    startstopgroup = new QActionGroup(0);
    a1 = menu.addAction(menu_start);
    a1->setCheckable(true);
    a1->setChecked(true);
    startstopgroup->addAction(a1);
    a1 = menu.addAction(menu_stop);
    a1->setCheckable(true);
    a1->setChecked(false);
    startstopgroup->addAction(a1);

    menu.addSeparator();

    menu.addAction(menu_properties);

    menu.addAction(menu_quit);

    QObject::connect(&menu, SIGNAL(triggered(QAction *)), &w, SLOT(event_triggered(QAction *)));
    QObject::connect(&w.t, SIGNAL(return_read(const char *, quint32, quint32)), &w.display, SLOT(return_read(const char *, quint32, quint32)));
    QObject::connect(&w.t, SIGNAL(return_special_keys(quint32)), &w.display_mouse, SLOT(return_special_keys(quint32)));
    QObject::connect(&w.tmouse, SIGNAL(return_read(quint32, QPoint)), &w.display_mouse, SLOT(return_read(quint32, QPoint)));
    //QObject::connect(&t, SIGNAL(return_read(void)), qApp, SLOT(aboutQT()));

    systrayicon.setIcon(icon);
    systrayicon.setContextMenu(&menu);
    systrayicon.show();

    //w.show();
    a.setQuitOnLastWindowClosed(false);

    w.set_systray_object(&systrayicon);
    w.t.set_strings();
    w.t.set_device(w.result.at(w.index_of_active_device));
    w.t.setup();
    w.t.start();

    w.tmouse.set_device(w.mouseresult.at(w.index_of_active_mousedevice));
    w.tmouse.setup();
    w.tmouse.start();

    return a.exec();

}

