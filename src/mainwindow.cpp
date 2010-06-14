#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
//#include "comport_lin.h"
#include "wake.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QRegExpValidator *validator = new QRegExpValidator(QRegExp("[0-9a-fA-F ]{1,}"), this);

    QStringList *sl = new QStringList;
    *sl << "Name" << "Addr" << "Cmd" << "Data" << "Enable" << "Start" << "Incoming data view";

    ui->setupUi(this);

    connect(&signalMapper, SIGNAL(mapped(int)), this, SLOT(slotRun(int)));

    ui->leWakeData->setValidator(validator);
    ui->tableWidget->setHorizontalHeaderLabels(*sl);
    ui->tableWidget->setColumnWidth(0,100);
    ui->tableWidget->setColumnWidth(1,40);
    ui->tableWidget->setColumnWidth(2,40);
    ui->tableWidget->setColumnWidth(3,180);
    ui->tableWidget->setColumnWidth(4,52);
    ui->tableWidget->setColumnWidth(5,44);
    ui->tableWidget->setColumnWidth(6,180);

    ui->lePortData->setValidator(validator);
    //for (int i=0;i<20;i++) if (AccessPort(i)==1)  ui->cbxPort->addItem(QString("COM%1").arg(i));
    ui->cbxPort->addItem("/dev/ttyS0");
    ui->cbxPort->addItem("/dev/ttyS1");
    ui->cbxPort->addItem("/dev/ttyUSB0");
    ui->cbxPort->addItem("/dev/ttyUSB1");
    ui->cbxPort->addItem("COM1");
    ui->cbxPort->addItem("COM2");
    ui->cbxPort->addItem("COM3");
    ui->cbxPort->addItem("COM4");
    ui->cbxPort->addItem("COM5");
    ui->cbxPort->addItem("COM6");
    ui->cbxPort->addItem("COM7");
    ui->cbxPort->addItem("COM8");
    ui->cbxPort->addItem("COM9");

    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
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

void MainWindow::readSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,"qWake", "Config");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  ui->tabWidget->setCurrentIndex(settings.value("tab").toInt());
  ui->cbxLogLevel->setCurrentIndex(settings.value("logLevel").toInt());
  ui->cbxASCII->setChecked(settings.value("ascii").toBool());

  int i= ui->cbxSpeed->findText(settings.value("speed").toString());
  if (i) ui->cbxSpeed->setCurrentIndex(i);

  ui->sbxTimeout->setValue(settings.value("timeout").toInt());
  if (settings.value("connected").toBool())
  {
    int i= ui->cbxPort->findText(settings.value("port").toString());
    if (i)
    {
      ui->cbxPort->setCurrentIndex(ui->cbxPort->findText(settings.value("port").toString()));
      on_pbConnect_clicked();
    }
  }
  int n = settings.beginReadArray("commands");
  for (int i = 0; i < n; ++i)
  {
    settings.setArrayIndex(i);
    newRow(i);
    ui->tableWidget->setColumnWidth(i, settings.value("width").toInt());
    ui->tableWidget->item(i,0)->setText(settings.value("name").toString());
    ui->tableWidget->item(i,1)->setText(settings.value("addr").toString());
    ui->tableWidget->item(i,2)->setText(settings.value("cmd").toString());
    ui->tableWidget->item(i,3)->setText(settings.value("data").toString());
    ((QCheckBox*)ui->tableWidget->cellWidget(i,4))->setChecked(settings.value("enable").toBool());
    ui->tableWidget->item(i,6)->setText(settings.value("view").toString());
  }
  settings.endArray();

  resize(size);
  move(pos);
}

