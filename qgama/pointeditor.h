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

#ifndef POINTEDITOR_H
#define POINTEDITOR_H

#include <QWidget>

namespace GNU_gama { namespace local {
    class PointData;
}}

class PointTableModel;
class QMenu;
class QTableView;

class PointEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit PointEditor(QWidget *parent = nullptr);
    ~PointEditor();

    void connectPointData(GNU_gama::local::PointData* pd);
    void enableEdit(bool edit);
    
private:
    QTableView* tableView;
    PointTableModel* model;
    QMenu* pointMenu;
    bool   readonly;
    int    pointLogicalIndex;

signals:
    void warning(QString);

private slots:
    void pointContextMenu(QPoint p);
    void pointDelete();
};

#endif // POINTEDITOR_H
