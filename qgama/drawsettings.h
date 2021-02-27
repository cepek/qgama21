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

#ifndef DRAWSETTINGS_H
#define DRAWSETTINGS_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QAbstractButton>

class QComboBox;
class QCheckBox;
class QDoubleSpinBox;

namespace GNU_gama { namespace local {
    class LocalNetwork;
    class GamaLocalSVG;
}}

class DrawSettings : public QWidget
{
    Q_OBJECT
    
public:
    explicit DrawSettings(GNU_gama::local::GamaLocalSVG *lsvg, QWidget *parent = nullptr);
    ~DrawSettings();
    
signals:
    void redraw();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    QComboBox* comboBoxFixedSymbol;
    QComboBox* comboBoxConstrainedSymbol;
    QComboBox* comboBoxFreeSymbol;
    QComboBox* comboBoxFixedFill;
    QComboBox* comboBoxConstrainedFill;
    QComboBox* comboBoxFreeFill;

    QDoubleSpinBox* doubleSpinBoxFontSize;
    QDoubleSpinBox* doubleSpinBoxSymbolSize;
    QDoubleSpinBox* doubleSpinBoxStrokeWidth;
    QDoubleSpinBox* doubleSpinBoxEllipsesScale;

    QCheckBox* checkBoxDrawPointSymbols;
    QCheckBox* checkBoxDrawPointIDs;
    QCheckBox* checkBoxDrawEllipses;
    QCheckBox* checkBoxDrawObservations;
    QCheckBox* checkBoxDrawAxes;

    GNU_gama::local::GamaLocalSVG *svg;
    QStringList pointsymbols;
    QStringList pointfill;

    void setSpinBoxDefaults(QDoubleSpinBox* spinBox);
    void readSvgValues();

    bool r_drawPointSymbols, r_drawPointIDs, r_drawEllipses, r_drawObservations, r_drawAxes;
    double r_fontSize, r_symbolSize, r_strokeWidth, r_ellipsesScale;
    std::string r_fixedSymbol, r_constrainedSymbol, r_freeSymbol;
    std::string r_fixedFill, r_constrainedFill, r_freeFill;
};

#endif // DRAWSETTINGS_H
