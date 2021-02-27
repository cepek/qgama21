/* 
  GNU Gama Qt based GUI
  Copyright (C) 2013, 2020 Ales Cepek <cepek@gnu.org>

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

#include "parametereditor.h"
#include <QMessageBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDebug>

ParameterEditor::ParameterEditor(QWidget *parent) :
    QWidget(parent),
    lnet(nullptr)
{
    checkBox_update_cc   = new QCheckBox;
    comboBox_sigma_act   = new QComboBox;
    comboBox_axes_xy     = new QComboBox;
    comboBox_angles      = new QComboBox;
    comboBox_algorithm   = new QComboBox;
    comboBox_ang_units   = new QComboBox;
    comboBox_ellipsoid   = new QComboBox;
    lineEdit_sigma_apr   = new QLineEdit;
    lineEdit_conf_pr     = new QLineEdit;
    lineEdit_tol_abs     = new QLineEdit;
    lineEdit_epoch       = new QLineEdit;
    lineEdit_adj_covband = new QLineEdit;
    lineEdit_latitude    = new QLineEdit;

    plainTextEdit = new QPlainTextEdit;

    enableEdit(false);

    comboBox_sigma_act->addItems({"a posteriori", "a priori"});
    comboBox_axes_xy  ->addItems({"ne","sw","es","wn","en","nw","se","ws"});
    comboBox_angles   ->addItems({"clockwise","counter-clocwise"});
    comboBox_algorithm->addItems({"gso","svd","cholesky","envelope"});
    comboBox_ang_units->addItems({"400","360"});
    // comboBox_ellipsoid;

    connect(comboBox_sigma_act,  &QComboBox::currentTextChanged, this,
            [this](const QString& text){on_comboBox_sigma_act_currentTextChanged(text);});
    connect(comboBox_axes_xy,    &QComboBox::currentTextChanged, this,
            [this](const QString& text){on_comboBox_axes_xy_currenTextChanged(text);});
    connect(comboBox_angles,     &QComboBox::currentTextChanged, this,
            [this](const QString& text){on_comboBox_angles_currentTextChanged(text);});
    connect(comboBox_algorithm,  &QComboBox::currentTextChanged, this,
            [this](const QString& text){on_comboBox_algorithm_currentTextChanged(text);});
    connect(comboBox_ang_units,  &QComboBox::currentTextChanged, this,
            [this](const QString& text){on_comboBox_ang_units_currentTextChanged(text);});
    connect(comboBox_ellipsoid,  &QComboBox::currentTextChanged, this,
            [this](const QString& text){on_comboBox_ellipsoid_currentTextChanged(text);});

    connect(lineEdit_sigma_apr,  &QLineEdit::editingFinished, this,
            [this](){on_lineEdit_sigma_apr_editingFinished();});
    connect(lineEdit_conf_pr,    &QLineEdit::editingFinished, this,
            [this](){on_lineEdit_conf_pr_editingFinished();});
    connect(lineEdit_tol_abs,    &QLineEdit::editingFinished, this,
            [this](){on_lineEdit_tol_abs_editingFinished();});
    connect(lineEdit_epoch,      &QLineEdit::editingFinished, this,
            [this](){on_lineEdit_epoch_editingFinished();});
    connect(lineEdit_adj_covband,&QLineEdit::editingFinished, this,
            [this](){on_lineEdit_adj_covband_editingFinished();});
    connect(lineEdit_latitude,   &QLineEdit::editingFinished, this,
            [this](){on_lineEdit_latitude_editingFinished();});

    connect(plainTextEdit,       &QPlainTextEdit::textChanged, this,
            [this](){on_plainTextEdit_textChanged();});

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("a priori stdev (sigma)"),    lineEdit_sigma_apr);
    formLayout->addRow(tr("confidence probability"),    lineEdit_conf_pr);
    formLayout->addRow(tr("tolerance for abs. terms"),  lineEdit_tol_abs);
    formLayout->addRow(tr("actual type of stdev used"), comboBox_sigma_act);
    formLayout->addRow(tr("axes x and y are oriented"), comboBox_axes_xy);
    formLayout->addRow(tr("angles are observed"),       comboBox_angles);
    formLayout->addRow(tr("algorithm"),                 comboBox_algorithm);
    formLayout->addRow(tr("angular units"),             comboBox_ang_units);
    formLayout->addRow(tr("adjusted cxx band witdth"),  lineEdit_adj_covband);
    formLayout->addRow(tr("epoch"),                     lineEdit_epoch);
    // formLayout->addRow(tr("latitude"),               lineEdit_latitude);
    // formLayout->addRow(tr("ellipsoid"),              comboBox_ellipsoid);

    QHBoxLayout* hBoxLayout = new QHBoxLayout;
    hBoxLayout->addLayout(formLayout);
    hBoxLayout->addWidget(plainTextEdit,1);
    setLayout(hBoxLayout);
}

ParameterEditor::~ParameterEditor()
{
}

void ParameterEditor::enableEdit(bool edit)
{
    readonly = !edit;

    plainTextEdit->setReadOnly(readonly);

    lineEdit_sigma_apr  ->setEnabled(edit);
    lineEdit_conf_pr    ->setEnabled(edit);
    lineEdit_tol_abs    ->setEnabled(edit);
    comboBox_sigma_act  ->setEnabled(edit);
    checkBox_update_cc  ->setEnabled(edit);
    comboBox_axes_xy    ->setEnabled(edit);
    comboBox_angles     ->setEnabled(edit);
    lineEdit_epoch      ->setEnabled(edit);
    comboBox_algorithm  ->setEnabled(edit);
    comboBox_ang_units  ->setEnabled(edit);
    lineEdit_adj_covband->setEnabled(edit);
    lineEdit_latitude   ->setEnabled(edit && false);
    comboBox_ellipsoid  ->setEnabled(edit && false);
}

void ParameterEditor::connectParameters(GNU_gama::local::LocalNetwork *ln)
{
    lnet = ln;

    lnet->remove_inconsistency();

    plainTextEdit->setPlainText( QString::fromUtf8(lnet->description.c_str()) );
    lineEdit_sigma_apr->setText( QString::number(lnet->apriori_m_0()) );
    lineEdit_conf_pr  ->setText( QString::number( lnet->conf_pr()) );
    lineEdit_tol_abs  ->setText( QString::number(lnet->tol_abs()) );

    if(double epoch = lnet->epoch()) {
        lineEdit_epoch->setText( QString::number(epoch) );
    } else {
        lineEdit_epoch->clear();
    }

    if (lnet->m_0_aposteriori())
        comboBox_sigma_act->setCurrentIndex(0);
    else
        comboBox_sigma_act->setCurrentIndex(1);

    {
        using CS = GNU_gama::local::LocalCoordinateSystem::CS;

        QComboBox * cb = comboBox_axes_xy;
        bool editable = cb->isEditable();
        cb->setEditable(false);

        // lcoords.h  EN= 1, NW= 2, SE= 4, WS=  8, NE=16, SW=32, ES=64, WN=128
        CS lcs = ln->PD.local_coordinate_system;
        switch (lcs) {
        case CS::EN: cb->setCurrentText("en"); break;
        case CS::NW: cb->setCurrentText("nw"); break;
        case CS::SE: cb->setCurrentText("se"); break;
        case CS::WS: cb->setCurrentText("ws"); break;
        case CS::NE: cb->setCurrentText("ne"); break;
        case CS::SW: cb->setCurrentText("sw"); break;
        case CS::ES: cb->setCurrentText("es"); break;
        case CS::WN: cb->setCurrentText("wn"); break;
        default:
                     cb->setCurrentText("en"); break;
        }

        cb->setEditable(editable);
    }

    if (lnet->PD.left_handed_angles())
        comboBox_angles->setCurrentIndex(0);
    else
        comboBox_angles->setCurrentIndex(1);

    QString algorithm = lnet->algorithm().c_str();
    int index_alg = 0;
    for (int i=0; i<comboBox_algorithm->count(); i++)
        if (comboBox_algorithm->itemText(i) == algorithm)
        {
            index_alg = i;
            break;
        }
    comboBox_algorithm->setCurrentIndex(index_alg);

    if (lnet->gons())
        comboBox_ang_units->setCurrentIndex(0);
    else
        comboBox_ang_units->setCurrentIndex(1);

    lineEdit_adj_covband->setText(QString::number(lnet->adj_covband()));
}

void ParameterEditor::on_plainTextEdit_textChanged()
{
    lnet->description = plainTextEdit->toPlainText().toUtf8().data();
}

void ParameterEditor::on_lineEdit_sigma_apr_editingFinished()
{
    bool ok;
    double value = lineEdit_sigma_apr->text().toDouble(&ok);

    if (ok && value > 0)
    {
        lnet->apriori_m_0(value);
        lnet->update_adjustment();
        return;
    }

    QString adjpar = tr("A priori reference standard deviation");
    if (!ok) QMessageBox::critical(this, adjpar,tr("Conversion error"));
    else     QMessageBox::critical(this, adjpar,tr("Must greater than 0"));

    lineEdit_sigma_apr->setFocus();
}

void ParameterEditor::on_lineEdit_conf_pr_editingFinished()
{
    bool ok;
    double value = lineEdit_conf_pr->text().toDouble(&ok);

    if (ok && value > 0 && value < 1)
    {
        lnet->conf_pr(value);
        lnet->update_adjustment();
        return;
    }

    QString adjpar = tr("Confidence probability used in statistical tests");
    if (!ok) QMessageBox::critical(this, adjpar,tr("Conversion error"));
    else     QMessageBox::critical(this, adjpar,tr("Must from from interval 0 and 1"));

    lineEdit_conf_pr->setFocus();
}

void ParameterEditor::on_lineEdit_tol_abs_editingFinished()
{
    bool ok;
    double value = lineEdit_tol_abs->text().toDouble(&ok);

    if (ok && value > 0)
    {
        lnet->tol_abs(value);
        lnet->update_residuals();
        return;
    }

    QString adjpar = tr("Tolerance for the identification of gross absolute terms");
    if (!ok) QMessageBox::critical(this, adjpar,tr("Conversion error"));
    else     QMessageBox::critical(this, adjpar,tr("Must from greater than 0"));

    lineEdit_tol_abs->setFocus();
}

void ParameterEditor::on_lineEdit_epoch_editingFinished()
{
    if (lineEdit_epoch->text().simplified().isEmpty())
    {
        lnet->set_epoch(0);
        return;
    }

    bool ok;
    double value = lineEdit_epoch->text().toDouble(&ok);

    if (ok)
    {
        lnet->set_epoch(value);
        return;
    }

    QString adjpar = tr("Epoch of the measurement");
    if (!ok) QMessageBox::critical(this, adjpar,tr("Conversion error"));
    //else     QMessageBox::critical(this, adjpar,tr("????"));

    lineEdit_epoch->setFocus();
}

void ParameterEditor::on_lineEdit_latitude_editingFinished()
{
    QMessageBox::warning(this, "Latitude", "Latitude corrections not implemented");
}

void ParameterEditor::on_comboBox_sigma_act_currentTextChanged(const QString &arg1)
{
    if (arg1 == "a posteriori")
        lnet->set_m_0_aposteriori();
    else
        lnet->set_m_0_apriori();

    lnet->update_adjustment();
}

void ParameterEditor::on_comboBox_axes_xy_currenTextChanged(const QString &arg1)
{
    using namespace GNU_gama::local;

    LocalCoordinateSystem::CS& lcs = lnet->PD.local_coordinate_system;
    lnet->return_inconsistency();

    if      (arg1 == "ne") lcs = LocalCoordinateSystem::CS::NE;
    else if (arg1 == "sw") lcs = LocalCoordinateSystem::CS::SW;
    else if (arg1 == "es") lcs = LocalCoordinateSystem::CS::ES;
    else if (arg1 == "wn") lcs = LocalCoordinateSystem::CS::WN;
    else if (arg1 == "en") lcs = LocalCoordinateSystem::CS::EN;
    else if (arg1 == "nw") lcs = LocalCoordinateSystem::CS::NW;
    else if (arg1 == "se") lcs = LocalCoordinateSystem::CS::SE;
    else if (arg1 == "ws") lcs = LocalCoordinateSystem::CS::WS;
    else
      {
      }

    lnet->remove_inconsistency();
    lnet->update_points();
}

void ParameterEditor::on_comboBox_angles_currentTextChanged(const QString &arg1)
{
    lnet->return_inconsistency();

    if (arg1 == "clockwise")
        lnet->PD.setAngularObservations_Lefthanded();
    else
        lnet->PD.setAngularObservations_Righthanded();

    lnet->remove_inconsistency();
    lnet->update_points();
}

void ParameterEditor::on_comboBox_algorithm_currentTextChanged(const QString &arg1)
{
    lnet->set_algorithm(arg1.toStdString());
}

void ParameterEditor::on_comboBox_ang_units_currentTextChanged(const QString &arg1)
{
    if (arg1 == "360")
        lnet->set_degrees();
    else
        lnet->set_gons();

    emit angular_units_changed();
}

void ParameterEditor::on_comboBox_ellipsoid_currentTextChanged(const QString &/*arg1*/)
{
    QMessageBox::warning(this, "Ellipsoid", "Ellipsoid selection not implemented");
}

void ParameterEditor::on_lineEdit_adj_covband_editingFinished()
{
    QString s = lineEdit_adj_covband->text().simplified();
    int n = s.toInt();
    if (n == 0) s.clear();
    else if (n < 0) s = "-1";
    lineEdit_adj_covband->setText(s);

    lnet->set_adj_covband(n);
}
