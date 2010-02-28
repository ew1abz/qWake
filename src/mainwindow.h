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
    QLabel info_bar;
    void Text2Hex(QString s, QByteArray *ba);
    void show_tx_log(unsigned char * clear_data, int size);
    void show_rx_log(unsigned char * clear_data, int size);
    void newRow(int row);

private slots:
    void on_toolButton_clicked();
    void on_pbPortSend_clicked();
    void on_btClear_clicked();
    void on_pbConnect_clicked();
    void on_pbSend_clicked();
};

#endif // MAINWINDOW_H
