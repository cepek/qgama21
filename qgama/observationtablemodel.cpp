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

#include "observationtablemodel.h"
#include "constants.h"
#include "shrinkbandwidth.h"
#include "insertobservationdialog.h"
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/gon2deg.h>

#include <QBrush>
#include <QFont>
#include <QMessageBox>
#include <typeinfo>

typedef GNU_gama::local::LocalNetwork    LocalNetwork;
using namespace QGama;

/* implementation of private structure ObsInfo - information on an Observation */
// -------------------------------------------------------------------------------
QStringList ObservationTableModel::ObsInfo::obsNames;

ObservationTableModel::ObsInfo::ObsInfo()
    : cluster(nullptr), clusterIndex(0), observation(nullptr), observationNameIndex(-1), angle(nullptr), angular(false),
      positiveDefinite(true), group(0)
{
    if (obsNames.size() == 0)
    {
        using namespace QGama;
        obsNames << distObsName  << dirObsName  << angleObsName << azimObsName << hdifObsName    // see constants.cpp
                 << xdifObsName  << ydifObsName << zdifObsName  << xObsName << yObsName << zObsName
                 << slopeObsName << zangleObsName;
    }
}

QString  ObservationTableModel::ObsInfo::from()
{
    return QString::fromUtf8(observation->from().str().c_str());
}
QString  ObservationTableModel::ObsInfo::to()
{
    QString to = QString::fromUtf8(observation->to().str().c_str());

    if (angle)
    {
        to += "; ";
        to += QString::fromUtf8(angle->fs().str().c_str());
    }
    return to;
}
QString  ObservationTableModel::ObsInfo::name()
{
    return obsNames.at(observationNameIndex);
}
QVariant ObservationTableModel::ObsInfo::value(LocalNetwork* lnet)
{
    double val = observation->value();
    if (!angular)        return QString::number(val,'f',5);
    val *= R2G;
    if (lnet->degrees()) return GNU_gama::gon2deg(val, 0, 2).c_str();
    return  QString::number(val,'f',6);

}
QVariant ObservationTableModel::ObsInfo::stdDev(LocalNetwork* lnet)
{
    double stdev = observation->stdDev();
    if(angular && lnet->degrees()) stdev *= 0.324;
    return QString::number(stdev, 'g', 3);
}
QString ObservationTableModel::ObsInfo::active()
{
    return observation->active() ? "1" : "0";
}
QVariant ObservationTableModel::ObsInfo::fromDh()
{
    double d = observation->from_dh();
    if (bool(d)) return d;
    return QVariant();
}
QVariant ObservationTableModel::ObsInfo::toDh()
{
    double d1 = observation->to_dh();
    if (const Angle* angle = dynamic_cast<const Angle*>(observation))
    {
        double d2 = angle->fs_dh();
        if (d1 > 0 || d2 > 0) return QString("%1; %2").arg(d1).arg(d2);
    }
    else
    {
        if (d1 > 0) return QString("%1").arg(d1);
    }

    return QVariant();
}
QVariant ObservationTableModel::ObsInfo::Hdist()
{
    if (const H_Diff* hd = dynamic_cast<const H_Diff*>(observation))
    {
        if (double dist = hd->dist())
        {
           return QVariant(dist);
        }
    }

    return QVariant();
}


// -------------------------------------------------------------------------------

ObservationTableModel::ObservationTableModel(GNU_gama::local::LocalNetwork *localNetwork,
                                             QObject *parent) :
    QAbstractTableModel(parent),
    lnet(localNetwork),
    obsData(lnet->OD),
    columnCountMax(0)
{
    initObsMap();
}

