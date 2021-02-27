/*  Qt based GUI for GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2014  Ales Cepek <cepek@gnu.org>

    This file is a part of GNU Gama.

    GNU Gama is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GNU Gama is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INSERTOBSERVATIONDIALOG_H
#define INSERTOBSERVATIONDIALOG_H

#include "observationtablemodel.h"
#include <QDialog>
#include <QSet>

class QLineEdit;
class QComboBox;
class QFormLayout;
class QTextEdit;

namespace GNU_gama { namespace local {
    class Observation;
}}

class InsertObservationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit InsertObservationDialog(QString cluster_name, QWidget *parent = nullptr);
    ~InsertObservationDialog();

    int position() const;
    const QStringList& observationNames() const;

    // InsertObservationDialog is a friend class of ObservationTableMode which defines
    // private class ObsInfo
    typedef ObservationTableModel::ObsInfo ObsInfo;
    const   QList<ObsInfo>& obsInfo;

public slots:
    void accept();

private:
    bool acceptDegrees;
    bool isObsAngle;
    QString clusterName;
    QStringList obsNames;

    QComboBox* comboObservation;
    QComboBox* comboPosition;

    QLineEdit* lineEditFrom;
    QLineEdit* lineEditTo;
    QLineEdit* lineEditValue;
    QString    standpoint, target1, target2;

    QLineEdit* lineEditXdiff;
    QLineEdit* lineEditYdiff;
    QLineEdit* lineEditZdiff;

    QLineEdit* lineEditHdiff;

    QLineEdit* lineEditPoint;
    QLineEdit* lineEditX;
    QLineEdit* lineEditY;
    QLineEdit* lineEditZ;

    QFormLayout* formLayout {};
    QTextEdit* warnings {};

    QList<ObsInfo> obsinfo;

    bool OK;
    QSet<QString> tags;
    QString html;

    void setWarning(QString tag, QString warning);
    void getPointID(QString tag, QLineEdit* edit, bool isTarget=false);
    void getDouble (QString tag, QLineEdit* edit, double& val, bool positive=false);
};

#endif // INSERTOBSERVATIONDIALOG_H
