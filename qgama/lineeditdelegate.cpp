
#include "lineeditdelegate.h"

#include <QLineEdit>
#include <QDebug>

LineEditDelegate::LineEditDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

// void LineEditDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
//                            const QModelIndex &index) const {}

// QSize LineEditDelegate::sizeHint(const QStyleOptionViewItem &option,
//                                const QModelIndex &index) const {}


QWidget * LineEditDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem & /*option*/,
                                            const QModelIndex & index) const
{
    auto editor = new QLineEdit(parent);
    // editor->setAlignment(Qt::AlignRight);
    editor->setAlignment(Qt::AlignmentFlag(index.model()->data(index,Qt::TextAlignmentRole).toInt()));
    return editor;
}

void LineEditDelegate::setEditorData(QWidget *weditor, const QModelIndex &index) const
{
    auto editor = static_cast<QLineEdit*>(weditor);
    editor->setText(index.data().toString());
}

void LineEditDelegate::setModelData(QWidget *weditor, QAbstractItemModel *model,
                                       const QModelIndex &index) const
{
    auto editor = static_cast<QLineEdit*>(weditor);
    model->setData(index, editor->text(), Qt::EditRole);
}