void ObservationTableModel::initObsMap()
{
    obsMap.clear();

    ClusterList::iterator ic = obsData.clusters.begin();
    ClusterList::iterator ec = obsData.clusters.end();
    for ( ; ic!=ec; ++ic)
    {
        ObsInfo info;
        info.rowType = clusterHeader;
        info.cluster = *ic;

        if (dynamic_cast<const GNU_gama::local::StandPoint*>(*ic))
        {
             info.clusterName = obsClusterName;
        }
        else if (dynamic_cast<const GNU_gama::local::Coordinates*>(*ic))
        {
             info.clusterName = xyzClusterName;
        }
        else if (dynamic_cast<const GNU_gama::local::HeightDifferences*>(*ic))
        {
             info.clusterName = hdfClusterName;
        }
        else  if (dynamic_cast<const GNU_gama::local::Vectors*>(*ic))
        {
             info.clusterName = vecClusterName;
        }

        obsMap.push_back(info);
        info.clusterName.clear();

        ObservationList::iterator i = (*ic)->observation_list.begin();
        ObservationList::iterator e = (*ic)->observation_list.end();
        int clusterIndex = 1;
        for ( ; i!=e; ++i)
        {
            ObsInfo obs      = info;
            info.clusterName = "";
            obs.observation  = *i;
            obs.clusterIndex = clusterIndex++;
            obs.rowType      = obsRow;

            if (dynamic_cast<const GNU_gama::local::Distance*>(*i))
            {
                obs.observationNameIndex = indDist;
            }
            else if (dynamic_cast<const GNU_gama::local::Direction*>(*i))
            {
                obs.observationNameIndex = indDir;
                obs.angular = true;
            }
            else if (GNU_gama::local::Angle *a
                     = dynamic_cast<GNU_gama::local::Angle*>(*i))
            {
                obs.observationNameIndex = indAngle;
                obs.angle = a;
                obs.angular = true;
            }
            else if (dynamic_cast<GNU_gama::local::Azimuth*>(*i))
            {
                obs.observationNameIndex = indAzimuth;
                obs.angular = true;
            }
            else if (dynamic_cast<const GNU_gama::local::H_Diff*>(*i))
            {
                obs.observationNameIndex = indHdiff;
            }
            else if (dynamic_cast<const GNU_gama::local::S_Distance*>(*i))
            {
                obs.observationNameIndex = indSdist;
            }
            else if (dynamic_cast<const GNU_gama::local::Z_Angle*>(*i))
            {
                obs.observationNameIndex = indZangle;
                obs.angular = true;
            }
            else if (dynamic_cast<const GNU_gama::local::Xdiff*>(*i))
            {
                obs.observationNameIndex = indXdiff;
            }
            else if (dynamic_cast<const GNU_gama::local::Ydiff*>(*i))
            {
                obs.observationNameIndex = indYdiff;
            }
            else if (dynamic_cast<const GNU_gama::local::Zdiff*>(*i))
            {
                obs.observationNameIndex = indZdiff;
            }
            else if (dynamic_cast<const GNU_gama::local::X*>(*i))
            {
                obs.observationNameIndex = indX;
            }
            else if (dynamic_cast<const GNU_gama::local::Y*>(*i))
            {
                obs.observationNameIndex = indY;
            }
            else if (dynamic_cast<const GNU_gama::local::Z*>(*i))
            {
                obs.observationNameIndex = indZ;
            }

            obsMap.push_back(obs);
        }

        ObsInfo tail;
        tail.rowType = clusterTail;
        obsMap.push_back(tail);
    }

    ObsInfo tableEnd;
    tableEnd.rowType = tableTail;
    obsMap.push_back(tableEnd);

    setColumnCountMax();
    for (int i=0; i<obsMap.size(); i++)
        if (obsMap[i].rowType == clusterHeader)
            testCovarianceMatrix(i);

    for (int i=1; i<obsMap.size()-2; i++)
    {
        ObsInfo& infoX = obsMap[i];
        bool vec = infoX.observationNameIndex == indXdiff;
        bool xyz = infoX.observationNameIndex == indX;
        if (!vec && !xyz) continue;

        ObsInfo& infoY = obsMap[i+1];
        if (xyz) xyz = infoY.observationNameIndex == indY;
        if (xyz) xyz = infoX.observation != 0 && infoY.observation != 0;
        if (xyz) xyz = infoX.observation->from() == infoY.observation->from();
        if (xyz) infoX.group = infoY.group = QGama::getUnique();

        ObsInfo& infoZ = obsMap[i+2];
        if (vec) vec = infoY.observationNameIndex == indYdiff && infoZ.observationNameIndex == indZdiff;
        if (vec) vec = infoX.observation != 0 && infoY.observation != 0 && infoZ.observation != 0;
        if (vec) vec = infoX.observation->from() == infoY.observation->from();
        if (vec) vec = infoY.observation->from() == infoZ.observation->from();
        if (vec) vec = infoX.observation->to() == infoY.observation->to();
        if (vec) vec = infoY.observation->to() == infoZ.observation->to();
        if (vec) infoX.group = infoY.group = infoZ.group = QGama::getUnique();
    }
}

