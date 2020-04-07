/* 
 * MIT License
 * z80.ro
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include "worker.h"
#include "worker_mouse.h"
#include "transparent.h"
#include "transparent_mouse.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;
    QMessageBox box;
    const char *queue[10];
    int qi = 0;
    QSystemTrayIcon *systrayicon;

private slots:
        void event_triggered(QAction *action);
        void quit_application();
        void hide_application();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void set_systray_object(QSystemTrayIcon *sti);
    worker t;
    workermouse tmouse;
    transparent display;
    transparent_mouse display_mouse;
    int index_of_active_device, index_of_active_mousedevice;
    QStringList result, mouseresult, names, mousenames;

};

#endif // MAINWINDOW_H
