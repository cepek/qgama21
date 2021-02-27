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

#ifndef GAMALOCALPOINTTABLEMODEL_H
#define GAMALOCALPOINTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <gnu_gama/local/gamadata.h>


class PointTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PointTableModel(GNU_gama::local::PointData& pd,
        QObject *parent = nullptr);

        // abstract methods from QAbstractTableModel
        int        rowCount   (const QModelIndex &parent) const;
        int        columnCount(const QModelIndex &parent) const;
        QVariant   data       (const QModelIndex &index, int role) const;

        // implementation of header section
        QVariant   headerData (int section, Qt::Orientation orientation,
                               int role = Qt::DisplayRole) const;

        // resizeable data
        bool       setData (const QModelIndex &index,
                            const QVariant &value, int role);
        Qt::ItemFlags flags(const QModelIndex &index) const;

        // columns are static in GamaLocalPointModel, only rows
        // operations need to be implemented
        bool insertRows(int row, int count, const QModelIndex &parent);
        bool removeRows(int row, int count, const QModelIndex &parent);

        int  precision() const   { return dprec; }
        void setPrecision(int p) { dprec = p;    }
        void scan_data();

signals:
        void warning(QString);
    
public slots:

private:
    GNU_gama::local::PointData& pointData;
    std::vector<std::string>    pids;
    std::vector<int>            ptxy;
    QStringList                 coordinate_types;
    int                         dprec;

    enum PointColIndx { indPointId,  indPointX,  indPointY,  indPointTxy,
                        indPointZ,   indPointTz, indPointColumnCount  };
    enum PointNames   { namePointId, namePointX, namePointY, namePointTxy,
                        namePointZ,  namePointTz };

    QStringList pointNames;

    void insertPoint(GNU_gama::local::PointID id,
                     GNU_gama::local::LocalPoint point = GNU_gama::local::LocalPoint());
    void pt_warning(int row, int col, QString text, QString value=QString(), bool ok=false);

    double y_sign() const {
        return (pointData.left_handed_coordinates() == pointData.left_handed_angles()) ? +1.0 : -1.0;
    }
};

#endif // GAMALOCALPOINTMODEL_H
