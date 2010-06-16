#include "hexlineeditdelegate.h"

hexLineEditDelegate::hexLineEditDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *hexLineEditDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
  QLineEdit *editor = new QLineEdit(parent);
  //editor->setMinimum(0);
  //editor->setMaximum(255);

  return editor;
}

void hexLineEditDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
  QString text = index.model()->data(index, Qt::EditRole).toString();

  QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
  //QRegExpValidator *validator = static_cast<QRegExpValidator*>(QRegExp("[0-9a-fA-F ]{1,}"), editor);
  QRegExpValidator *validator = new QRegExpValidator(QRegExp("[0-9a-fA-F ]{1,}"), editor);
  lineEdit->setValidator(validator);
  lineEdit->setText(text);
}

void hexLineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
  QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
  //lineEdit->interpretText();
  QString text = lineEdit->text();

  model->setData(index, text, Qt::EditRole);
  //model->setData(index, QString("%1").arg(value,2,16,QChar('0')).toUpper(), Qt::EditRole);
}

void hexLineEditDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}


