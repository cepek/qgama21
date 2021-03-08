/*  Qt based GUI for GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2015  Ales Cepek <cepek@gnu.org>

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

#include "qgamahelp.h"

#include <QTextEdit>
#include <QFile>
#include <QDebug>

/* The C++11 standard §6.7.4:
 *           If control enters the declaration concurrently while the variable
 *           is being initialized, the concurrent execution shall wait for
 *           completion of the initialization.
 */
QGamaHelp* QGamaHelp::get()
{
    static QGamaHelp help;
    return &help;
}


// private constructor

QGamaHelp::QGamaHelp(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(tr("Qgama help"));

    QFile file(":/help/qgama.html");
    file.open(QIODevice::ReadOnly);
    QString text = file.readAll();

    auto edit = new QTextEdit;
    edit->setMinimumSize(800,500);
    edit->setText(text);
    edit->setReadOnly(true);

    setCentralWidget(edit);
}
