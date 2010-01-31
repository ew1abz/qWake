#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "comport_lin.h"
#include "wake.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QRegExpValidator *validator = new QRegExpValidator(QRegExp("[0-9a-fA-F ]{1,}"), this);
    QStringList *sl = new QStringList;
    *sl << "Name" << "Cmd" << "Data" << "Cycle" << "Start";
    QCheckBox *ch = new QCheckBox;
    QToolButton *tb = new QToolButton;
    tb->setIcon(QIcon(":/Start.ico"));

    ui->setupUi(this);
    ui->leWakeData->setValidator(validator);
    ui->tableWidget->setHorizontalHeaderLabels(*sl);
    ui->tableWidget->setColumnWidth(0,80);
    ui->tableWidget->setColumnWidth(1,40);
    ui->tableWidget->setColumnWidth(2,180);
    ui->tableWidget->setColumnWidth(3,32);
    ui->tableWidget->setColumnWidth(4,32);
    ui->tableWidget->setCellWidget(0,3,ch);
    ui->tableWidget->setCellWidget(0,4,tb);

    //for (int i=0;i<20;i++) if (AccessPort(i)==1)  ui->cbxPort->addItem(QString("COM%1").arg(i));
    ui->cbxPort->addItem("/dev/ttyS1");
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
  if (!connected)
  {
    if (portOpen(ui->cbxPort->currentText().toLocal8Bit()) == EXIT_FAILURE) {ui->statusBar->showMessage("portOpen ERROR", 2000); return;}
    portSetOptions(115200,0);
    wake_init(portWrite, portRead);
//    if(dev->GetInfo(info) == EXIT_FAILURE) {ui->statusBar->showMessage("Get_Info ERROR", 2000); portClose(); return;}
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
    portClose();
    ui->pbConnect->setText("Connect");
    ui->pbConnect->setChecked(false);
    ui->statusBar->showMessage("Disconnected",2000);
    info_bar.clear();
    connected = false;
    ui->cbxPort->setEnabled(true);
  }
}

void MainWindow::show_tx_log(unsigned char * clear_data, int size)
{
  unsigned char data[518];
  int len, i;
  QString s;

  if (ui->cbxRaw->isChecked())
  {
    len = wake_get_tx_raw_buffer(data);
    s = "<font color=#ff0000>TX: </font>";
    i = 0;
    s += QString("<font color=#c0c0c0>%1 </font>").arg(data[i++],2,16,QChar('0')); // FEND
    if (data[i] & 0x80)
      s += QString("<font color=black>%1 </font>").arg(data[i++],2,16,QChar('0')); // ADDR
    s += QString("<font color=#808080>%1 </font>").arg(data[i++],2,16,QChar('0')); // CMD
    s += QString("<font color=#808080>%1 </font>").arg(data[i++],2,16,QChar('0')); // N
    s += "<font color=#ff0000>";
    for(;i<len-1;i++) s += QString("%1 ").arg(data[i],2,16,QChar('0')); // data
    s += "</font>";
    s += QString("<font color=#808080>%1 </font>").arg(data[len-1],2,16,QChar('0')); // crc
  }
  else
  {
    s = "<font color=#ff0000>TX: ";
    for(i=0;i<size;i++) s += QString("%1 ").arg(clear_data[i],2,16,QChar('0')); // data
    s += "</font>";
  }
  if (ui->cbxASCII->isChecked())
  {
    s += "<font color=#0000ff>";
    for(i=0;i<size;i++) if (clear_data[i]>0x30) s += QChar(clear_data[i]); else s += '.';
    s += "</font>";
  }
  ui->teLog->append(s.toLocal8Bit());
}

void MainWindow::show_rx_log(unsigned char * clear_data, int size)
{
  unsigned char data[518];
  int len, i;
  QString s;

  if (ui->cbxRaw->isChecked())
  {
    len = wake_get_rx_raw_buffer(data);
    s = "<font color=green>RX: ";
    i = 0;
    s += QString("<font color=#c0c0c0>%1 </font>").arg(data[i++],2,16,QChar('0')); // FEND
    if (data[i] & 0x80)
      s += QString("<font color=black>%1 </font>").arg(data[i++],2,16,QChar('0')); // ADDR
    s += QString("<font color=#808080>%1 </font>").arg(data[i++],2,16,QChar('0')); // CMD
    s += QString("<font color=#808080>%1 </font>").arg(data[i++],2,16,QChar('0')); // N
    s += "<font color=green>";
    for(;i<len-1;i++) s += QString("%1 ").arg(data[i],2,16,QChar('0')); // data
    s += "</font>";
    s += QString("<font color=#808080>%1 </font>").arg(data[len-1],2,16,QChar('0')); // crc
  }
  else
  {
    s = "<font color=green>RX: ";
    for(i=0;i<size;i++) s += QString("%1 ").arg(clear_data[i],2,16,QChar('0')); // data
    s += "</font>";
  }
  if (ui->cbxASCII->isChecked())
  {
    s += "<font color=#0000ff>";
    for(i=0;i<size;i++) if (clear_data[i]>0x30) s += QChar(clear_data[i]); else s += '.';
    s += "</font>";
  }
  ui->teLog->append(s.toLocal8Bit());
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
  show_tx_log((unsigned char *)ba.constData() ,ba.size());

  if (wake_rx_frame(200, &addr, &cmd, &len, data) < 0)
   {ui->teLog->append("wake_rx_frame error"); return;}
  show_rx_log(data ,len);
}

void MainWindow::on_btClear_clicked()
{
  ui->teLog->clear();
}
