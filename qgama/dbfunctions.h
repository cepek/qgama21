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

#ifndef DBFUNCTIONS_H
#define DBFUNCTIONS_H

#include <QWidget>

class DropDeleteBase : public QWidget
{
    Q_OBJECT

public:
    DropDeleteBase(QWidget* parent);
    virtual ~DropDeleteBase();

    bool exec();

protected slots:
    void drop_or_delete_schema_tables();

protected:
    virtual void init() = 0;

    QString confirmWindowTitle;
    QString confirmQuestion;
    QString critical_1;
    QString critical_2;
    QString critical_3;
    QString query_string;
    QString confName;
    bool    needsConfName;
    bool    success;
};

class DB_DropTables : public DropDeleteBase
{
public:
    DB_DropTables(QWidget* parent);

protected:
     virtual void init();

};

class DB_DeleteAllData : public DropDeleteBase
{
public:
    DB_DeleteAllData(QWidget* parent);

protected:
     virtual void init();

};

class DB_DeleteNetworkConfiguration : public DropDeleteBase
{
    Q_OBJECT
public:
    DB_DeleteNetworkConfiguration(QWidget* parent);

protected:
     virtual void init();

private slots:
    void getConfigurationName(QString name);
};




#endif // DBFUNCTIONS_H
