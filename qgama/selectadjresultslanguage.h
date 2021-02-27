/*  Qt based GUI for GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2014, 2016  Ales Cepek <cepek@gnu.org>

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

#ifndef SELECTADJRESULTSLANGUAGE_H
#define SELECTADJRESULTSLANGUAGE_H

#include <QDialog>

class QComboBox;
class QLabel;
class QDialogButtonBox;

class SelectAdjResultsLanguage : public QDialog
{
    Q_OBJECT

public:
    explicit SelectAdjResultsLanguage(QWidget *parent = nullptr);
    ~SelectAdjResultsLanguage();

private slots:
    void on_buttonBox_accepted();

private:
    QLabel* label {nullptr};
    QComboBox* comboBox {nullptr};
    QDialogButtonBox* buttonBox {nullptr};
};

#endif // SELECTADJRESULTSLANGUAGE_H
