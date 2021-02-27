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

#ifndef GAMALOCALOBSERVATIONTABLEMODEL_H
#define GAMALOCALOBSERVATIONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <gnu_gama/local/network.h>

typedef GNU_gama::local::Observation     Observation;
typedef GNU_gama::local::Direction       Direction;
typedef GNU_gama::local::Angle           Angle;
typedef GNU_gama::local::Azimuth         Azimuth;
typedef GNU_gama::local::Z_Angle         Z_Angle;
typedef GNU_gama::local::H_Diff          H_Diff;
typedef GNU_gama::Cluster<Observation>   Cluster;
typedef std::list<Cluster*>              ClusterList;
typedef GNU_gama::local::ObservationList ObservationList;
typedef GNU_gama::local::LocalNetwork    LocalNetwork;

class InsertObservationDialog;

class ObservationTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ObservationTableModel(GNU_gama::local::LocalNetwork* lnt,
                          QObject *parent=nullptr);

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

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    // Gama interface

    void deleteCluster (int logicalIndex);
    void insertCluster (int logicalIndex, int position, QString clusterName);
    bool clusterIndexes(int logicalIndex, int& minIndex, int& maxIndex);
    bool groupIndexes  (int logicalIndex, int& minIndex, int& maxIndex);

    bool isObservationRow (int logicalIndex) const;
    void deleteObservation(int logicalIndex);

    QString currentClusterName(int logicalIndex) const;
    void insertObservation(int logicalIndex, const InsertObservationDialog& dialog);

private:
    GNU_gama::local::LocalNetwork*    lnet;
    GNU_gama::local::ObservationData& obsData;

    enum RowType  { clusterHeader, clusterTail, obsRow, tableTail };
    enum ColIndx  { indFrom, indTo, indName, indVal, indStdev,
                    indActive, indFromDh, indToDh, indHdist,
                    indColumnCount };
    enum ObsNames { indDist,  indDir,   indAngle, indAzimuth, indHdiff,
                    indXdiff, indYdiff, indZdiff, indX, indY, indZ,
                    indSdist, indZangle };

    // InsertObservationDialog returns a list of ObsInfo objects
    friend class InsertObservationDialog;

    struct ObsInfo
    {
        ObsInfo();

        QString  from();
        QString  to();
        QString  name();
        QVariant value(LocalNetwork* lnet);
        QVariant stdDev(LocalNetwork* lnet);
        QString  active();
        QVariant fromDh();
        QVariant toDh();
        QVariant Hdist();

        static QStringList obsNames;

        RowType      rowType;
        Cluster*     cluster;
        QString      clusterName;
        int          clusterIndex;
        Observation* observation;
        int          observationNameIndex;
        Angle*       angle;
        bool         angular;
        bool         positiveDefinite;
        int          group;
    };

    QVector<ObsInfo> obsMap;
    int columnCountMax;

    void initObsMap();
    void setColumnCountMax();
    void testCovarianceMatrix(int logicalIndex);

signals:
    void warning(QString);
};

#endif // GAMALOCALOBSERVATIONTABLEMODEL_H
