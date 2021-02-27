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

#include <QtPlugin>
#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

#include "adjustmentdemoplugin.h"
#include "networkadjustmentpanel.h"
#include <gnu_gama/local/network.h>

#include <QGridLayout>
#include <QLabel>

#include <QDebug>

AdjustmentDemoPlugin::AdjustmentDemoPlugin()
{
}

QString AdjustmentDemoPlugin::getName()
{
    return "Adjustment demo plugin";
}

QWidget* AdjustmentDemoPlugin::create(NetworkAdjustmentPanel* adjp)
{
    NetworkAdjustmentPanel* nap = adjp;
    confName = nap->configurationName();
    lnet = nap->adjustment()->get_local_network();

    return getFrame();
}

QFrame* AdjustmentDemoPlugin::getFrame()
{
    frame = new QFrame;
    if (frame) {

        frame->setWindowTitle("Adjustement plugin demo '" + confName + "'");

        auto layout = new QGridLayout;
        layout->addWidget(new QLabel("Configuration"), 0, 0);
        layout->addWidget(new QLabel(confName), 0, 1);
        layout->addWidget(new QLabel("Points"), 1, 0);
        int p = lnet->PD.size();
        layout->addWidget(new QLabel(QString::number(p)), 1, 1);

        using namespace GNU_gama::local;
        int obs {}, dis {}, dir {};
        //for (GNU_gama::local::ObservationData::iterator t = lnet->OD.begin();
        //     t != lnet->OD.end(); ++t) ... dynamic_cast<Distance*>(*t)) dis++;
        for (auto t : lnet->OD)
        {
            obs++;
            if      (/*Distance*  ds =*/ dynamic_cast<Distance*> (t)) dis++;
            else if (/*Direction* dr =*/ dynamic_cast<Direction*>(t)) dir++;
        }
        layout->addWidget(new QLabel("Observations"), 2, 0);
        layout->addWidget(new QLabel(QString::number(obs)), 2, 1);
        layout->addWidget(new QLabel("Distances"), 3, 0);
        layout->addWidget(new QLabel(QString::number(dis)), 3, 1);
        layout->addWidget(new QLabel("Directions"), 4, 0);
        layout->addWidget(new QLabel(QString::number(dir)), 4, 1);

        int clusters = lnet->OD.clusters.size();
        layout->addWidget(new QLabel("Clusters"), 5, 0);
        layout->addWidget(new QLabel(QString::number(clusters)), 5, 1);

        frame->setLayout(layout);
        frame->setMinimumSize(300,100);
    }

    return frame;
}