// QAbstractTableModel functions

int ObservationTableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return obsMap.size();
}

int ObservationTableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return columnCountMax;
}

void ObservationTableModel::setColumnCountMax()
{
    int oldCount = columnCountMax;
    const ClusterList& cl = lnet->OD.clusters;
    int max = 0;
    for (ClusterList::const_iterator i=cl.begin(), e=cl.end(); i!=e; ++i)
    {
        int n = (*i)->covariance_matrix.rows();
        if (n > max) max = n;
    }
    columnCountMax =  max + indColumnCount;

    if (oldCount == 0)
    {
    }
    else if (columnCountMax > oldCount)
    {
        insertColumns(oldCount, columnCountMax-oldCount);
    }
    else if (columnCountMax < oldCount)
    {
        removeColumns(columnCountMax, oldCount-columnCountMax);
    }
    else
    {
    }
}

QVariant ObservationTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int col = index.column();
    ObsInfo info = obsMap[row];

    if (role == Qt::BackgroundRole)
    {        
        if (info.rowType == obsRow && col >= indColumnCount)
        {
            int r = info.clusterIndex;
            int b = col - indColumnCount;
            if (r+b > info.cluster->covariance_matrix.cols())      return QGama::colorOutsideCovMat;
            if ( b  > info.cluster->covariance_matrix.bandWidth()) return QGama::colorPassiveBackground;
            // !info.positiveDefinite ... would not work here, why?
            if (!obsMap[row].positiveDefinite)                     return QGama::colorSingularCovMat;
            return QVariant();
        }

        if (info.rowType == clusterHeader) return QGama::colorClusterHeader;
        if (info.observation)
        {
            if ( info.observation->stdDev() <= 0 && col == indStdev) return QGama::colorSingularCovMat;
            if (!info.observation->active()) return QGama::colorPassiveBackground;
        }

        return QVariant();
    }

    /*if (role == Qt::FontRole)
    {
        if (info.rowType == clusterHeader)
        {
            QFont  f;
            f.setBold(true);
            return f;
        }
    }*/

    if (role == Qt::TextAlignmentRole)
    {
        if (info.rowType == clusterHeader) return Qt::AlignCenter;

        switch (col)
        {
        case indVal:
        case indStdev:
            return (Qt::AlignRight);
        case indActive:
        case indName:
            return (Qt::AlignHCenter);
        }

        if (col >= indColumnCount) return (Qt::AlignRight);

        return QVariant();
    }

    if (!(role == Qt::DisplayRole || role == Qt::EditRole)) return QVariant();

    if (info.rowType == clusterHeader && col >= indColumnCount)
    {
        if (col == indColumnCount) return tr("Cov");

        int N = info.cluster->size();
        int K = col - indColumnCount;
        if (K < N) return QString("%1").arg(K);

        return QVariant();
    }

    if (info.rowType == obsRow)
    {
        switch(col)
        {
        case indFrom  : return info.from();
        case indTo    : return info.to();
        case indName  : return info.name();
        case indVal   : return info.value(lnet);
        case indStdev : return info.stdDev(lnet);
        case indActive: return info.active();
        case indFromDh: return info.fromDh();
        case indToDh  : return info.toDh();
        case indHdist : return info.Hdist();
        }

        if (col >= indColumnCount)
        {
            int i = info.clusterIndex;
            int j = i+col-indColumnCount;

            if (j <= info.cluster->covariance_matrix.cols())
            {
               double c = ((const Cluster*)(info.cluster))->covariance_matrix(i, j);
               if (c == 0) return QVariant();

               if (lnet->degrees())
               {
                  ObsInfo inf2 = obsMap[row + col - indColumnCount];
                  if (info.angular) c *= 0.324;
                  if (inf2.angular) c *= 0.324;
               }

               return QVariant(c).toString();
               // return c; !!! results in doblespinbox editor for the item !!!
            }
            return QVariant();
        }
    }
    else if (info.rowType == clusterHeader && info.clusterName == "obs")
    {
        switch(col)
        {
        case indFrom  : return tr("From");
        case indTo    : return tr("To");
        case indName  : return tr("Obs");
        case indVal   : return lnet->gons() ? tr("Value [m|g]")   : tr("Value [m|d]");
        case indStdev : return lnet->gons() ? tr("Stdev [mm|cc]") : tr("Stdev [mm|ss]");
        case indActive: return tr("Active");
        case indFromDh: return tr("From dh");
        case indToDh  : return tr("To dh");
        case indHdist : return tr("Hdiff dist [km]");
        }
    }
    else if (info.rowType == clusterHeader && info.clusterName == "hdf")
    {
        switch(col)
        {
        case indFrom  : return tr("From");
        case indTo    : return tr("To");
        case indName  : return tr("Obs");
        case indVal   : return tr("Value [m]");
        case indStdev : return tr("Stdev [mm]");
        case indActive: return tr("Active");
        case indHdist : return tr("Hdiff dist [km]");
        }
    }
    else if (info.rowType == clusterHeader && info.clusterName == "vec")
    {
        switch(col)
        {
        case indFrom  : return tr("From");
        case indTo    : return tr("To");
        case indName  : return tr("Obs");
        case indVal   : return tr("Value [m]");
        case indStdev : return tr("Stdev [mm]");
        case indActive: return tr("Active");
        case indFromDh: return tr("From dh");
        case indToDh  : return tr("To dh");
        }
    }
    else if (info.rowType == clusterHeader && info.clusterName == "xyz")
    {
        switch(col)
        {
        case indFrom  : return tr("Id");
        case indName  : return tr("Obs");
        case indVal   : return tr("Value [m]");
        case indStdev : return tr("Stdev [mm]");
        case indActive: return tr("Active");
        }
    }

    return QVariant();
}

