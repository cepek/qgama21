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

#include "importconfigurationfile.h"
#include "constants.h"

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QHeaderView>
#include <QTableWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QSettings>
#include <QDebug>


// headers from gala-local-xml2sql.cpp
#include <gnu_gama/local/localnetwork2sql.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/exception.h>
#include <gnu_gama/xml/gkfparser.h>
#include <sstream>
#include <cctype>


ImportConfigurationFile::ImportConfigurationFile(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle(tr("Import XML input data"));
    setWindowModality(Qt::ApplicationModal);

    label_File = new QLabel;
    tableWidget_ExistingConfigurationNames = new QTableWidget;
    lineEdit_ConfigurationName = new QLineEdit;
    pushButton_Import = new QPushButton;

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("XML input file"), label_File);
    formLayout->addRow(tr("Existing configuration names"), tableWidget_ExistingConfigurationNames);
    formLayout->addRow(tr("New configuration name"), lineEdit_ConfigurationName);

    buttons = new QDialogButtonBox(QDialogButtonBox::Cancel |
                                                     QDialogButtonBox::Reset);
    buttons->addButton(tr("Import"), QDialogButtonBox::AcceptRole);

    QVBoxLayout* vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(formLayout);
    vBoxLayout->addWidget(buttons);
    setLayout(vBoxLayout);

    auto clicked = [this](QAbstractButton* button){
        switch (buttons->buttonRole(button)) {
        case QDialogButtonBox::RejectRole:
            close();
            break;
        case QDialogButtonBox::AcceptRole:
            import_configuration();
            break;
        case QDialogButtonBox::ResetRole:
            lineEdit_ConfigurationName->setText(basename);
            break;
        default:
            break;
        }};
    connect(buttons, &QDialogButtonBox::clicked, clicked);

    auto changed = [this](const QString& text){on_lineEdit_ConfigurationName_textChanged(text);};
    connect(lineEdit_ConfigurationName, &QLineEdit::textChanged, changed);

    auto doubleclicked = [this](int row, int col){
        lineEdit_ConfigurationName->setText(tableWidget_ExistingConfigurationNames->item(row,col)->text());
        };
    connect(tableWidget_ExistingConfigurationNames, &QTableWidget::cellDoubleClicked, doubleclicked);
}

ImportConfigurationFile::~ImportConfigurationFile()
{
}

void ImportConfigurationFile::exec()
{
    QSettings settings;
    QString importdir = settings.value(import_xmldir).toString();
    QFileDialog fileDialog(nullptr,tr("Open XML Input File"));
    if (!importdir.isEmpty()) fileDialog.setDirectory(importdir);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    QStringList filters;
    filters << tr("Gama XML input files (*.xml *.gkf)")
            << tr("Any files (*)");
    fileDialog.setNameFilters(filters);
    fileDialog.setViewMode(QFileDialog::Detail);

    if (!fileDialog.exec()) return;

    auto selected = fileDialog.selectedFiles();
    file = selected[0];
    settings.setValue(import_xmldir, fileDialog.directory().absolutePath());

    show();

    configure();
}

void ImportConfigurationFile::configure()
{
    QSqlDatabase db = QSqlDatabase::database(GamaQ2::connection_implicit_db);
    QSqlQuery query(db);

    {
        tableWidget_ExistingConfigurationNames->setColumnCount(1);

        QHeaderView* hv = new QHeaderView(Qt::Horizontal, tableWidget_ExistingConfigurationNames);
        tableWidget_ExistingConfigurationNames->setHorizontalHeader(hv);
        hv->setStretchLastSection(true);
        hv->setVisible(false);
        tableWidget_ExistingConfigurationNames->
                setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Existing Configuration Names")));

        query.exec("SELECT conf_name "
                   "FROM   gnu_gama_local_configurations "
                   "ORDER BY conf_name DESC ");
        while (query.next())
        {
            tableWidget_ExistingConfigurationNames->insertRow(0);

            QString conf = query.value(0).toString();
            QTableWidgetItem* item = new QTableWidgetItem(conf);
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            tableWidget_ExistingConfigurationNames->setItem(0,0,item);
        }
    }

    QFileInfo info(file);
    basename = info.baseName();
    label_File->setText(info.fileName());
    lineEdit_ConfigurationName->setText(basename);
}

