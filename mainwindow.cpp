/* 
 * MIT License
 * z80.ro
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::set_systray_object(QSystemTrayIcon *sti) {
    this->systrayicon = sti;
}

void MainWindow::event_triggered(QAction *action)
{
    if (action->text() == "Quit") quit_application();
    if (action->text() == "Properties") this->show();
    if (action->text() == "Start capturing") {
        display.start_capturing = true;
        systrayicon->setIcon(QIcon(":/icon_active.png"));
    }
    if (action->text() == "Stop capturing") {
        display.start_capturing = false;
        systrayicon->setIcon(QIcon(":/icon_stopped.png"));
    }
    if (action->text().contains("event")) {
        qDebug() << "New device: " << action->text();
        t.set_device(action->text().split(char('-')).at(0).trimmed());
        t.restart_capture();
    }
}

void MainWindow::quit_application() {
    if (t.isRunning()) {
        t.terminate();
        t.wait();
    }
    QApplication::quit();
}

void MainWindow::hide_application() {
    this->hide();
}
