/*
  GNU Gama Qt based GUI
  Copyright (C) 2020 Ales Cepek <cepek@gnu.org>

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

#include "tableviewstyle.h"

// Helper function to define QTableView style for point and observation editors.

QString tableViewStyle()
{
  return QString(
        "QTableCornerButton::section{border-width:1px;"
        "background-color:transparent;"
        "border-color: lightgrey; border-style:solid}"

        "QHeaderView::section{border-width:1px;"
        "background-color:transparent;"
        "border-color: lightgrey; border-style:solid}"
        );
}
