/*
  GNU Gama Qt based GUI
  Copyright (C) 2013 Ales Cepek <cepek@gnu.org>

  This file is part of GNU Gama.

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pointtypecombobox.h"

#include <QComboBox>

PointTypeComboBox::PointTypeComboBox(QObject* parent)
    : QItemDelegate(parent)
{
}


QWidget *PointTypeComboBox::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  QComboBox* editor = new QComboBox(parent);

  editor->addItem("");       // unused
  editor->addItem("fixed");
  editor->addItem("constr");
  editor->addItem("free");

  return editor;
}

void PointTypeComboBox::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  QString value = index.model()->data(index, Qt::EditRole).toString();
  int ind;
  if      (value == "fixed")  ind = 1;
  else if (value == "constr") ind = 2;
  else if (value == "free")   ind = 3;
  else                        ind = 0;
  comboBox->setCurrentIndex(ind);
}

void PointTypeComboBox::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void PointTypeComboBox::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index) const
{
  editor->setGeometry(option.rect);
}

/*
void PointTypeComboBox::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
}
*/