// implementation of header section

QVariant ObservationTableModel::headerData(int section, Qt::Orientation orientation,
           int role) const
{
    if (role != Qt::DisplayRole) return QVariant();

    if (orientation == Qt::Vertical)
    {
        ObsInfo info = obsMap[section];
        switch (info.rowType)
        {
        case clusterHeader: return QVariant(info.clusterName);
        case obsRow       : return QVariant(QString("%1").arg(info.clusterIndex));
        case clusterTail  : return QVariant("");
        case tableTail    : return QVariant("   ");
        }
    }

   return QVariant();
}

// resizeable data

bool ObservationTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole) return false;

    int row = index.row();
    int col = index.column();
    ObsInfo info = obsMap[row];
    if (info.rowType != obsRow) return false;

    if (col == indVal)
    {
        bool ok;
        double val = value.toDouble(&ok);
        if (!ok)
        {
            if (info.angular) ok = GNU_gama::deg2gon(value.toString().toStdString(), val);
            if (!ok)
            {
                emit warning(tr("Non-numeric format of value %1").arg(value.toString()));
                return false;
            }
        }

        if (info.angular) val /= R2G;
        info.observation->set_value(val);

        return true;
    }

    if (col == indStdev)
    {
        bool ok;
        double stdv = value.toDouble(&ok);
        if (!ok)
        {
            emit warning(tr("Non-numeric format of StdDev %1").arg(value.toString()));
            return false;
        }

        bool angular = info.angular;
        if(angular && lnet->degrees()) stdv /= 0.324;

        int i = info.clusterIndex;
        Cluster* cluster = const_cast<Cluster*>(info.cluster);
        cluster->covariance_matrix(i,i) = stdv*stdv;

        testCovarianceMatrix(row);
        return true;
    }

    if (col == indActive)
    {
        QString val = value.toString().simplified();
        if (val.isEmpty()) val = "0";

        Observation* obs = const_cast<Observation*>(info.observation);

        if      (val == "1") obs->set_active();
        else if (val == "0") obs->set_passive();
        else {
            emit warning(tr("Active values can be 1 or 0 only"));
            return false;
        }

        return true;
    }

    if (col >= indColumnCount)
    {
        int b = col - indColumnCount;
        if (b > info.cluster->covariance_matrix.bandWidth() && !value.toString().simplified().isEmpty())
        {
            //emit warning("elements outside the bandwidth");
            Cluster *cluster = const_cast<Cluster*>(info.cluster);
            GNU_gama::CovMat<>& C = cluster->covariance_matrix;
            int N = C.rows();
            int B = C.bandWidth();
            GNU_gama::CovMat<> cm(N, b);
            cm.set_zero();
            for (int i=1; i<=N; i++)
                for (int j=i; j<=i+B; j++)
                {
                    if (j<=N) cm(i,j) = C(i,j);
                }
            C = cm;
        }

        bool ok;
        double val = value.toDouble(&ok);
        if (!ok)
        {
            emit warning(tr("Bad numeric format of data"));
            return false;
        }

        ObsInfo inf2 = obsMap[row + b];

        if (lnet->degrees())
        {
            if (info.angular) val /= 0.324;
            if (inf2.angular) val /= 0.324;
        }

        int i = info.clusterIndex;
        int j = inf2.clusterIndex;
        Cluster* cluster = const_cast<Cluster*>(info.cluster);
        cluster->covariance_matrix(i,j) = val;

        if (val == 0) shrinkBandWidth(cluster->covariance_matrix);

        // is variance-covariance matrix positive definite?
        testCovarianceMatrix(row);

        return true;
    }

    return false;
}

