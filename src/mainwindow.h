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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    void writeSettings();
    void readSettings();
    QextSerialPort *port;
    QRegExpValidator *validator;
    QLabel info_bar;
    void Text2Hex(QString s, QByteArray *ba);
    void show_tx_log(char * clear_data, int size);
    void show_rx_log(char * clear_data, int size);
    void newRow(int row);
    void changeRows(int rowUp, int rowDown);
    QSignalMapper signalMapper;
    QLabel *statusTime;
    QLabel *statusConnect;
    QLabel *statusErrors;
    QLabel *statusCycles;

private slots:
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionOpen_frameset_triggered();
    void on_lineEditFramesetName_editingFinished();
    void on_actionSave_frameset_triggered();
    void on_btAdd_clicked();
    void on_btInsert_clicked();
    void on_btUp_clicked();
    void on_btDown_clicked();
    void on_btDel_clicked();
    void on_tbBatch_clicked();
    void on_pbPortSend_clicked();
    void on_btClear_clicked();
    void on_pbConnect_clicked();
    void on_pbSend_clicked();
    void slotRun(int);
};

#endif // MAINWINDOW_H
