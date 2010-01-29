#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "hexspinbox.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    bool connected;
    class Dev *dev;
    QLabel info_bar;
    void Text2Hex(QString s, QByteArray *ba);


private slots:
    void on_pbI2C_test_clicked();
    void on_pbConnect_clicked();
    void on_pbSend_clicked();
};

#endif // MAINWINDOW_H
