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

#include "drawsettings.h"

#include <gnu_gama/local/svg.h>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>

DrawSettings::DrawSettings(GNU_gama::local::GamaLocalSVG *lsvg, QWidget *parent) :
    QWidget(parent),
    svg(lsvg)
{
    comboBoxFixedSymbol = new QComboBox;
    comboBoxConstrainedSymbol = new QComboBox;
    comboBoxFreeSymbol = new QComboBox;
    comboBoxFixedFill = new QComboBox;
    comboBoxConstrainedFill = new QComboBox;
    comboBoxFreeFill = new QComboBox;

    doubleSpinBoxFontSize = new QDoubleSpinBox;
    doubleSpinBoxSymbolSize = new QDoubleSpinBox;
    doubleSpinBoxStrokeWidth = new QDoubleSpinBox;
    doubleSpinBoxEllipsesScale = new QDoubleSpinBox;

    checkBoxDrawPointSymbols = new QCheckBox(tr("Point Symbols"));
    checkBoxDrawPointIDs = new QCheckBox(tr("Point IDs"));
    checkBoxDrawEllipses = new QCheckBox(tr("Ellipses"));
    checkBoxDrawObservations = new QCheckBox(tr("Observations"));
    checkBoxDrawAxes = new QCheckBox(tr("Axes"));

    pointsymbols << "triangle" << "circle" << "none";
    comboBoxFixedSymbol      ->addItems(pointsymbols);
    comboBoxConstrainedSymbol->addItems(pointsymbols);
    comboBoxFreeSymbol       ->addItems(pointsymbols);

    // 16 basic colors
    pointfill << "aqua"   << "black" << "blue"   << "fuchsia"
              << "gray"   << "green" << "lime"   << "maroon"
              << "navy"   << "olive" << "purple" << "red"
              << "silver" << "teal"  << "white"  << "yellow"
              << "none";
    comboBoxFixedFill      ->addItems(pointfill);
    comboBoxConstrainedFill->addItems(pointfill);
    comboBoxFreeFill       ->addItems(pointfill);

    setSpinBoxDefaults(doubleSpinBoxFontSize);
    setSpinBoxDefaults(doubleSpinBoxSymbolSize);
    setSpinBoxDefaults(doubleSpinBoxStrokeWidth);
    setSpinBoxDefaults(doubleSpinBoxEllipsesScale);

    readSvgValues();

    // stored values for Reset function
    r_drawPointSymbols = svg->drawPointSymbols();
    r_drawPointIDs = svg->drawPointIDs();
    r_drawEllipses = svg->drawEllipses();
    r_drawObservations = svg->drawObservations();
    r_drawAxes = svg->drawAxes();

    r_fontSize = svg->fontSize();
    r_symbolSize = svg->symbolSize();
    r_strokeWidth = svg->strokeWidth();
    r_ellipsesScale = svg->ellipsesScale();

    r_fixedSymbol = svg->fixedSymbol();
    r_constrainedSymbol = svg->constrainedSymbol();
    r_freeSymbol = svg->freeSymbol();

    r_fixedFill = svg->fixedFill();
    r_constrainedFill = svg->constrainedFill();
    r_freeFill = svg->freeFill();

    QHBoxLayout* hBoxLayout = new QHBoxLayout;

    QGroupBox* groupBox_Draw = new QGroupBox(tr("Draw"));
    {
        QVBoxLayout* vBoxLayout_Draw = new QVBoxLayout;
        vBoxLayout_Draw->addWidget(checkBoxDrawPointSymbols);
        vBoxLayout_Draw->addWidget(checkBoxDrawPointIDs);
        vBoxLayout_Draw->addWidget(checkBoxDrawEllipses);
        vBoxLayout_Draw->addWidget(checkBoxDrawObservations);
        vBoxLayout_Draw->addWidget(checkBoxDrawAxes);

        groupBox_Draw->setLayout(vBoxLayout_Draw);
    }
    QGroupBox* groupBox_Size = new QGroupBox(tr("Size"));
    {
        QFormLayout* form = new QFormLayout;
        form->addRow(tr("Font size"), doubleSpinBoxFontSize);
        form->addRow(tr("Symbol size"), doubleSpinBoxSymbolSize);
        form->addRow(tr("Stroke width"), doubleSpinBoxStrokeWidth);
        form->addRow(tr("Ellipses scale"), doubleSpinBoxEllipsesScale);

        groupBox_Size->setLayout(form);
    }
    hBoxLayout->addWidget(groupBox_Draw);
    hBoxLayout->addWidget(groupBox_Size);

    QGroupBox* groupBox_Point_Symbols = new QGroupBox(tr("Point Symbols"));
    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("Points")), 0, 0, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel(tr("Symbol")), 0, 1, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel(tr("Fill")  ), 0, 2, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(new QLabel(tr("Fixed")), 1, 0);
    gridLayout->addWidget(comboBoxFixedSymbol,     1, 1);
    gridLayout->addWidget(comboBoxFixedFill,       1, 2);
    gridLayout->addWidget(new QLabel(tr("Constrained")), 2, 0);
    gridLayout->addWidget(comboBoxConstrainedSymbol,     2, 1);
    gridLayout->addWidget(comboBoxConstrainedFill,       2, 2);
    gridLayout->addWidget(new QLabel(tr("Free")), 3, 0);
    gridLayout->addWidget(comboBoxFreeSymbol,     3, 1);
    gridLayout->addWidget(comboBoxFreeFill,       3, 2);
    groupBox_Point_Symbols->setLayout(gridLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox;

    QPushButton* pBreset = new QPushButton(tr("Reset"));
    buttonBox->addButton(pBreset, QDialogButtonBox::ResetRole);
    QPushButton* pBrestore = new QPushButton(tr("Restore Defaults"));
    buttonBox->addButton(pBrestore, QDialogButtonBox::ResetRole);
    QPushButton* pBclose = new QPushButton(tr("Close"));
    buttonBox->addButton(pBclose, QDialogButtonBox::RejectRole);
    QPushButton* pBapply = new QPushButton(tr("Apply"));
    buttonBox->addButton(pBapply, QDialogButtonBox::ApplyRole);

    QVBoxLayout* vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addWidget(groupBox_Point_Symbols);
    vBoxLayout->addWidget(buttonBox);
    setLayout(vBoxLayout);

    auto clicked = [this](QAbstractButton* button){on_buttonBox_clicked(button);};
    connect(buttonBox, &QDialogButtonBox::clicked, clicked);
}

