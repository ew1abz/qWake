#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));
    w.show();
    return a.exec();
}
