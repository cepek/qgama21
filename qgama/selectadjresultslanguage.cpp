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

#include "selectadjresultslanguage.h"
#include "constants.h"
#include "qgamacontrolpanel.h"
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>

SelectAdjResultsLanguage::SelectAdjResultsLanguage(QWidget *parent) :
    QDialog(parent)
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                           | QDialogButtonBox::Cancel);
    comboBox = new QComboBox;
    label = new QLabel;
    comboBox->insertItems(0, QGama::languages);
    comboBox->setCurrentText(QGama::currentLanguage);
    label->setText(QGama::currentLanguage);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Current language ISO 639-1 code"), label);
    formLayout->addRow(tr("New language"), comboBox);

    QVBoxLayout* vboxLayout = new QVBoxLayout;
    vboxLayout->addLayout(formLayout);
    vboxLayout->addWidget(buttonBox);
    setLayout(vboxLayout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this](){on_buttonBox_accepted();});
    connect(buttonBox, &QDialogButtonBox::rejected, this, [this](){reject();});
}

SelectAdjResultsLanguage::~SelectAdjResultsLanguage()
{
}

void SelectAdjResultsLanguage::on_buttonBox_accepted()
{
    QString code = comboBox->currentText();
    if (code != QGama::currentLanguage)
    {
       QGamaControlPanel* panel = dynamic_cast<QGamaControlPanel*>(parent());
       if (panel == nullptr) return;

       panel->set_adjustment_results_language(code);
    }

    accept();
}