void MainWindow::writeSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,"qWake", "Config");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
  settings.setValue("tab",ui->tabWidget->currentIndex());
  settings.setValue("port", ui->cbxPort->currentText());
  settings.setValue("speed", ui->cbxSpeed->currentText());
  settings.setValue("timeout", ui->sbxTimeout->value());
  settings.setValue("connected", port->isOpen());
  settings.setValue("logLevel", ui->cbxLogLevel->currentIndex());
  settings.setValue("ascii", ui->cbxASCII->isChecked());

  settings.beginWriteArray("commands");
  for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
  {
    settings.setArrayIndex(i);
    settings.setValue("width", ui->tableWidget->columnWidth(i));
    settings.setValue("name", ui->tableWidget->item(i,0)->text());
    settings.setValue("addr", ui->tableWidget->item(i,1)->text());
    settings.setValue("cmd", ui->tableWidget->item(i,2)->text());
    settings.setValue("data", ui->tableWidget->item(i,3)->text());
    settings.setValue("enable", ((QCheckBox*)ui->tableWidget->cellWidget(i,4))->isChecked());
    settings.setValue("view", ui->tableWidget->item(i,6)->text());
  }
  settings.endArray();
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
  if (!port->isOpen())
  {
    port = new QextSerialPort(ui->cbxPort->currentText());
    if (ui->cbxSpeed->currentText() == "1200") port->setBaudRate(BAUD1200);
    else if (ui->cbxSpeed->currentText() == "38400") port->setBaudRate(BAUD38400);
    else if (ui->cbxSpeed->currentText() == "57600") port->setBaudRate(BAUD57600);
    else if (ui->cbxSpeed->currentText() == "115200") port->setBaudRate(BAUD115200);

    port->setFlowControl(FLOW_OFF);
    port->setParity(PAR_NONE);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->setTimeout(0,ui->sbxTimeout->value());
    if (port->open(QIODevice::ReadWrite) == 0)
      {ui->statusBar->showMessage(port->errorString(),0); return;}
    qDebug("is open: %d", port->isOpen());

    wake_init(port);

    ui->statusBar->showMessage("On-Line",0);
    ui->pbConnect->setChecked(true);
    ui->pbConnect->setText("Disconnect");
    //connected = true;
    ui->cbxPort->setEnabled(false);
    ui->cbxSpeed->setEnabled(false);
    ui->sbxTimeout->setEnabled(false);
  }
  else
  {
    port->close();
    ui->pbConnect->setText("Connect");
    ui->pbConnect->setChecked(false);
    ui->statusBar->showMessage("Disconnected",2000);
    //connected = false;
    ui->cbxPort->setEnabled(true);
    ui->cbxSpeed->setEnabled(true);
    ui->sbxTimeout->setEnabled(true);
  }
}

void MainWindow::show_tx_log(char * clear_data, int size)
{
  unsigned char data[518];
  int len, i;
  QString s;

  if (ui->cbxLogLevel->currentIndex() == 0 || ui->cbxLogLevel->currentIndex() == 1) // full raw
  {
    len = wake_get_tx_raw_buffer((char*)data);
    s = "<font color=#ff0000>TX: </font>";
    i = 0;
    s += QString("<font color=#c0c0c0>%1 </font>").arg(data[i++],2,16,QChar('0')); // FEND
    if (data[i] & 0x80)
      s += QString("<font color=black>%1 </font>").arg(data[i++],2,16,QChar('0')); // ADDR
    s += QString("<font color=#808080>%1 </font>").arg(data[i++],2,16,QChar('0')); // CMD
    s += QString("<font color=#808080>%1 </font>").arg(data[i++],2,16,QChar('0')); // N
    // data
    s += "<font color=#ff0000>";
    for(;i<len-1;i++)
    if ((unsigned char)data[i] == 0xdb) // with stuffing
        {
          if (ui->cbxLogLevel->currentIndex() == 0)
          {
            s += QString("<font color=#b5a642>%1 ").arg(data[i++],2,16,QChar('0')); // stuffed data
            s += QString("%1 </font>").arg(data[i],2,16,QChar('0'));                // stuffed data
          }
          else switch (data[++i])
               {
               case 0xdc: s += "c0 "; break;
               case 0xdd: s += "db "; break;
               default:   s += "XX "; break;
               }
        } else s += QString("%1 ").arg(data[i],2,16,QChar('0')); // data
    s += "</font>";
    s += QString("<font color=#808080>%1 </font>").arg(data[len-1],2,16,QChar('0')); // crc
  }

  if (ui->cbxLogLevel->currentIndex() == 2) // logic only
  {
    s = "<font color=#ff0000>TX: ";
    for(i=0;i<size;i++) s += QString("%1 ").arg(clear_data[i],2,16,QChar('0')); // data
    s += "</font>";
  }
  if (ui->cbxASCII->isChecked())
  {
    s += "<font color=#0000ff>";
    for(i=0;i<size;i++) if (clear_data[i]>=' ') s += QChar(clear_data[i]); else s += '.';
    s += "</font>";
  }
  ui->teLog->append(s.toLocal8Bit());
}

void MainWindow::show_rx_log(char * clear_data, int size)
{
  unsigned char data[518];
  int len, i;
  QString s;

  if (ui->cbxLogLevel->currentIndex() == 0 || ui->cbxLogLevel->currentIndex() == 1) // raw
  {
    len = wake_get_rx_raw_buffer((char*)data);
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
    for(i=0;i<size;i++) s += QString("%1 ").arg((unsigned char)clear_data[i],2,16,QChar('0')); // data
    s += "</font>";
  }
  if (ui->cbxASCII->isChecked())
  {
    s += "<font color=#0000ff>";
    s += QString().fromLocal8Bit(clear_data, size);
    s += "</font>";
  }
  ui->teLog->append(s.toLocal8Bit());
}

