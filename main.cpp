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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSystemTrayIcon systrayicon;
    QIcon icon;
    QMenu menu;
    QAction *a1;
    QActionGroup *startstopgroup, *eventgroup;
    QString tmpresult;

    MainWindow w;
    icon.addFile(":/icon_active.png");


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


    const QString menu_properties = "Properties";
    const QString menu_start = "Start capturing";
    const QString menu_stop = "Stop capturing";
    const QString menu_quit = "Quit";

    eventgroup = new QActionGroup(0);

    for (int i=0; i<w.result.count(); i++) {
        a1 = menu.addAction(w.result.at(i)+" - "+w.names.at(i));
        a1->setCheckable(true);
        if (i == 0) {
            a1->setChecked(true);
            w.index_of_active_device = i;
        } else a1->setChecked(false);
        eventgroup->addAction(a1);
    }

    menu.addSeparator();


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

    return a.exec();

}

