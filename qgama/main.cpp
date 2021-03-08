/*  Qt based GUI for GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2014, 2019  Ales Cepek <cepek@gnu.org>

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

#include <locale>

#include <QApplication>
#include <QSettings>
#include "qgamacontrolpanel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("GNU Gama");
    QCoreApplication::setOrganizationDomain("gnu.org/software/gama/");
    QCoreApplication::setApplicationName("qgama");

    QApplication a(argc, argv);
    QIcon icon(":/img/g-logo.svg");
    QApplication::setWindowIcon(icon);

    QSettings settings;
    QVariant sf = settings.value("qgama-font");
    if (sf.isValid())
      {
        QFont font;
        bool ok = font.fromString(sf.toString());
        if (ok) QApplication::setFont(font);
      }

    // https://en.cppreference.com/w/cpp/locale/locale/classic
    std::locale::global( std::locale::classic() );

    QGamaControlPanel w;
    w.show();

    return a.exec();
}