DrawSettings::~DrawSettings()
{
}

void DrawSettings::setSpinBoxDefaults(QDoubleSpinBox *spinBox)
{
    const double minv = svg->minimalSize();
    const double maxv = 1000 - minv;

    spinBox->setDecimals(4);
    spinBox->setMaximum(maxv);
    spinBox->setMinimum(minv);
    spinBox->setSingleStep(0.1);
    spinBox->adjustSize();

    spinBox->setValue(minv);
}

void DrawSettings::readSvgValues()
{
    checkBoxDrawPointSymbols->setChecked(svg->drawPointSymbols());
    checkBoxDrawPointIDs    ->setChecked(svg->drawPointIDs());
    checkBoxDrawEllipses    ->setChecked(svg->drawEllipses());
    checkBoxDrawObservations->setChecked(svg->drawObservations());
    checkBoxDrawAxes        ->setChecked(svg->drawAxes());

    doubleSpinBoxFontSize     ->setValue(svg->fontSize());
    doubleSpinBoxSymbolSize   ->setValue(svg->symbolSize());
    doubleSpinBoxStrokeWidth  ->setValue(svg->strokeWidth());
    doubleSpinBoxEllipsesScale->setValue(svg->ellipsesScale());

    comboBoxFixedSymbol      ->setCurrentIndex(pointsymbols.indexOf(svg->fixedSymbol().c_str()));
    comboBoxConstrainedSymbol->setCurrentIndex(pointsymbols.indexOf(svg->constrainedSymbol().c_str()));
    comboBoxFreeSymbol       ->setCurrentIndex(pointsymbols.indexOf(svg->freeSymbol().c_str()));

    comboBoxFixedFill      ->setCurrentIndex(pointfill.indexOf(svg->fixedFill().c_str()));
    comboBoxConstrainedFill->setCurrentIndex(pointfill.indexOf(svg->constrainedFill().c_str()));
    comboBoxFreeFill       ->setCurrentIndex(pointfill.indexOf(svg->freeFill().c_str()));
}

void DrawSettings::on_buttonBox_accepted()
{
    svg->setDrawPointSymbols(checkBoxDrawPointSymbols->isChecked());
    svg->setDrawPointIDs    (checkBoxDrawPointIDs    ->isChecked());
    svg->setDrawEllipses    (checkBoxDrawEllipses    ->isChecked());
    svg->setDrawObservations(checkBoxDrawObservations->isChecked());
    svg->setDrawAxes        (checkBoxDrawAxes        ->isChecked());

    svg->setFontSize     (doubleSpinBoxFontSize   ->value());
    svg->setSymbolSize   (doubleSpinBoxSymbolSize ->value());
    svg->setStrokeWidth  (doubleSpinBoxStrokeWidth->value());
    svg->setEllipsesScale(doubleSpinBoxEllipsesScale->value());

    svg->setFixedSymbol      (comboBoxFixedSymbol      ->currentText().toStdString());
    svg->setConstrainedSymbol(comboBoxConstrainedSymbol->currentText().toStdString());
    svg->setFreeSymbol       (comboBoxFreeSymbol       ->currentText().toStdString());

    svg->setFixedFill      (comboBoxFixedFill      ->currentText().toStdString());
    svg->setConstrainedFill(comboBoxConstrainedFill->currentText().toStdString());
    svg->setFreeFill       (comboBoxFreeFill       ->currentText().toStdString());

    emit redraw();
}

void DrawSettings::on_buttonBox_clicked(QAbstractButton *button)
{
    QString text = button->text();
    if (text == tr("Apply"))
    {
        on_buttonBox_accepted();
    }
    else if (text == tr("Close"))
    {
        close();
    }
    else if (text == tr("Restore Defaults"))
    {
        svg->restoreDefaults();
        readSvgValues();
    }
    else if (text == tr("Reset"))
    {
        svg->setDrawPointSymbols(r_drawPointSymbols);
        svg->setDrawPointIDs(r_drawPointIDs);
        svg->setDrawEllipses(r_drawEllipses);
        svg->setDrawObservations(r_drawObservations);
        svg->setDrawAxes(r_drawAxes);

        svg->setFontSize(r_fontSize);
        svg->setSymbolSize(r_symbolSize);
        svg->setStrokeWidth(r_strokeWidth);
        svg->setEllipsesScale(r_ellipsesScale);

        svg->setFixedSymbol(r_fixedSymbol);
        svg->setConstrainedSymbol(r_constrainedSymbol);
        svg->setFreeSymbol(r_freeSymbol);

        svg->setFixedFill(r_fixedFill);
        svg->setConstrainedFill(r_constrainedFill);
        svg->setFreeFill(r_freeFill);

        readSvgValues();
    }
    else
    {
    }

    emit redraw();
}
