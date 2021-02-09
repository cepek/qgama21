/*
  GNU Gama Qt based GUI
  Copyright (C) 2013, 2016 Ales Cepek <cepek@gnu.org>

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

#include "selectconfiguration.h"
#include "constants.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCompleter>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QTableWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QHeaderView>
#include <QDebug>

SelectConfiguration::SelectConfiguration(QString connectionName, bool tabsDocks,
                                         bool newConfName, QWidget *parent) :
    QDialog(parent),
    connection_name(connectionName),
    tabDockRadioButtons(tabsDocks),
    enableNewConfigurationName(newConfName),
    tabbedWidgets(true)
{
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Select Configuration Name"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                     QDialogButtonBox::Cancel);
    radioButton_tabs = new QRadioButton(tr("Tabbed windows"));
    radioButton_dock = new QRadioButton(tr("Docked windows"));
    tableWidget_ExistingConfigurationNames = new QTableWidget;
    lineEdit_ConfigurationName = new QLineEdit;
    label_DatabaseName = new QLabel;

    // Ok button is disabled until a valid configurations is selected
    QPushButton *okButton=buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);

    QFormLayout* form = new QFormLayout;
    form->addRow(tr("Database Name"), label_DatabaseName);
    form->addRow(tr("Existing Configurations"), tableWidget_ExistingConfigurationNames);
    form->addRow(tr("Configuration Name"), lineEdit_ConfigurationName);
    if (tabDockRadioButtons)
    {
        radioButton_tabs->setChecked(true);
        form->addRow("", radioButton_tabs);
        form->addRow("", radioButton_dock);
    }
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(form);
    layout->addWidget(buttonBox);
    setLayout(layout);

    connect(buttonBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton* button){
        switch (buttonBox->buttonRole(button)) {
        case QDialogButtonBox::RejectRole:
            close();
            break;
        case QDialogButtonBox::AcceptRole:
            emit selectConfiguration(lineEdit_ConfigurationName->text(), tabbedWidgets);
            close();
            break;
        default:
            break;
        }
    });
    connect(lineEdit_ConfigurationName, &QLineEdit::textChanged, this, [this](const QString & text){
        on_lineEdit_ConfigurationName_textChanged(text);
    });
    connect(tableWidget_ExistingConfigurationNames, &QTableWidget::cellClicked, this, [this](int row, int col){
        QString txt = tableWidget_ExistingConfigurationNames->item(row,col)->text();
        lineEdit_ConfigurationName->setText(txt);
    });
    connect(radioButton_tabs, &QRadioButton::toggled, this, [this](bool){
        tabbedWidgets = radioButton_tabs->isChecked();
    });
}

SelectConfiguration::~SelectConfiguration()
{
}

void SelectConfiguration::select()
{
    QStringList confs;
    {
        QSqlDatabase db = QSqlDatabase::database(connection_name);
        label_DatabaseName->setText(db.databaseName());

        tableWidget_ExistingConfigurationNames->setColumnCount(1);
        QHeaderView* hv = new QHeaderView(Qt::Horizontal, tableWidget_ExistingConfigurationNames);
        hv->setStretchLastSection(true);
        hv->setVisible(false);
        tableWidget_ExistingConfigurationNames->setHorizontalHeader(hv);

        QSqlQuery query(db);
        query.exec("SELECT conf_name FROM gnu_gama_local_configurations "
                   "ORDER BY conf_name DESC");
        while (query.next())
        {
            tableWidget_ExistingConfigurationNames->insertRow(0);
            QString conf = query.value(0).toString();
            QTableWidgetItem* item = new QTableWidgetItem(conf);
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);  // the table must be readonly

            tableWidget_ExistingConfigurationNames->setItem(0,0,item);

            confs << query.value(0).toString();
        }
    }

    QCompleter *completer = new QCompleter(confs, this);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    lineEdit_ConfigurationName->setCompleter(completer);

    show();
}

void SelectConfiguration::on_lineEdit_ConfigurationName_textChanged(const QString &arg1)
{
    const QRegularExpression whitespaces("\\s");
    lineEdit_ConfigurationName->setText(QString(arg1).remove(whitespaces));

    bool found  = false;
    int  row  = 0;
    int  rows = tableWidget_ExistingConfigurationNames->rowCount();
    for (int i=rows-1; i>=0; i--)
    {
        QString item = tableWidget_ExistingConfigurationNames->item(i, 0)->text();
        if (item.startsWith(arg1)) row = i;
        if (item == arg1)
        {
            found = true;
            break;
        }
    }

    tableWidget_ExistingConfigurationNames->selectRow(row);
    QPushButton *okButton=buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(found || enableNewConfigurationName);
}

