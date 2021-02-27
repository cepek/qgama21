/*
  GNU Gama Qt based GUI
  Copyright (C) 2013, 2016 Ales Cepek <cepek@gnu.org>

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

#ifndef NETWORKADJUSTMENTPANEL_H
#define NETWORKADJUSTMENTPANEL_H

#include "adjustment.h"

#include <QMainWindow>

class ParameterEditor;
class PointEditor;
class ObservationEditor;
class NetworkSVG;
class AdjustmentResults;

class NetworkAdjustmentPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit NetworkAdjustmentPanel(QString connectionName, QWidget *parent = nullptr);
    ~NetworkAdjustmentPanel();

    void exec();
    bool isValid() const { return valid; }
    Adjustment* adjustment() {return &adj; }

    QString configurationName() const { return configuration_name; }

    static bool        closeAllNetworkAdjustmentPanels;
    static QWidgetList allNetworkAdjustmentPanelsList;

private:

    enum {menu_parameters, menu_points, menu_observations };

    Adjustment         adj;
    ParameterEditor   *par;
    PointEditor       *ped;
    ObservationEditor *obs;
    NetworkSVG        *svg;
    AdjustmentResults *res;

    QWidgetList localPluginsList;

    QString connection_name;
    QString configuration_name;
    bool    useTabbedWidgets;
    bool    valid;

    QMenu*   menuFile           {nullptr};
    QMenu*   menuAdjExport      {nullptr};
    QMenu*   menuAdjustment     {nullptr};
    QMenu*   menuView           {nullptr};
    QMenu*   menuEdit           {nullptr};
    QMenu*   menuSetup          {nullptr};
    QMenu*   menuTools          {nullptr};
    QMenu*   menuHelp           {nullptr};
    QAction* actionParameters   {nullptr};
    QAction* actionPoints       {nullptr};
    QAction* actionObservations {nullptr};

    void set_gui_adjustment_functions_status(bool isvalid);

private slots:

    void update_point_editor_table();
    void update_observation_editor_table();

    void getConfigurationName(QString conf, bool tabbed);
    void draw_network_configuration();
    void tabIndexChanged(int n);
    void update_adjustment_results(bool solved=false);
    void status_bar(QString text);
    void save_configuration(QString confname);

    void action_Export_XML_adjustment_results();
    void action_Export_adjustment_results_as_plain_text();
    void action_Export_adjustment_results_as_octave_file();
    void action_Export_adjustment_results_as_HTML();
    void action_Close();
    void action_Save_as_SQL_file();
    void action_Run();
    void action_Save_network_configuration_outline();
    void action_Outline_draw();
    void closeEvent(QCloseEvent *event);
    void action_Points_changed();
    void action_Parameters_changed();
    void action_Observations_changed();
    void action_Save_into_db();
    void action_Save_As_into_db();
    void action_Print();

    void action_Gama_q2_help();

    void AdjustmentPluginSlot();
signals:

    void networkAdjustmentPanel(bool newPanel);
};

#endif // NETWORKADJUSTMENTPANEL_H
