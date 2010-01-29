#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "rs_io.h"
#include "i2c_adapter.h"
#include "wake.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QRegExpValidator *validator = new QRegExpValidator(QRegExp("[0-9a-fA-F ]{1,}"), this);
    dev = new class Dev;

    ui->setupUi(this);
    ui->leWakeData->setValidator(validator);

    for (int i=0;i<20;i++) if (AccessPort(i)==1)  ui->cbxPort->addItem(QString("COM%1").arg(i));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//---------------------------- Text2Hex: ------------------------------------
QByteArray intToByteArray(int value)
{
  QByteArray ba;
  ba.append(value);
  if ((value >> 8) & 0xff) ba.append(value >> 8);
  if ((value >> 16) & 0xff) ba.append(value >> 16);
  if ((value >> 24) & 0xff) ba.append(value >> 24);
  return ba;
}


void MainWindow::Text2Hex(QString s, QByteArray *ba)
{
    bool ok;
    QStringList list1 = s.split(" ", QString::SkipEmptyParts);
    list1 = list1.filter(QRegExp("^[0-9a-fA-F]{1,8}$"));
    for (int i=0;i<list1.size();i++)
      ba->append(intToByteArray(list1.at(i).toInt(&ok, 16)));
}

void MainWindow::on_pbConnect_clicked()
{
  char info[32];
  int port;
  bool ok;
  if (!connected)
  {
    port = ui->cbxPort->currentText().remove(0,3).toInt(&ok,10);
    if (ok) //OpenPort(port); else return;
    if (dev->OpenDevice(port) == EXIT_FAILURE){ui->statusBar->showMessage("OpenDevice ERROR", 2000); return;}
    if(dev->GetInfo(info) == EXIT_FAILURE) {ui->statusBar->showMessage("Get_Info ERROR", 2000); ClosePort(); return;}
    ui->statusBar->showMessage("On-Line",0);
    ui->statusBar->addPermanentWidget(&info_bar,0);
    info_bar.setText(QString(info));
    ui->pbConnect->setChecked(true);
    ui->pbConnect->setText("Disconnect");
    connected = true;
    ui->cbxPort->setEnabled(false);
  }
  else
  {
    ClosePort();
    ui->pbConnect->setText("Connect");
    ui->pbConnect->setChecked(false);
    ui->statusBar->showMessage("Disconnected",2000);
    info_bar.clear();
    connected = false;
    ui->cbxPort->setEnabled(true);
  }
}

void MainWindow::on_pbSend_clicked()
{
  unsigned char data[518];
  QString s;
  unsigned char addr, cmd, len;
  QByteArray ba;

  Text2Hex(ui->leWakeData->text(), &ba);
  if (wake_tx_frame(ui->hsbAddr->value(), ui->hsbCmd->value(), ba.size(), (unsigned char *)ba.constData()) < 0)
    {ui->teLog->append("wake_tx_frame error"); return;}
  len = wake_get_tx_buffer(data);
  s = "<font color=#ff0000>TX: </font>";
  s += QString("<font color=#c0c0c0>%1 </font>").arg(data[0],2,16,QChar('0')); // FEND
  s += QString("<font color=#808080>%1 </font>").arg(data[1],2,16,QChar('0')); // ADDR or CMD
  s += QString("<font color=#808080>%1 </font>").arg(data[2],2,16,QChar('0')); // N
  s += "<font color=#ff0000>";
  // todo: show byte stuffing, move to other function
  for(int i=3;i<len-1;i++) s += QString("%1 ").arg(data[i],2,16,QChar('0')); // data
  s += "</font>";
  s += QString("<font color=#808080>%1 </font>").arg(data[len-1],2,16,QChar('0')); // crc
  ui->teLog->append(s.toLocal8Bit());
  if (wake_rx_frame(200, &addr, &cmd, &len, data) < 0) {ui->teLog->append("wake_rx_frame error"); return;}
  s = "<font color=green>RX: ";
  for(int i=0;i<len;i++) s += QString("%1 ").arg(data[i],2,16,QChar('0'));
  s += "</font>";
  ui->teLog->append(s.toLocal8Bit());
}

void MainWindow::on_pbI2C_test_clicked()
{

}
