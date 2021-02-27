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

#include "pointtablemodel.h"
#include <QDebug>

#include <gnu_gama/local/network.h>
#include <gnu_gama/local/gamadata.h>

using GNU_gama::local::PointID;
using GNU_gama::local::LocalPoint;
using GNU_gama::local::PointData;

PointTableModel::PointTableModel(GNU_gama::local::PointData& pd,
                                 QObject *parent) :
    QAbstractTableModel(parent),
    pointData(pd)
{
    coordinate_types << "" << "fixed" << "constr" << "free";
    scan_data();
    dprec = 5;

    pointNames << "Id" << "X" << "Y" << "Txy" << "Z" << "Tz";
}


// QAbstractTableModel functions

int PointTableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return int(pointData.size()+1);
}

int PointTableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 6;
}

QVariant PointTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();      // return invalig QVariant

    int row = index.row();
    if (row == pointData.size()) return QVariant();
    int col = index.column();
    LocalPoint lp = pointData[pids[row]];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (col == indPointId)
        {
            return QVariant(QString().fromUtf8(pids[row].c_str()));
        }
        else if (col == indPointX)
        {
            if (lp.test_xy() && (ptxy[row] == 3 || ptxy[row] == 1))
                return QString("%1").arg(lp.x(), 0, 'F', dprec);
        }
        else if (col == indPointY)
        {
            if (lp.test_xy() && (ptxy[row] == 3 || ptxy[row] == 2))
                return QString("%1").arg(y_sign()*lp.y(), 0, 'F', dprec);
        }
        else if (col == indPointTxy)
        {
            if      (lp.fixed_xy())       return coordinate_types[1];
            else if (lp.constrained_xy()) return coordinate_types[2];
            else if (lp.free_xy())        return coordinate_types[3];
        }
        else if (col == indPointZ)
        {
            if (lp.test_z()) return QString("%1").arg(lp.z(), 0, 'F', dprec);
        }
        else if (col == indPointTz)
        {
            if      (lp.fixed_z())        return coordinate_types[1];
            else if (lp.constrained_z())  return coordinate_types[2];
            else if (lp.free_z())         return coordinate_types[3];
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        switch (col) {
        case indPointX:
        case indPointY:
        case indPointZ:
            return (Qt::AlignRight);
        case indPointTxy:
        case indPointTz:
            return (Qt::AlignCenter);
        }
    }

    return QVariant();
}

QVariant PointTableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section < indPointColumnCount) return pointNames[section];
        }
        else if (orientation == Qt::Vertical)
        {
            if (section == pointData.size()) return QVariant("*");
            if (pids[section].empty())  return QVariant();

            return QVariant(section+1);
        }
    }

    return QVariant();
}