void MainWindow::on_pbSend_clicked()
{
  char data[518];
  QString s;
  unsigned char addr, cmd, len;
  QByteArray ba;
  int res;

  Text2Hex(ui->leWakeData->text(), &ba);
  res = wake_tx_frame(ui->hsbAddr->value(), ui->hsbCmd->value(), ba.size(), ba.constData());
  if (res < 0)
    {ui->teLog->append("wake_tx_frame error"); qDebug("%d", res); return;}
  show_tx_log((char *)ba.constData(), ba.size());

  if (wake_rx_frame(200, &addr, &cmd, &len, data) < 0)
   {ui->teLog->append("wake_rx_frame error"); return;}
  show_rx_log(data ,len);
}

void MainWindow::on_btClear_clicked()
{
  ui->teLog->clear();
}

void MainWindow::on_pbPortSend_clicked()
{
  QByteArray ba;
  QString s;
  int res;

  Text2Hex(ui->lePortData->text(), &ba);
  res = port->write(ba.constData(), ba.size());
  if (res < 0) {ui->teLog->append("portWrite error"); return;}
  if (res != ba.size()) {ui->teLog->append("portWrite len error"); return;}

  s = "<font color=red>RAW TX: ";
  foreach(unsigned char data, ba) s += QString("%1 ").arg(data,2,16,QChar('0'));
  s += "</font>";

  if (ui->cbxASCII->isChecked())
  {
    s += "<font color=#0000ff>";
    foreach(int data, ba) if (data>0x30) s += QChar(data); else s += '.';
    s += "</font>";
  }
  ui->teLog->append(s.toLocal8Bit());
}

void MainWindow::newRow(int row)
{
  ui->tableWidget->insertRow(row);
  ui->tableWidget->setRowHeight(row, 18);
  ui->tableWidget->verticalHeader()->setResizeMode(row, QHeaderView::Fixed);
  // name
  QTableWidgetItem *item0 = new QTableWidgetItem;
  ui->tableWidget->setItem(row,0, item0);
  ui->tableWidget->item(row,0)->setText(QString("Command %1").arg(row,3,10,QChar('0')));
  // addr
  QTableWidgetItem *item1 = new QTableWidgetItem;
  ui->tableWidget->setItem(row,1, item1);
  ui->tableWidget->item(row,1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  ui->tableWidget->item(row,1)->setText("00");
  // cmd
  QTableWidgetItem *item2 = new QTableWidgetItem;
  ui->tableWidget->setItem(row,2, item2);
  ui->tableWidget->item(row,2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  ui->tableWidget->item(row,2)->setText(QString("%1").arg(row,2,16,QChar('0')));
  // data
  QTableWidgetItem *item3 = new QTableWidgetItem;
  ui->tableWidget->setItem(row,3, item3);
  ui->tableWidget->item(row,3)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
 // ui->tableWidget->item(row,3)->setFlags(Qt::ItemIsUserCheckable);
  // enable
  QCheckBox *cbxSel = new QCheckBox;
  ui->tableWidget->setCellWidget(row,4,cbxSel);
  //cbxSel->setText("t");
  cbxSel->setGeometry(10,0,18,18);
  // start
  QToolButton *run = new QToolButton;
  run->setIcon(QIcon(":/Start.ico"));
  ui->tableWidget->setCellWidget(row,5,run);

  QTableWidgetItem *item6 = new QTableWidgetItem;
  ui->tableWidget->setItem(row,6, item6);
  signalMapper.setMapping(run, row);
  connect(run, SIGNAL(clicked()), &signalMapper, SLOT (map()));

  ui->tableWidget->setCurrentCell(row,0);
}

void MainWindow::on_tbAddCmd_clicked()
{
  newRow(ui->tableWidget->rowCount());
}

void MainWindow::slotRun(int row)
{
  //QByteArray ba;
  bool res;
  //QString s;
  //char d[128];
  unsigned char addr = ui->tableWidget->item(row,1)->text().toInt(&res,16);
  unsigned char cmd = ui->tableWidget->item(row,2)->text().toInt(&res,16);
  //bool sel = ((QCheckBox*)ui->tableWidget->cellWidget(i,5))->isChecked();

  //Text2Hex(ui->tableWidget->item(row,4)->text(), &ba);

  qDebug("addr %d cmd %d", addr, cmd);
}