Qt::ItemFlags ObservationTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;

    const Qt::ItemFlags itemIsEditable = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    int row = index.row();
    int col = index.column();
    ObsInfo info = obsMap[row];

    if (info.rowType == obsRow)
    {
        switch (col)
        {
        case indVal:
        case indStdev:
        case indActive:
            return itemIsEditable;
        }

        if (col >= indColumnCount)
        {
            int r = info.clusterIndex;
            int b = col - indColumnCount;
            if (r+b > info.cluster->covariance_matrix.cols()) return Qt::NoItemFlags;

            return itemIsEditable;
        }
    }

    if (info.rowType == clusterTail || info.rowType == tableTail) return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ObservationTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count-1);

    endInsertRows();
    return true;
}

bool ObservationTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count-1);

    endRemoveRows();
    return true;
}

bool ObservationTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column+count-1);

    endInsertColumns();
    return true;
}

bool ObservationTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column+count-1);

    endRemoveColumns();
    return true;
}


void ObservationTableModel::deleteCluster(int logicalIndex)
{
    ObsInfo obsinfo = obsMap[logicalIndex];
    if (obsinfo.rowType == tableTail) return;

    int minIndex, maxIndex;
    if (!clusterIndexes(logicalIndex, minIndex, maxIndex)) return;

    const Cluster* cluster = obsMap[minIndex].cluster;
    ClusterList cl;
    for (ClusterList::iterator
         i=lnet->OD.clusters.begin(), e=lnet->OD.clusters.end(); i!=e; ++i)
    {
        if (cluster != *i) cl.push_back(*i);
    }

    lnet->OD.clusters.clear();
    for (ClusterList::iterator i=cl.begin(), e=cl.end(); i!=e; ++i)
    {
        lnet->OD.clusters.push_back(*i);
    }

    lnet->update_observations();

    obsMap.remove(minIndex, maxIndex-minIndex+1);
    removeRows(minIndex, maxIndex-minIndex+1);

    setColumnCountMax();
}


