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

#include "insertclusterdialog.h"
#include "constants.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>

InsertClusterDialog::InsertClusterDialog(QWidget *parent) :
    QDialog(parent)
{
    comboBoxClusterName = new QComboBox(this);
    comboBoxPosition = new QComboBox(this);

    comboBoxClusterName->addItem(GamaQ2::obsClusterName);
    comboBoxClusterName->addItem(GamaQ2::xyzClusterName);
    comboBoxClusterName->addItem(GamaQ2::hdfClusterName);
    comboBoxClusterName->addItem(GamaQ2::vecClusterName);

    comboBoxPosition->addItem(tr("Last item in the cluster list"));
    comboBoxPosition->addItem(tr("After the selected cluster"));
    comboBoxPosition->addItem(tr("Before the selected cluster"));
    comboBoxPosition->addItem(tr("First item in the cluster list"));

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                     QDialogButtonBox::Cancel);
    QFormLayout* form = new QFormLayout;
    form->addRow(tr("Cluster"),  comboBoxClusterName);
    form->addRow(tr("Position"), comboBoxPosition);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addLayout(form);
    vbox->addWidget(buttons);
    setLayout(vbox);

    connect(buttons, &QDialogButtonBox::accepted, this, [this](){accept();});
    connect(buttons, &QDialogButtonBox::rejected, this, [this](){reject();});
}

InsertClusterDialog::~InsertClusterDialog()
{
}

QString InsertClusterDialog::clusterName() const
{
    return comboBoxClusterName->currentText();
}

int InsertClusterDialog::position() const
{
    return comboBoxPosition->currentIndex();
}
