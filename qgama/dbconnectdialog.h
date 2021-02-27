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

#ifndef DBCONNECTDIALOG_H
#define DBCONNECTDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QStackedWidget;
class QFormLayout;
class QComboBox;
class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

class QSqlDatabase;

class DBConnectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DBConnectDialog(QString connectionName, QWidget *parent = nullptr);
    ~DBConnectDialog();
    
private:
    QString connection_name;

    void create_missing_tables(QSqlDatabase& db);

    QComboBox* comboBox_Driver {};
    QComboBox* comboBox_Driver2 {};
    QLineEdit* lineEdit_DatabaseName {};
    QLineEdit* lineEdit_DatabaseFile {};
    QLineEdit* lineEdit_Hostname {};
    QLineEdit* lineEdit_Username {};
    QLineEdit* lineEdit_Password {};
    QLineEdit* lineEdit_Port {};
    QStackedWidget* stackedWidget {};
    QDialogButtonBox* buttonBox {};
    QPushButton* pushButton_CreateNewDbFile {};
    QCheckBox* checkBox_CreateNewDbFile {};

    // QSettings
    const QString sqlite_dbfile_ {"sqlite/dbfile"};

private slots:
    void switchStackedWidgets();
    void on_pushButton_OpenFileDialog_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_buttonBox_helpRequested();

signals:
    void input_data_open(bool);
};

#endif // DBCONNECTDIALOG_H