void ObservationTableModel::insertCluster(int logicalIndex, int position, QString clusterName)
{
    Cluster* cluster = nullptr;
    if      (clusterName == QGama::obsClusterName) cluster = new GNU_gama::local::StandPoint(&obsData);
    else if (clusterName == QGama::xyzClusterName) cluster = new GNU_gama::local::Coordinates(&obsData);
    else if (clusterName == QGama::hdfClusterName) cluster = new GNU_gama::local::HeightDifferences(&obsData);
    else if (clusterName == QGama::vecClusterName) cluster = new GNU_gama::local::Vectors(&obsData);

    if (cluster == nullptr) return;

    ObsInfo infoHeader;
    infoHeader.rowType     = clusterHeader;
    infoHeader.clusterName = clusterName;
    infoHeader.cluster     = cluster;

    ObsInfo infoTail;
    infoTail.rowType = clusterTail;

    enum {lastItem, afterCurrent, beforeCurrent, firstItem};

    if (position == lastItem || lnet->OD.clusters.empty())
    {
        lnet->OD.clusters.push_back(cluster);
        int N = obsMap.size() - 1;
        obsMap.insert(N,1,infoTail);
        obsMap.insert(N,1,infoHeader);
        insertRows(N, 2);
        lnet->update_observations();
        return;
    }

    int minIndex, maxIndex;
    if (!clusterIndexes(logicalIndex, minIndex, maxIndex)) return;

    const Cluster* selectedCluster = obsMap[minIndex].cluster;
    if (position == firstItem)
    {
        position = beforeCurrent;
        selectedCluster = obsMap[0].cluster;
    }

    int N = -1, index = 0;
    ClusterList cl;
    for (ClusterList::iterator
         i=lnet->OD.clusters.begin(), e=lnet->OD.clusters.end(); i!=e; ++i)
    {
        if (selectedCluster == *i && position == beforeCurrent)
        {
            N = index;
            cl.push_back(cluster);
        }

        cl.push_back(*i);
        index += (*i)->observation_list.size() + 2;

        if (selectedCluster == *i && position == afterCurrent)
        {
            N = index;
            cl.push_back(cluster);
        }
    }

    lnet->OD.clusters.clear();
    for (ClusterList::iterator i=cl.begin(), e=cl.end(); i!=e; ++i)
    {
        lnet->OD.clusters.push_back(*i);
    }

    if (N < 0) return;  /* this should never happen */

    obsMap.insert(N, 1, infoTail);
    obsMap.insert(N, 1, infoHeader);
    insertRows(N, 2, QModelIndex());
    lnet->update_observations();
}

bool ObservationTableModel::clusterIndexes(int logicalIndex, int &minIndex, int &maxIndex)
{
    if (logicalIndex < 0) return false;
    if (logicalIndex >= obsMap.size()) return false;
    if (obsMap[logicalIndex].rowType == tableTail) return false;

    // cluster header row
    minIndex = logicalIndex;
    while (obsMap[minIndex].rowType != clusterHeader) minIndex--;

    // cluster sentinel row
    maxIndex = logicalIndex;
    while (obsMap[maxIndex].rowType != clusterTail) maxIndex++;

    return true;
}

bool ObservationTableModel::groupIndexes(int logicalIndex, int &minIndex, int &maxIndex)
{
    if (logicalIndex < 0) return false;
    if (logicalIndex >= obsMap.size()) return false;
    if (obsMap[logicalIndex].rowType != obsRow) return false;

    int group = obsMap[logicalIndex].group;
    minIndex = maxIndex = logicalIndex;
    if (group == 0) return true;

    while (obsMap[minIndex-1].group == group) minIndex--;
    while (obsMap[maxIndex+1].group == group) maxIndex++;

    return true;
}