bool PointTableModel::setData (const QModelIndex &index,
                                   const QVariant &value,
                                   int role)
{
    if (!index.isValid() || role != Qt::EditRole) return false;

    QString warning_text = tr("Bad point data");
    QString warning_txyz = tr("Possible values are 'fixed'/'constr'/'free'/''");
    QString warning_id   = tr("Point Id cannot be changed");

    QString simple = value.toString().simplified();
    int row = index.row();
    int col = index.column();

    if (row == pointData.size())
      {
        if(col == indPointId)
        {
            std::string newPointId = simple.toStdString();
            bool idFound  = (pointData.find(newPointId) != pointData.end());
            bool idIsGood = !idFound && !newPointId.empty();

            if (idFound)
            {
                pt_warning(row,col,tr("Id already exists"), simple);
            }
            else if (idIsGood)
            {
                insertPoint(newPointId);
                pids.push_back(newPointId);
                ptxy.push_back(0);
                insertRow(row+1);
                emit dataChanged(index, index);
            }
            return idIsGood;
        }
        else
        {
            return false;
        }
      }

    LocalPoint& lp = pointData[pids[row]];

    bool ok = true;
    if (col == indPointId)
    {
        if (simple.isEmpty())
        {
            removeRows(row,1,QModelIndex());
        }
        else
        {
            ok = false;
            warning_text = warning_id;
        }
    }
    else if (col == indPointX)
    {
        if (simple.isEmpty())
        {
            lp.unset_xy();
            ptxy[row] = 0;
        }
        else if (ptxy[row] == 0 || ptxy[row] == 1)
        {
            double x = value.toDouble(&ok);
            if (ok)
            {
                lp.set_xy(x, 0);
                ptxy[row] = 1;
            }
        }
        else if (ptxy[row] == 2 || ptxy[row] == 3)
        {
            double y = lp.y();
            double x = value.toDouble(&ok);
            if (ok)
            {
                lp.set_xy(x, y);
                ptxy[row] = 3;
            }
        }
    }
    else if (col == indPointY)
    {
        if (simple.isEmpty())
        {
            lp.unset_xy();
            ptxy[row] = 0;
        }
        else if (ptxy[row] == 0 || ptxy[row] == 2)
        {
            double y = y_sign()*value.toDouble(&ok);
            if (ok)
            {
                lp.set_xy(0, y);
                ptxy[row] = 2;
            }
        }
        else if (ptxy[row] == 1 || ptxy[row] == 3)
        {
            double x = lp.x();
            double y = y_sign()*value.toDouble(&ok);
            if (ok)
            {
                lp.set_xy(x, y);
                ptxy[row] = 3;
            }
        }
    }
    else if (col == indPointTxy)    // XY fixed / constrained / free / unused
    {
        if      (value == coordinate_types[1]) lp.set_fixed_xy();
        else if (value == coordinate_types[2]) lp.set_constrained_xy();
        else if (value == coordinate_types[3]) lp.set_free_xy();
        else if (simple.isEmpty())             lp.set_unused_xy();
        else
        {
            warning_text = warning_txyz;
            ok = false;
        }
    }
    else if (col == indPointZ)
    {
        if (simple.isEmpty())
            lp.unset_z();
        else
        {
            double z = value.toDouble(&ok);
            if (ok) lp.set_z(z);
        }
    }
    else if (col == indPointTz)   // Z fixed / constrained / free / unused
    {
        if      (value == coordinate_types[1]) lp.set_fixed_z();
        else if (value == coordinate_types[2]) lp.set_constrained_z();
        else if (value == coordinate_types[3]) lp.set_free_z();
        else if (simple.isEmpty())             lp.set_unused_z();
        else
        {
            warning_text = warning_txyz;
            ok = false;
        }
    }

    pt_warning(row, col, warning_text, simple, ok);

    if (ok) emit dataChanged(index, index);
    return ok;
}

Qt::ItemFlags PointTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        {
            return Qt::NoItemFlags;
        }

    if (index.column() == 0 && index.row() < rowCount(index)-1)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
}

bool PointTableModel::insertRows(int row, int count,
                                     const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);

    // no need to call pointData (std::map) allocator here
    // if (count > 0) pointData.get_allocator().allocate(count);

    endInsertRows();
    return true;
}

bool PointTableModel::removeRows(int row, int count,
                                     const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    for (int i=0; i<count; i++)
    {
        pointData.erase(pids[row+i]);
    }
    std::vector<std::string>::iterator   a=pids.begin()+row;
    std::vector<int>::iterator           b=ptxy.begin()+row;
    pids.erase(a, a+count);
    ptxy.erase(b, b+count);

    endRemoveRows();
    return true;
}


// GamaLocalPointModel functions

void PointTableModel::scan_data()
{
    pids.clear();
    ptxy.clear(); // 0 - no xy, 1 - x only, 2 - y only, 3 - both xy
    for (PointData::iterator i=pointData.begin(), e=pointData.end(); i!=e; ++i)
    {
        PointID id = i->first;
        pids.push_back(id.str());
        if (i->second.test_xy())
            ptxy.push_back(3);
        else
            ptxy.push_back(0);
    }
}

void PointTableModel::insertPoint(GNU_gama::local::PointID id,
                                      GNU_gama::local::LocalPoint point)
{
    pointData[id] = point;
}

void PointTableModel::pt_warning(int row, int col, QString text, QString value, bool ok)
{
    if (ok) return;

    QString txt = QString("%1 (row %2 col %3 val %4)")
        .arg(text, headerData(row, Qt::Vertical).toString(),
        headerData(col, Qt::Horizontal).toString(), value);
    emit warning(txt);
}
