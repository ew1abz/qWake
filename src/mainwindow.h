#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "hexspinbox.h"
#include "qextserialport/qextserialport.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int set_rx_to(int to);
    int tx_buffer(unsigned char* data, int size);
    int rx_byte (unsigned char* byte);
    int (MainWindow::*p_set_rx_to)(int);


protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    void writeSettings();
    void readSettings();
    QextSerialPort *port;
    //bool connected;
    QLabel info_bar;
    void Text2Hex(QString s, QByteArray *ba);
    void show_tx_log(char * clear_data, int size);
    void show_rx_log(char * clear_data, int size);
    void newRow(int row);
    QSignalMapper signalMapper;

private slots:
    void on_tbBatch_clicked();
    void on_tbAddCmd_clicked();
    void on_pbPortSend_clicked();
    void on_btClear_clicked();
    void on_pbConnect_clicked();
    void on_pbSend_clicked();
    void slotRun(int);
};

#endif // MAINWINDOW_H
