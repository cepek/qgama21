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

#ifndef Network_Svg_H_network_svg_H_NETWORK_SVG_H
#define Network_Svg_H_network_svg_H_NETWORK_SVG_H

#include <QGraphicsView>

class NetworkSVG : public QGraphicsView
{
public:
    NetworkSVG(QWidget* parent=nullptr);
    ~NetworkSVG();

public slots:
    //void getSVG(const char* file);
    void getSVG(QByteArray bytea);

protected:
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void refresh();
    double svgWidth, svgHeight;
};

#endif
