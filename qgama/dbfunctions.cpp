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

#include "dbfunctions.h"
#include "constants.h"
#include "selectconfiguration.h"
#include "dbconnectdialog.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

DropDeleteBase::DropDeleteBase(QWidget *parent)
    : QWidget(parent), needsConfName(false), success(false)
{
}

DropDeleteBase::~DropDeleteBase()
{
}

DB_DropTables::DB_DropTables(QWidget *parent) : DropDeleteBase(parent) {}

void DB_DropTables::init()
{
    confirmWindowTitle = tr("Drop Gama Tables");
    confirmQuestion    = tr("Do you really want to remove all Gama tables from the database?");
    critical_1         = tr("Schema tables cannot be dropped from the implicitly open datatase.");
    critical_2         = tr("Database tables cannot be dropped. Database connection is not open.");
    critical_3         = tr("Database tables cannot be dropped.\n\n%1");
    query_string       = "DROP TABLE %1%2";      // table name, confName
    confName.clear();
}

DB_DeleteAllData::DB_DeleteAllData(QWidget *parent) : DropDeleteBase(parent) {}

void DB_DeleteAllData::init()
{
    confirmWindowTitle = tr("Delete Gama Tables");
    confirmQuestion    = tr("Do you really want to delete all data from Gama tables in the database?");
    critical_1         = tr("Data cannot be deleted from tables in the implicitly open datatase.");
    critical_2         = tr("Data cannot be deleted. Database connection is not open.");
    critical_3         = tr("Data cannot be deleted.\n\n%1");
    query_string       = "DELETE FROM %1%2";      // table name, confName
    confName.clear();
}

DB_DeleteNetworkConfiguration::DB_DeleteNetworkConfiguration(QWidget *parent) : DropDeleteBase(parent) {}

void DB_DeleteNetworkConfiguration::init()
{
    needsConfName      = true;
    confirmWindowTitle = tr("Delete Network Configuration");
    confirmQuestion    = tr("Do you really want to delete a network configuration in the database?");
    critical_1         = tr("Network configuration cannot be deleted in the implicitly open datatase.");
    critical_2         = tr("Network configuration cannot be deleted. Database connection is not open.");
    critical_3         = tr("Network configuration cannot be deleted.\n\n%1");
    query_string       = "DELETE FROM %1 WHERE conf_id=(SELECT conf_id "
                         "                              FROM   gnu_gama_local_configurations "
                         "                              WHERE  conf_name='%2')";  // table name, confName
    confName.clear();
}

void DB_DeleteNetworkConfiguration::getConfigurationName(QString name)
{

    confName = name;
    drop_or_delete_schema_tables();
    close();
}

// ----------------------------------------------------------------------------

bool DropDeleteBase::exec()
{
    setAttribute(Qt::WA_DeleteOnClose);
    init();

    QMessageBox confirm;
    confirm.setWindowTitle(confirmWindowTitle);
    confirm.setText(confirmQuestion);
    confirm.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    confirm.setDefaultButton  (QMessageBox::Cancel);

    if (confirm.exec() == QMessageBox::Yes)
    {
        if (needsConfName)
        {
            SelectConfiguration* sc = new SelectConfiguration(QGama::connection_implicit_db, false, false, this);
            connect(sc, SIGNAL(selectConfiguration(QString)), this, SLOT(getConfigurationName(QString)));
            sc->select();
            return success;
        }

        drop_or_delete_schema_tables();
    }

    close();
    return success;
}

void DropDeleteBase::drop_or_delete_schema_tables()
{
    QSqlDatabase impl= QSqlDatabase::database(QGama::connection_implicit_db);
    if (!impl.isOpen())
    {
        QMessageBox::critical(this, tr("Database Connection Failed"), critical_2);
        close();
        return;
    }

    QSqlQuery dq(impl);

    impl.transaction();

    QStringList tables = QGama::gama_local_schema_table_names;
    if (query_string.startsWith("DELETE")) tables.removeOne("gnu_gama_local_schema_version");
    if (needsConfName)                     tables.removeOne("gnu_gama_local_options");
    for (QStringList::const_iterator i=tables.cbegin(), e=tables.cend(); i!=e; ++i)
    {
        dq.exec(QString(query_string).arg(*i, confName));
        if (dq.lastError().isValid())
        {
            QMessageBox::critical(this, tr("Database Error"),
                                  critical_3.arg(dq.lastError().databaseText()));
            impl.rollback();
            impl.close();
            close();
            return;
        }
    }

    impl.commit();

    QMessageBox::information(this, confirmWindowTitle, tr("Operation succesfully completed"));
    success = true;
}
