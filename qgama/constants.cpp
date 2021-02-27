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

QString GamaQ2::name;
QString GamaQ2::version;
QString GamaQ2::copyright;

QStringList GamaQ2::gama_local_schema;
QStringList GamaQ2::gama_local_schema_table_names;

QStringList GamaQ2::languages;
QString GamaQ2::currentLanguage;

const QString GamaQ2::connection_implicit_db     = "connection_implicit_db";
const QString GamaQ2::connection_dbf_drop_tables = "connection_dbf_drop_tables";

const QString GamaQ2::obsClusterName = "obs";
const QString GamaQ2::xyzClusterName = "xyz";
const QString GamaQ2::hdfClusterName = "hdf";
const QString GamaQ2::vecClusterName = "vec";

const QString GamaQ2::distObsName    = "dist.";
const QString GamaQ2::dirObsName     = "dir.";
const QString GamaQ2::angleObsName   = "angle";
const QString GamaQ2::azimObsName    = "azim.";
const QString GamaQ2::hdifObsName    = "h dif";
const QString GamaQ2::xdifObsName    = "x dif";
const QString GamaQ2::ydifObsName    = "y dif";
const QString GamaQ2::zdifObsName    = "z dif";
const QString GamaQ2::xObsName       = "x";
const QString GamaQ2::yObsName       = "y";
const QString GamaQ2::zObsName       = "z";
const QString GamaQ2::slopeObsName   = "slope";
const QString GamaQ2::zangleObsName  = "zangle";

const QString GamaQ2::vectorObsName      = "vector";
const QString GamaQ2::coordinateObsName  = "coordinates";

const QString GamaQ2::delegate_style_sheet = "QLineEdit {"
                                             " border: 0px white;"
                                             " background: white;"
                                             "}";

const QColor GamaQ2::colorClusterHeader     = QColor(210,210,210);
const QColor GamaQ2::colorPassiveBackground = QColor(240,240,240);   // Qt::lightGray is too dark
const QColor GamaQ2::colorOutsideCovMat     = Qt::lightGray;
const QColor GamaQ2::colorSingularCovMat    = Qt::red;

int GamaQ2::getUnique()
{
    static int unique;
    return ++unique;
}
