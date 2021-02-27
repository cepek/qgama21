/*  Qt based GUI for GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2014, 2016  Ales Cepek <cepek@gnu.org>

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

#include "insertobservationdialog.h"
#include "constants.h"
#include <gnu_gama/local/observation.h>
#include <gnu_gama/gon2deg.h>

#include <QComboBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QTextEdit>
#include <QDialogButtonBox>

InsertObservationDialog::InsertObservationDialog(QString cluster_name, QWidget *parent) :
    QDialog(parent),
    obsInfo(obsinfo),
    acceptDegrees(false),
    isObsAngle(false),
    clusterName(cluster_name)
{
    formLayout = new QFormLayout;
    warnings = new QTextEdit;

    warnings->hide();
    warnings->setReadOnly(true);

    comboPosition = new QComboBox(this);
    comboPosition->addItem(tr("Last Observation in the Cluster"));
    comboPosition->addItem(tr("After the Selected Obesrvation"));
    comboPosition->addItem(tr("Before the Slecter Observation"));
    comboPosition->addItem(tr("First observation in the Cluster"));

    if (clusterName == GamaQ2::obsClusterName)
    {
        setWindowTitle(tr("Insert observation"));

        comboObservation = new QComboBox(this);
        comboObservation->addItem(GamaQ2::distObsName);
        comboObservation->addItem(GamaQ2::dirObsName);
        comboObservation->addItem(GamaQ2::angleObsName);
        comboObservation->addItem(GamaQ2::azimObsName);
        comboObservation->addItem(GamaQ2::slopeObsName);
        comboObservation->addItem(GamaQ2::zangleObsName);

        formLayout->addRow(tr("Observation"), comboObservation);
        formLayout->addRow(tr("Position"),    comboPosition);
        formLayout->addRow(tr("From" ), lineEditFrom  = new QLineEdit(this));
        formLayout->addRow(tr("To"   ), lineEditTo    = new QLineEdit(this));
        formLayout->addRow(tr("Value"), lineEditValue = new QLineEdit(this));
    }
    else if (clusterName == GamaQ2::xyzClusterName)
    {
        setWindowTitle(tr("Insert coordinates"));

        formLayout->addRow(tr("Position"), comboPosition);
        formLayout->addRow(tr("Point"), lineEditPoint = new QLineEdit(this));
        formLayout->addRow(tr("X"), lineEditX = new QLineEdit(this));
        formLayout->addRow(tr("Y"), lineEditY = new QLineEdit(this));
        formLayout->addRow(tr("Z"), lineEditZ = new QLineEdit(this));

    }
    else if (clusterName == GamaQ2::hdfClusterName)
    {
        setWindowTitle(tr("Insert height difference"));

        formLayout->addRow(tr("Position"), comboPosition);
        formLayout->addRow(tr("From" ),    lineEditFrom  = new QLineEdit(this));
        formLayout->addRow(tr("To"   ),    lineEditTo    = new QLineEdit(this));
        formLayout->addRow(tr("H diff"),   lineEditHdiff = new QLineEdit(this));
    }
    else if (clusterName == GamaQ2::vecClusterName)
    {
        setWindowTitle(tr("Insert vector"));

        formLayout->addRow(tr("Position"), comboPosition);
        formLayout->addRow(tr("From" ),    lineEditFrom  = new QLineEdit(this));
        formLayout->addRow(tr("To"   ),    lineEditTo    = new QLineEdit(this));
        formLayout->addRow(tr("X diff"),   lineEditXdiff = new QLineEdit(this));
        formLayout->addRow(tr("Y diff"),   lineEditYdiff = new QLineEdit(this));
        formLayout->addRow(tr("Z diff"),   lineEditZdiff = new QLineEdit(this));
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Cancel);

    QVBoxLayout* vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(formLayout);
    vBoxLayout->addWidget(buttonBox);
    vBoxLayout->addWidget(warnings);
    setLayout(vBoxLayout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this](){accept();});
    connect(buttonBox, &QDialogButtonBox::rejected, this, [this](){close ();});
}

InsertObservationDialog::~InsertObservationDialog()
{
}

int InsertObservationDialog::position() const
{
    return comboPosition->currentIndex();
}

const QStringList& InsertObservationDialog::observationNames() const
{
    return obsNames;
}

void InsertObservationDialog::accept()
{
    using namespace GNU_gama::local;

    OK = true;
    tags.clear();
    html.clear();
    html += "<dl>";

    acceptDegrees = false;
    isObsAngle = false;

    try {
        if (clusterName == GamaQ2::obsClusterName) {

            QString name = comboObservation->currentText();

            if (name==GamaQ2::dirObsName || name ==  GamaQ2::angleObsName ||
               name == GamaQ2::zangleObsName || name == GamaQ2::azimObsName)
            {
                acceptDegrees = true;    // see getDouble()
            }

            if (name ==  GamaQ2::angleObsName)
            {
                isObsAngle = true;       // see getPointID
            }

            double value;
            getPointID(tr("From"),  lineEditFrom);
            getPointID(tr("To"),    lineEditTo, true);
            getDouble (tr("Value"), lineEditValue, value);

            if (OK) try {
                using namespace GNU_gama::local;

                std::string sp(standpoint.toUtf8().data());
                std::string t1(target1.toUtf8().data());
                std::string t2(target2.toUtf8().data());

                ObsInfo info;
                info.rowType = ObservationTableModel::obsRow;

                if (name == GamaQ2::distObsName) {
                    info.observation = new GNU_gama::local::Distance(sp, t1, value);
                    info.observationNameIndex = ObservationTableModel::indDist;
                } else if (name == GamaQ2::dirObsName) {
                    info.observation = new GNU_gama::local::Direction(sp, t1, value*G2R);
                    info.angular = true;
                    info.observationNameIndex = ObservationTableModel::indDir;
                } else if (name == GamaQ2::angleObsName) {
                    info.observation = info.angle = new GNU_gama::local::Angle(sp, t1, t2, value*G2R);
                    info.angular = true;
                    info.observationNameIndex = ObservationTableModel::indAngle;
                } else if (name == GamaQ2::azimObsName) {
                    info.observation = new GNU_gama::local::Azimuth(sp, t1, value);
                    info.angular = true;
                    info.observationNameIndex = ObservationTableModel::indAzimuth;
                } else if (name == GamaQ2::slopeObsName) {
                    info.observation = new S_Distance(sp, t1, value);
                    info.observationNameIndex = ObservationTableModel::indSdist;
                } else if (name == GamaQ2::zangleObsName) {
                    info.observation = new GNU_gama::local::Z_Angle(sp, t1, value*G2R);
                    info.angular = true;
                    info.observationNameIndex = ObservationTableModel::indZangle;
                } else
                    throw Exception(tr("unnown observation type in "
                                       "GamaQ2::obsClusterName").toUtf8().data());

                obsinfo.push_back(info);
            }
            catch(const Exception& e) {
                setWarning(name, e.what());
            }
            catch (...) {
                throw;
            }
        }
        else if (clusterName == GamaQ2::xyzClusterName) {

            bool bx = !lineEditX->text().simplified().isEmpty();
            bool by = !lineEditY->text().simplified().isEmpty();
            bool bz = !lineEditZ->text().simplified().isEmpty();

            if (!bx && !by && !bz) setWarning(GamaQ2::xyzClusterName, tr("No coordinates"));

            getPointID(tr("Point"), lineEditPoint);

            double x, y, z;
            if (bx || by)
            {
                getDouble (tr("X"), lineEditX, x);
                getDouble (tr("Y"), lineEditY, y);
            }
            if (bz)
            {
                getDouble (tr("Z"), lineEditZ, z);
            }

            if (OK) try {
                using namespace GNU_gama::local;

                QString qpoint = lineEditPoint->text().simplified();
                std::string point(qpoint.toUtf8().data());

                if (bx)
                {
                    ObsInfo infoX, infoY;
                    infoX.rowType = infoY.rowType = ObservationTableModel::obsRow;
                    infoX.group = infoY.group = GamaQ2::getUnique();

                    infoX.observation = new X(point, x);
                    infoX.observationNameIndex = ObservationTableModel::indX;

                    infoY.observation = new Y(point, y);
                    infoY.observationNameIndex = ObservationTableModel::indY;

                    obsinfo.push_back(infoX);
                    obsinfo.push_back(infoY);
                }

                if (bz)
                {
                    ObsInfo infoZ;
                    infoZ.rowType = ObservationTableModel::obsRow;

                    infoZ.observation = new Z(point, z);
                    infoZ.observationNameIndex = ObservationTableModel::indZ;

                    obsinfo.push_back(infoZ);
                }
            }
            catch(const Exception& e) {
                setWarning(comboObservation->currentText(), e.what());
            }
            catch (...) {
                throw;
            }
        }
        else if (clusterName == GamaQ2::hdfClusterName) {

            double value;
            getPointID(tr("From"),  lineEditFrom);
            getPointID(tr("To"),    lineEditTo, true);
            getDouble (tr("Value"), lineEditHdiff, value);

            if (OK) try {
                using namespace GNU_gama::local;

                QString standpoint = lineEditFrom->text().simplified();
                QString target     = lineEditTo->text().simplified();

                std::string sp(standpoint.toUtf8().data());
                std::string tp(target.toUtf8().data());

                ObsInfo info;
                info.rowType = ObservationTableModel::obsRow;

                info.observation = new GNU_gama::local::H_Diff(sp, tp, value);
                info.observationNameIndex = ObservationTableModel::indHdiff;

                obsinfo.push_back(info);
            }
            catch(const Exception& e) {
                setWarning(GamaQ2::hdfClusterName, e.what());
            }
            catch (...) {
                throw;
            }
        }
        else if (clusterName == GamaQ2::vecClusterName) {

            double vecXdiff, vecYdiff, vecZdiff;
            getPointID(tr("From"),   lineEditFrom);
            getPointID(tr("To"),     lineEditTo, true);
            getDouble (tr("X diff"), lineEditXdiff, vecXdiff);
            getDouble (tr("Y diff"), lineEditYdiff, vecYdiff);
            getDouble (tr("Z diff"), lineEditZdiff, vecZdiff);

            if (OK) try {
                using namespace GNU_gama::local;

                QString standpoint = lineEditFrom->text().simplified();
                QString target     = lineEditTo->text().simplified();

                std::string sp(standpoint.toUtf8().data());
                std::string tp(target.toUtf8().data());

                ObsInfo infoXdiff, infoYdiff, infoZdiff;
                infoXdiff.rowType = infoYdiff.rowType = infoZdiff.rowType = ObservationTableModel::obsRow;
                infoXdiff.group = infoYdiff.group = infoZdiff.group = GamaQ2::getUnique();

                infoXdiff.observation = new Xdiff(sp, tp, vecXdiff);
                infoYdiff.observation = new Ydiff(sp, tp, vecYdiff);
                infoZdiff.observation = new Zdiff(sp, tp, vecZdiff);

                infoXdiff.observationNameIndex = ObservationTableModel::indXdiff;
                infoYdiff.observationNameIndex = ObservationTableModel::indYdiff;
                infoZdiff.observationNameIndex = ObservationTableModel::indZdiff;

                obsinfo.push_back(infoXdiff);
                obsinfo.push_back(infoYdiff);
                obsinfo.push_back(infoZdiff);
            }
            catch(const Exception& e) {
                setWarning(GamaQ2::vecClusterName, e.what());
            }
            catch (...) {
                throw;
            }
        }
        else {
            setWarning(tr("FATAL"), tr("unknown cluster type"));
            setWarning(tr("FATAL"), tr("this should never happen"));
        }
    }
    catch (...) {
        setWarning(tr("FATAL"), tr("unknown exception"));
        setWarning(tr("FATAL"), tr("this should never happen"));
    }

    html += "</dl>";
    warnings->clear();
    warnings->insertHtml(html);
    warnings->moveCursor(QTextCursor::Start);

    if (OK) done(Accepted);
    else    warnings->show();
}

void InsertObservationDialog::getPointID(QString tag, QLineEdit *edit, bool isTarget)
{
    if (edit->text().isEmpty())
        setWarning(tag,   tr("Point ID is empty"));
    if (!isTarget && edit->text().contains(";"))
        setWarning(tag, tr("Point ID contains semicolon (;)"));
    if (isTarget && !edit->text().isEmpty() && !edit->text().contains(";") && isObsAngle)
        setWarning(tag, tr("Target field does not contain semicolon (;)"
                                ", for an angle semilocon separates backsight and foresight IDs"));

    if (isTarget && isObsAngle)
    {
        QStringList list = edit->text().split(QRegularExpression("\\s*;\\s*"));

        if (list.size() != 2 || list[0].isEmpty() || list[1].isEmpty())
        {
            setWarning(tag, tr("An angle must have two target IDs separated by one semicolon"));
            target1.clear();
            target2.clear();
        }
        else
        {
            target1 = list[0].simplified();
            target2 = list[1].simplified();
        }
    }
    else if (isTarget)
    {
        target1 = edit->text().simplified();
        target2.clear();
    }
    else
    {
        standpoint = edit->text().simplified();
    }

    QString tst = isTarget ? target1 + target2 : standpoint;
    if (tst.contains(QRegularExpression("\\s")))
        setWarning(tag, tr("ID must not contain whitespaces"));
}

void InsertObservationDialog::setWarning(QString tag, QString warning)
{
    OK = false;
    if (!tags.contains(tag))
    {
        html.append("<dt>" + tag + "</dt>\n");
    }
    tags.insert(tag);
    html.append("<dd>" + warning + "</dd>\n");
}

void InsertObservationDialog::getDouble(QString tag, QLineEdit* edit, double &val, bool positive)
{
    bool good = true;
    val = edit->text().toDouble(&good);
    if (!good)
    {
        if (acceptDegrees) good = GNU_gama::deg2gon(edit->text().toStdString(), val);
        if (!good) setWarning(tag, tr("Not a numeric value"));
    }
    if (good & positive & (val <= 0)) setWarning(tag, "Value must be positive");
}
