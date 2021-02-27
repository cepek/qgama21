#ifndef LINE_EDIT_ITEM_DELEGATE_H
#define LINE_EDIT_ITEM_DELEGATE_H

#include <QStyledItemDelegate>

/*
class QWidget;
class QModelIndex;
class QVariant;
*/

class LineEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    LineEditDelegate(QWidget *parent = nullptr);

    // void paint(QPainter *painter, const QStyleOptionViewItem &option,
    //            const QModelIndex &index) const;
    // QSize sizeHint(const QStyleOptionViewItem &option,
    //                const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData   (QWidget *editor, const QModelIndex &index) const;
    void setModelData    (QWidget *editor, QAbstractItemModel *model,
                          const QModelIndex &index) const;
};

#endif // LINE_EDIT_ITEM_DELEGATE_H
