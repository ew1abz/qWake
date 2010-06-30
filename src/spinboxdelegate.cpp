#include "spinboxdelegate.h"
#include "hexspinbox.h"

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    HexSpinBox *editor = new HexSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(255);

    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    bool ok;
    int value = index.model()->data(index, Qt::EditRole).toString().toInt(&ok,16);

    HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    HexSpinBox *spinBox = static_cast<HexSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, QString("%1").arg(value,2,16,QChar('0')).toUpper(), Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
     editor->setGeometry(option.rect);
}



