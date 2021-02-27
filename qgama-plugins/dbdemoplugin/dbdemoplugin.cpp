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

#include <QtPlugin>
#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

#include "dbdemoplugin.h"

#include <QDebug>

DBdemoPlugin::DBdemoPlugin()
{
}

QString DBdemoPlugin::getName()
{
    return "DB demo plugin";
}

QWidget* DBdemoPlugin::create()
{
    QSqlDatabase db = QSqlDatabase::database("connection_implicit_db");
    if (!db.isOpen())
    {
        QMessageBox::critical(0, tr("DB demo plugin"),
                              tr("Database is not open, the plugin cannot be started"));
        return 0;
    }

    return getFrame();
}

QFrame* DBdemoPlugin::getFrame()
{
    frame = new QFrame;
    if (frame) {
        frame->setWindowTitle(tr("DB demo plugin"));

        tableView = new QTableView(frame);
        tableView->setMinimumSize(400,250);

        edit = new QLineEdit(frame);

        QGridLayout* grid = new QGridLayout(frame);
        grid->addWidget(edit);
        grid->addWidget(tableView);
        frame->setLayout(grid);
        frame->show();

        model = new QSqlQueryModel(frame);

        selectData("");

        connect(edit, SIGNAL(textChanged(QString)), SLOT(selectData(QString)));
    }

    return frame;
}

void DBdemoPlugin::selectData(QString str)
{
    QString sql =
            "SELECT   conf_name, COUNT(x) AS \"COUNT(x,y)\", "
            "         COUNT(z) AS \"COUNT(z)\" "
            "FROM     gnu_gama_local_configurations JOIN "
            "         gnu_gama_local_points USING (conf_id) "
            "WHERE    conf_name LIKE '%1%' "
            "GROUP BY conf_id, conf_name "
            "ORDER BY conf_name ASC ";

    QSqlDatabase db = QSqlDatabase::database("connection_implicit_db");
    model->setQuery(sql.arg(str.simplified()), db);
    tableView->setModel(model);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(dbdemoplugin, DBdemoPlugin)
#endif
