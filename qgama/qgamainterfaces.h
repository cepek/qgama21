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

#ifndef QGAMA_INTERFACES_qgama_interfaces_H
#define QGAMA_INTERFACES_qgama_interfaces_H

#include <QString>
class QWidget;

class NetworkAdjustmentPanel;

class DbInterface
{
public:
    virtual ~DbInterface() {}
    virtual QString getName() = 0;
    virtual QWidget* create() = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(DbInterface, "org.gnu.gama.gama-q2.DbInterface/1.0")
QT_END_NAMESPACE


class AdjustmentInterface
{
public:
    virtual ~AdjustmentInterface() {}
    virtual QString getName() = 0;
    virtual QWidget* create(NetworkAdjustmentPanel* nap) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(AdjustmentInterface, "org.gnu.gama.gama-q2.AdjustmentInterface/1.0")
QT_END_NAMESPACE

#endif // QGAMA_INTERFACES_qgama_interfaces_H
