#ifndef HEXLINEEDITDELEGATE_H
#define HEXLINEEDITDELEGATE_H

#include <QtGui>

class hexLineEditDelegate : public QItemDelegate
{
Q_OBJECT

public:
hexLineEditDelegate(QObject *parent = 0);

QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const;

void setEditorData(QWidget *editor, const QModelIndex &index) const;
void setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const;

void updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // HEXLINEEDITDELEGATE_H


