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

#ifndef ADJUSTEMENTDEMOPLUGIN_H
#define ADJUSTEMENTDEMOPLUGIN_H

#include <QObject>
#include <QString>
#include "gamaq2interfaces.h"

namespace GNU_gama {
namespace local {
    class LocalNetwork;
}
}
class NetworkAdjustmentPanel;

using LocalNetwork = GNU_gama::local::LocalNetwork;

class QFrame;

class AdjustmentDemoPlugin : public QObject, AdjustmentInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gnu.gama.gama-q2.AdjustmentInterface/1.0")
    Q_INTERFACES(AdjustmentInterface)

public:
    AdjustmentDemoPlugin();

    QString getName();
    QWidget* create(NetworkAdjustmentPanel* nadjp);

private:
    NetworkAdjustmentPanel* nap {nullptr};
    QString confName;
    LocalNetwork* lnet {nullptr};
    QFrame* frame;

    QFrame* getFrame();

private slots:
};

#endif // ADJUSTEMENTDEMOPLUGIN_H
