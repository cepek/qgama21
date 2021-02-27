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

#ifndef IMPORTCONFIGURATIONFILE_H
#define IMPORTCONFIGURATIONFILE_H

#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>

class QTableWidget;
class QLineEdit;
class QPushButton;
class QDialogButtonBox;

class ImportConfigurationFile : public QWidget
{
    Q_OBJECT

public:
    explicit ImportConfigurationFile(QWidget *parent = nullptr);
    ~ImportConfigurationFile();

    void exec();

private slots:
    void on_lineEdit_ConfigurationName_textChanged(const QString &arg1);
    void import_configuration();
    void setXmlLine();

private:
    QString file;
    QLabel* line {};
    QLabel* label_File {};
    QPlainTextEdit* edit {};
    QLineEdit* lineEdit_ConfigurationName {};
    QTableWidget* tableWidget_ExistingConfigurationNames {};
    QPushButton* pushButton_Import;
    QDialogButtonBox* buttons;
    QString basename;

    // QSettings
    const QString import_xmldir {"import/xmldir"};

    void configure();
};


#endif // IMPORTCONFIGURATIONFILE_H
