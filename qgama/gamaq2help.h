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

#ifndef GAMAQ2HELP_H
#define GAMAQ2HELP_H

#include <QMainWindow>

class GamaQ2help : public QMainWindow
{
    Q_OBJECT
public:

     static GamaQ2help* get();

private:
     explicit GamaQ2help(QWidget *parent = nullptr);
};

#endif // GAMAQ2HELP_H
