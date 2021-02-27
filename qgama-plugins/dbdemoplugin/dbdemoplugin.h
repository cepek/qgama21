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

#ifndef DBDEMO_H
#define DBDEMO_H

#include <QObject>
#include "gamaq2interfaces.h"

class QFrame;
class QLineEdit;
class QTableView;
class QSqlQueryModel;

class DBdemoPlugin : public QObject, DbInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.gnu.gama.gama-q2.DbInterface/1.0")
#endif
    Q_INTERFACES(DbInterface)

public:
    DBdemoPlugin();

    QString getName();
    QWidget* create();

private:
    QFrame* getFrame();

    QFrame     *frame;
    QLineEdit  *edit;
    QTableView *tableView;
    QSqlQueryModel *model;

private slots:
    void selectData(QString str);
};

#endif // DBDEMO_H
