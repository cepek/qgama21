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

#include "networksvg.h"

#include <QFile>
#include <QGraphicsSvgItem>
#include <QWheelEvent>
#include <QSvgRenderer>

#include <cmath>
#include <iostream>

NetworkSVG::NetworkSVG(QWidget* parent)
    : QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);

    svgWidth = svgHeight = 0;
}

NetworkSVG::~NetworkSVG()
{
}

void NetworkSVG::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
}

void NetworkSVG::wheelEvent(QWheelEvent *event)
{
    // double factor = std::pow(1.2, event->delta() / 240.0);
    QPoint d = event->angleDelta();
    double factor = std::pow(1.2, (d.x()+d.y()) / 240.0);
    scale(factor, factor);
    event->accept();
}

void NetworkSVG::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Refresh))
    {
        refresh();
    }
}

/*void NetworkSVG::getSVG(const char *file_name)
{
    QGraphicsScene* s = scene();
    s->clear();
    resetTransform();

    QGraphicsSvgItem* item = new QGraphicsSvgItem(file_name);
    s->addItem(item);

    QGraphicsRectItem* r = new QGraphicsRectItem(item->boundingRect());
    r->setOpacity(0.1);
    s->addItem(r);
}*/

void NetworkSVG::getSVG(QByteArray bytea)
{
    QSvgRenderer* renderer = new QSvgRenderer();
    renderer->load(bytea);

    QGraphicsScene* s = scene();
    s->clear();

    QGraphicsSvgItem* svg = new QGraphicsSvgItem;
    svg->setSharedRenderer(renderer);
    s->addItem(svg);

    svgWidth  = svg->boundingRect().width();
    svgHeight = svg->boundingRect().height();
    refresh();

#if 0
    QGraphicsRectItem* r = new QGraphicsRectItem(svg->boundingRect());
    r->setOpacity(0.1);
    s->addItem(r);
#endif
}

void NetworkSVG::refresh()
{
    if (svgWidth == 0 || svgHeight == 0) return;

    double factor = 1;
    if (svgWidth > svgHeight)
    {
        factor = width()/svgWidth;
        if (svgHeight*factor > height()) factor = height()/svgHeight;
    }
    else
    {
        factor = height()/svgHeight;
        if (svgWidth*factor > width()) factor = width()/svgWidth;
    }

    resetTransform();
    scale(factor, factor);
}
