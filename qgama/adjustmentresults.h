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

#ifndef ADJUSTMENTRESULTS_H
#define ADJUSTMENTRESULTS_H

#include <QWidget>

namespace GNU_gama { namespace local {
class LocalNetwork;
class GamaLocalHTML;
}}

class QTextEdit;

class AdjustmentResults : public QWidget
{
    Q_OBJECT
    
public:
    explicit AdjustmentResults(QWidget *parent = nullptr);
    ~AdjustmentResults();
    
    void set_local_network(GNU_gama::local::LocalNetwork* local_network);
    bool has_local_network() const { return html != nullptr; }

    QString getHtmlUtf8() const;

public slots:
    void update_adjustment_results();

private:
    friend class NetworkAdjustmentPanel;
    QTextEdit* textEdit {nullptr};
    GNU_gama::local::GamaLocalHTML *html;
};

#endif // ADJUSTMENTRESULTS_H
