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

#ifndef INSERTCLUSTERDIALOG_H
#define INSERTCLUSTERDIALOG_H

#include <QDialog>

class QComboBox;

class InsertClusterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit InsertClusterDialog(QWidget *parent = nullptr);
    ~InsertClusterDialog();

    QString clusterName() const;
    int     position() const;
    
private:
    QComboBox* comboBoxClusterName {};
    QComboBox* comboBoxPosition {};
};

#endif // INSERTCLUSTERDIALOG_H