bool ObservationTableModel::isObservationRow(int logicalIndex) const
{
    if (logicalIndex < 0) return false;
    if (logicalIndex >= obsMap.size()) return false;

    return obsMap[logicalIndex].rowType == obsRow;
}

void ObservationTableModel::deleteObservation(int logicalIndex)
{
    if (obsMap[logicalIndex].group)
    {
        int minIndex, maxIndex;
        groupIndexes(logicalIndex, minIndex, maxIndex);
        for (int i=minIndex; i<=maxIndex; i++) obsMap[i].group = 0;
        for (int i=minIndex; i<=maxIndex; i++) deleteObservation(minIndex);
        return;
    }

    const Observation* observation = obsMap[logicalIndex].observation;
    Cluster* cluster = const_cast<Cluster*>(observation->ptr_cluster());

    std::list<Observation*> obs;
    int cluster_index = 0, ci = 1;
    for (ObservationList::iterator i=cluster->observation_list.begin(),
         e=cluster->observation_list.end(); i!=e; ++i, ++ci)
    {
        if (*i == observation) {
            cluster_index = ci;
            delete *i;
        }
        else obs.push_back(*i);
    }
    cluster->observation_list.clear();
    for (ObservationList::iterator i=obs.begin(), e=obs.end(); i!=e; ++i)
    {
        cluster->observation_list.push_back(*i);
    }

    // reduced band width
    const int cdim  = cluster->covariance_matrix.rows();
    const int cband = cluster->covariance_matrix.bandWidth();
    int rband = 0;
    for (int row=1; row<=cdim; row++)
        if (row != cluster_index) {
            int maxcol = row + cband;
            if (maxcol > cdim) maxcol = cdim;
            for (int col=maxcol; col>row; col--)
                if (cluster->covariance_matrix(row,col) && col !=cluster_index) {
                    int b = col-row;
                    if (row < cluster_index && col > cluster_index) b--;
                    if (b > rband) rband = b;
                    break;
                }
        }

    // reduced covariance matrix
    Observation::CovarianceMatrix cov(cdim-1,rband);
    for (int r=1; r<=cov.rows(); r++)
        for (int b=0; b<=cov.bandWidth(); b++)
            if (r+b <= cov.cols() ){
                const int rr = ( r <cluster_index) ?  r  : r+1;
                const int cc = (r+b<cluster_index) ? r+b : r+b+1;
                // zero value outside the original band
                cov(r, r+b) = (cc-rr <= cband) ? cluster->covariance_matrix(rr,cc) : 0;
            }

    cluster->covariance_matrix = cov;
    cluster->update();
    lnet->update_points();
    obsMap.remove(logicalIndex, 1);
    int k = logicalIndex;
    while (obsMap[k].rowType != clusterTail)
    {
        obsMap[k++].clusterIndex--;
    }
    removeRows(logicalIndex, 1, QModelIndex());
    testCovarianceMatrix(logicalIndex);
    setColumnCountMax();
}

QString ObservationTableModel::currentClusterName(int logicalIndex) const
{
    while (logicalIndex >= 0 && obsMap[logicalIndex].rowType != clusterHeader) logicalIndex--;
    if (logicalIndex < 0) return QString();

    return obsMap[logicalIndex].clusterName;
}

void ObservationTableModel::testCovarianceMatrix(int logicalIndex)
{
    int minIndex, maxIndex;
    if (!clusterIndexes(logicalIndex, minIndex, maxIndex)) return;

    bool pd = true;
    GNU_gama::CovMat<> C = obsMap[minIndex].cluster->covariance_matrix;
    try {
        if (C.rows() > 0 && C(1,1) <= 0) pd = false;  // fixing a bug in C.cholDec()
        C.cholDec();
    }
    catch (GNU_gama::Exception::matvec e) {
        pd = false;
    }
    for (int i=minIndex+1; i<maxIndex; i++)
    {
        obsMap[i].positiveDefinite = pd;
    }
}

