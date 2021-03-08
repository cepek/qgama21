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

#include "constants.h"

QString QGama::name;
QString QGama::version;
QString QGama::copyright;

QStringList QGama::gama_local_schema;
QStringList QGama::gama_local_schema_table_names;

QStringList QGama::languages;
QString QGama::currentLanguage;

const QString QGama::connection_implicit_db     = "connection_implicit_db";
const QString QGama::connection_dbf_drop_tables = "connection_dbf_drop_tables";

const QString QGama::obsClusterName = "obs";
const QString QGama::xyzClusterName = "xyz";
const QString QGama::hdfClusterName = "hdf";
const QString QGama::vecClusterName = "vec";

const QString QGama::distObsName    = "dist.";
const QString QGama::dirObsName     = "dir.";
const QString QGama::angleObsName   = "angle";
const QString QGama::azimObsName    = "azim.";
const QString QGama::hdifObsName    = "h dif";
const QString QGama::xdifObsName    = "x dif";
const QString QGama::ydifObsName    = "y dif";
const QString QGama::zdifObsName    = "z dif";
const QString QGama::xObsName       = "x";
const QString QGama::yObsName       = "y";
const QString QGama::zObsName       = "z";
const QString QGama::slopeObsName   = "slope";
const QString QGama::zangleObsName  = "zangle";

const QString QGama::vectorObsName      = "vector";
const QString QGama::coordinateObsName  = "coordinates";

const QString QGama::delegate_style_sheet = "QLineEdit {"
                                             " border: 0px white;"
                                             " background: white;"
                                             "}";

const QColor QGama::colorClusterHeader     = QColor(210,210,210);
const QColor QGama::colorPassiveBackground = QColor(240,240,240);   // Qt::lightGray is too dark
const QColor QGama::colorOutsideCovMat     = Qt::lightGray;
const QColor QGama::colorSingularCovMat    = Qt::red;

int QGama::getUnique()
{
    static int unique;
    return ++unique;
}
