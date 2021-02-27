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

#include "adjustmentresults.h"
#include <QTextEdit>
#include <QGridLayout>
#include <gnu_gama/local/html.h>
#include <gnu_gama/local/network.h>

AdjustmentResults::AdjustmentResults(QWidget *parent) :
    QWidget(parent), html(nullptr)
{
    textEdit = new QTextEdit;
    textEdit->setReadOnly(true);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(textEdit);
    setLayout(layout);
}

AdjustmentResults::~AdjustmentResults()
{
    delete html;
}

void AdjustmentResults::set_local_network(GNU_gama::local::LocalNetwork *local_network)
{
    delete html;
    html = new GNU_gama::local::GamaLocalHTML(local_network);
}

void AdjustmentResults::update_adjustment_results()
{
    if (html == nullptr) return;

    html->exec();
    const std::string& utf8 = html->str();

    textEdit->clear();
    textEdit->insertHtml(QString::fromUtf8(utf8.c_str(), utf8.length()));
    textEdit->moveCursor(QTextCursor::Start);
}

QString AdjustmentResults::getHtmlUtf8() const
{
    return QString::fromUtf8(html->str().c_str());
}
