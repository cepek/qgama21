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

#ifndef OBSERVATIONEDITOR_H
#define OBSERVATIONEDITOR_H

#include <QWidget>
#include "observationtablemodel.h"

namespace GNU_gama { namespace local {
    class LocalNetwork;
}}

class ObservationTableModel;
class QMenu;
class QTableView;

class ObservationEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit ObservationEditor(QWidget *parent = nullptr);
    ~ObservationEditor();

    void connectObservationData(GNU_gama::local::LocalNetwork* lnet);
    void enableEdit(bool edit);

private:
    QTableView* tableView;
    ObservationTableModel* model;
    QMenu* observationMenu;
    bool   readonly;
    int    observationLogicalIndex;

    class SelectCluster {
    public:
        SelectCluster(ObservationTableModel* model, QTableView* tableview, int logicalIndex);
       ~SelectCluster();
        bool isValid() const;

    private:
        QTableView* tableView;
        bool        isValid_;
    };

    class SelectGroup {
    public:
        SelectGroup(ObservationTableModel* model, QTableView* tableview, int logicalIndex);
       ~SelectGroup();
        bool isValid() const;

    private:
        QTableView* tableView;
        bool        isValid_;
    };

signals:
    void warning(QString);

private slots:
    void observationContextMenu(QPoint p);
    void insertObservation();
    void deleteObservation();
    void insertCluster();
    void deleteCluster();};

#endif // OBSERVATIONEDITOR_H
