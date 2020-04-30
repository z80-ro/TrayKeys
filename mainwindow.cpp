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
        display_mouse.start_capturing = true;
        systrayicon->setIcon(QIcon(":/images/icon_active.png"));
    }
    if (action->text() == "Stop capturing") {
        display.start_capturing = false;
        display_mouse.start_capturing = false;
        systrayicon->setIcon(QIcon(":/images/icon_stopped.png"));
    }
    if (action->text().contains("(keyboard)")) {
        qDebug() << "New keyboard device: " << action->text();
        t.set_device(action->text().split(char('-')).at(0).trimmed());
        t.restart_capture();
    }
    if (action->text().contains("(mouse)")) {
        qDebug() << "New mouse device: " << action->text();
        tmouse.set_device(action->text().split(char('-')).at(0).trimmed());
        tmouse.restart_capture();
    }
}

void MainWindow::quit_application() {
    if (t.isRunning()) {
        t.terminate();
        t.wait();
    }
    if (tmouse.isRunning()) {
        tmouse.terminate();
        tmouse.wait();
    }
    QApplication::quit();
}

void MainWindow::hide_application() {
    this->hide();
}


void MainWindow::scroll_speed_change(int value) {
    //qDebug("Slider value changed to : %d", value);
    display.change_speed(value);

}

void MainWindow::adaptive_scroll(bool value) {
    //qDebug("Adaptive value : %d", value);
    display.adaptive_speed(value);
}

void MainWindow::reverse_scroll(bool value) {
    //qDebug("Reverse scroll value : %d", value);
    display_mouse.reverse_scroll(value);
}

void MainWindow::new_width(int w) {
    display.set_width(w);
    //qDebug("New width: %d", w);
}

void MainWindow::new_position(int x) {
    display.set_pos_x(x);
    //qDebug("New position: %d", x);
}

void MainWindow::new_adaptive_factor(int f) {
    display.set_adaptive_factor(f);
    //qDebug("New factor: %d", f);
}

void MainWindow::small_height_clicked(bool) {
    display.set_height_suffix("_small", 0);
}

void MainWindow::medium_height_clicked(bool) {
    display.set_height_suffix("_medium", 1);
}

void MainWindow::large_height_clicked(bool) {
    display.set_height_suffix("", 2);
}