void ImportConfigurationFile::on_lineEdit_ConfigurationName_textChanged(const QString &arg1)
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
    pushButton_Import->setEnabled(!found);
}

void ImportConfigurationFile::import_configuration()
{
    std::stringstream sql;
    QString qnm = label_File->text();

    QString xmlString;
    QFile inputFile(file);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        xmlString = in.readAll();
        inputFile.close();
    }
    else
    {
        QMessageBox::critical(this, tr("Critical Error on Opening File"), qnm);
        close();
        return;
    }

    bool repeat {false};
    do try {             // do try ... while (repeat)
        repeat = false;
        std::istringstream xml(xmlString.toUtf8().data());

        const QString qconfname = lineEdit_ConfigurationName->text();
        GNU_gama::local::LocalNetwork lnet;
        GNU_gama::local::LocalNetwork2sql imp(lnet);
        imp.readGkf(xml);
        imp.write  (sql, qconfname.toUtf8().data());
    }
    catch (const GNU_gama::local::ParserException& v)
    {
        QDialog dialog;

        dialog.setWindowTitle(tr("Import failed"));
        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(new QLabel(tr("File: ") + file + "\n" +
                                     tr("Error at line: ") + QString::number(v.line) +
                                     tr(" : ") + v.what()));

        line = new QLabel(this);
        layout->addWidget(line);

        edit = new QPlainTextEdit(xmlString, this);
        QTextCursor cursor = edit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,v.line-1);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        edit->setTextCursor(cursor);
        edit->setFocus();
        layout->addWidget(edit);

        connect(edit, SIGNAL(cursorPositionChanged()), this, SLOT(setXmlLine()));
        emit edit->cursorPositionChanged();

        QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok|
                                                         QDialogButtonBox::Cancel);
        connect(buttons, SIGNAL(accepted()), &dialog, SLOT(accept()));
        connect(buttons, SIGNAL(rejected()), &dialog, SLOT(reject()));
        layout->addWidget(buttons);

        dialog.setLayout(layout);
        int result = dialog.exec();
        if (result == QDialog::Accepted)
        {
            xmlString = edit->toPlainText();
            repeat = true;
        }
        else
        {
            close();
            return;
        }
    }
    catch (const GNU_gama::local::Exception& v)
    {
        QMessageBox::warning(this, tr("Import failed"), v.what());
        close();
        return;
    }
    catch (...)
    {
        QMessageBox::warning(this, tr("Import failed"), tr("Unknown exception"));
        close();
        return;
    }
    while (repeat);   // do try ... while (repeat)

    QSqlDatabase db = QSqlDatabase::database(GamaQ2::connection_implicit_db);
    QSqlQuery query(db);

    db.transaction();

    QString str;
    std::string text;
    while(std::getline(sql, text))
    {
        if (text == "begin;") continue;
        if (text == "commit;") continue;

        bool EndOfSQL = false;
        for (std::string::const_reverse_iterator i=text.rbegin(), e=text.rend(); i!=e; ++i)
            if (!std::isspace(*i))
            {
                if (*i == ';')
                {
                    EndOfSQL = true;
                    break;
                }
            }

        str += QString::fromUtf8(text.c_str());
        str += "\n";

        if (EndOfSQL)
        {
            query.exec(str);
            if (query.lastError().isValid())
            {
                QMessageBox::critical(this, tr("Database error"),
                                      query.lastError().databaseText() + "\n" + str);
                db.rollback();
                close();
                return;
            }
            str.clear();
        }
    }

    db.commit();
    close();
}

void ImportConfigurationFile::setXmlLine()
{
    QString str(tr("XML Line: %1"));
    line->setText(str.arg(edit->textCursor().blockNumber() + 1));
}
