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

#ifndef POINTTYPECOMBOBOX_H
#define POINTTYPECOMBOBOX_H

#include <QItemDelegate>

class QModelIndex;
class QWidget;
class QVariant;

class PointTypeComboBox : public QItemDelegate
{
    Q_OBJECT
public:
    PointTypeComboBox(QObject *parent = nullptr);

    QWidget* createEditor (QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const;
    void     setModelData (QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //       void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // POINTTYPECOMBOBOX_H