void ObservationTableModel::insertObservation(int logicalIndex, const InsertObservationDialog& dialog)
{
    int minIndex, maxIndex;
    if (!clusterIndexes(logicalIndex, minIndex, maxIndex)) return;

    Cluster*    cluster = obsMap[minIndex].cluster;
    const int cdim  = cluster->covariance_matrix.rows();
    const int cband = cluster->covariance_matrix.bandWidth();

    enum {lastItem, afterCurrent, beforeCurrent, firstItem};

    int position = dialog.position();
    int selectedObservation = logicalIndex;
    if (position == firstItem && !cluster->observation_list.empty())
    {
        position = beforeCurrent;
        selectedObservation = minIndex + 1;
    }

    if (position == lastItem || cluster->observation_list.empty())
    {
        Observation::CovarianceMatrix cov(cdim + dialog.obsInfo.size(), cband);
        cov.set_zero();

        for (int r=1; r<=cluster->covariance_matrix.rows(); r++)
            for (int b=0; b<=cluster->covariance_matrix.bandWidth(); b++)
                if (r+b <= cluster->covariance_matrix.cols() ){
                    cov(r,r+b) = cluster->covariance_matrix(r,r+b);
                }
        cluster->covariance_matrix = cov;

        for (int n=0, index=maxIndex;  n<dialog.obsInfo.size(); n++)
        {
            ObsInfo obs = dialog.obsInfo[n];
            obs.clusterIndex = index-minIndex;
            obs.cluster = cluster;
            obsMap.insert(index++, obs);

            cluster->observation_list.push_back(obs.observation);
        }

        selectedObservation = maxIndex;
    }
    else
    {
        int minGroup, maxGroup;
        if (!groupIndexes(selectedObservation, minGroup, maxGroup)) return;
        if (position == beforeCurrent) selectedObservation = minGroup;
        else                           selectedObservation = maxGroup;

        const int obsdim = dialog.obsInfo.size();
        Observation::CovarianceMatrix cov(cdim + obsdim, cband + obsdim);
        cov.set_zero();

        ObservationList obs;
        for (ObservationList::iterator
            i=cluster->observation_list.begin(), e=cluster->observation_list.end(); i!=e; ++i)
        {
            obs.push_back(*i);
        }
        cluster->observation_list.clear();

        ObservationList::iterator copy=obs.begin();
        for (int r=1, i=minIndex+1; i<maxIndex; i++, r++)
        {
            for (int step=1; step<=2; step++)
            {
                if ((step == 1 && position == beforeCurrent && selectedObservation == i) ||
                    (step == 2 && position == afterCurrent  && selectedObservation == i))
                {
                    for (int n=obsdim-1, m=0; m<obsdim; m++, n--)
                    {
                        ObsInfo obs = dialog.obsInfo[n];
                        obs.cluster = cluster;
                        obsMap.insert(i + (position == beforeCurrent ? 0 : 1), obs);

                        cluster->observation_list.push_back(dialog.obsInfo[m].observation);
                    }
                }
                if (step == 1)
                {
                    for (int b=0; b<=cband; b++)
                        if (r+b <= cdim)
                        {
                            int row = r;
                            int col = r+b;
                            if (position == beforeCurrent) {
                                if (row >= selectedObservation-minIndex) row += obsdim;
                                if (col >= selectedObservation-minIndex) col += obsdim;
                            } else {
                                if (row > selectedObservation-minIndex) row += obsdim;
                                if (col > selectedObservation-minIndex) col += obsdim;
                            }
                            cov(row,col) = cluster->covariance_matrix(r,r+b);
                        }

                    cluster->observation_list.push_back(*copy++);
                }
            }
        }

        for (int n=minIndex+1, i=0; i<cdim+obsdim; i++, n++) obsMap[n].clusterIndex = n - minIndex;
        cluster->covariance_matrix = cov;
        if (position == afterCurrent) selectedObservation++;
    }

    cluster->update();
    lnet->update_points();
    insertRows(selectedObservation, dialog.obsInfo.size());
    setColumnCountMax();
    testCovarianceMatrix(selectedObservation);
}

