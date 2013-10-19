/*
  GNU Gama Qt based GUI
  Copyright (C) 2013 Ales Cepek <cepek@gnu.org>

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

#ifndef GAMAG2CONTROLPANEL_H
#define GAMAG2CONTROLPANEL_H

#include <QMainWindow>
#include <QMap>
#include "showmessage.h"

class DbInterface;

class GamaQ2ControlPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit GamaQ2ControlPanel(QWidget *parent = 0);
    ~GamaQ2ControlPanel();

private slots:
    void on_action_Exit_triggered();
    void on_action_Connect_to_database_triggered();
    void on_action_Import_configuration_file_triggered();
    void on_action_Network_adjustment_triggered();
    void on_action_Drop_schema_Tables_triggered();
    void on_action_Delete_all_Data_from_the_Schema_triggered();
    void on_action_Delete_Network_Configuration_triggered();
    void disable_input_data(bool);
    void dbSlot();

private:
    QAction *actionDbConnect, *actionDbImport, *actionDbDropSchema,
            *actionDbDeleteData, *actionDbDeleteConfiguration,
            *actionDbExit;
    QAction *actionAdjAdjustment;

    QMap<QString, DbInterface*> mapDbPlugins;

    void closeEvent(QCloseEvent *);
    void init_schema_lists();
    void build_menus();
    void load_plugins();

signals:
    void gamaCloseSignal();
};

#endif // GAMAG2CONTROLPANEL_H