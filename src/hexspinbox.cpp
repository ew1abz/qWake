#include "hexspinbox.h"
#include <QValidator>

HexSpinBox::HexSpinBox(QWidget *parent)
    : QSpinBox(parent)
{
  //int textWidth = fontMetrics().width(prefix()+"CC");
  //setMinimumWidth(textWidth);
  //setMinimumWidth(200);
  validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,8}"), this);
}

QValidator::State HexSpinBox::validate(QString &text, int &pos) const
{
  QString t = text;

  QRegExp exp;
  if      (maximum() <= 0x0000000f) exp = QRegExp("[0-9A-Fa-f]{1,1}");
  else if (maximum() <= 0x000000ff) exp = QRegExp("[0-9A-Fa-f]{1,2}");
  else if (maximum() <= 0x00000fff) exp = QRegExp("[0-9A-Fa-f]{1,3}");
  else if (maximum() <= 0x0000ffff) exp = QRegExp("[0-9A-Fa-f]{1,4}");
  else if (maximum() <= 0x000fffff) exp = QRegExp("[0-9A-Fa-f]{1,5}");
  else if (maximum() <= 0x00ffffff) exp = QRegExp("[0-9A-Fa-f]{1,6}");
  else if (maximum() <= 0x0fffffff) exp = QRegExp("[0-9A-Fa-f]{1,7}");
  else exp = QRegExp("[0-9A-Fa-f]{1,8}");

  t.remove(0,prefix().length());
  validator->setRegExp(exp);
  return validator->validate(t, pos);
}

int HexSpinBox::valueFromText(const QString &text) const
{
  bool ok;
  QString t = text;
  t.remove(0,prefix().length());
  return t.toInt(&ok,16);
}

QString HexSpinBox::textFromValue(int value) const
{
  if (maximum() <= 0x0000000f) return QString("%1").arg(value,1,16,QChar('0')).toUpper();
  if (maximum() <= 0x000000ff) return QString("%1").arg(value,2,16,QChar('0')).toUpper();
  if (maximum() <= 0x00000fff) return QString("%1").arg(value,3,16,QChar('0')).toUpper();
  if (maximum() <= 0x0000ffff) return QString("%1").arg(value,4,16,QChar('0')).toUpper();
  if (maximum() <= 0x000fffff) return QString("%1").arg(value,5,16,QChar('0')).toUpper();
  if (maximum() <= 0x00ffffff) return QString("%1").arg(value,6,16,QChar('0')).toUpper();
  if (maximum() <= 0x0fffffff) return QString("%1").arg(value,7,16,QChar('0')).toUpper();
  return QString("%1").arg(value,8,16,QChar('0')).toUpper();
}
