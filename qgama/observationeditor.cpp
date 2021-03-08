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

#include "observationeditor.h"
#include "insertclusterdialog.h"
#include "insertobservationdialog.h"
#include "lineeditdelegate.h"
#include "constants.h"
#include "tableviewstyle.h"
#include <gnu_gama/local/gamadata.h>
#include <QMenu>
#include <QMessageBox>
#include <QTableView>
#include <QHeaderView>
#include <QGridLayout>
#include <typeinfo>

#include <gnu_gama/local/network.h>

ObservationEditor::SelectCluster::SelectCluster(ObservationTableModel *model,
                                                QTableView* tableview, int logicalIndex)
    : tableView(tableview), isValid_(true)
{
    int minIndex, maxIndex;
    if (!model->clusterIndexes(logicalIndex, minIndex, maxIndex))
    {
        isValid_ = false;
        return;
    }

    tableView->clearSelection();
    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    for (int index=maxIndex; index>=minIndex; index--)
    {
        tableView->selectRow(index);
    }
}

ObservationEditor::SelectCluster::~SelectCluster()
{
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->clearSelection();
}

bool ObservationEditor::SelectCluster::isValid() const
{
    return isValid_;
}

ObservationEditor::SelectGroup::SelectGroup(ObservationTableModel *model,
                                                QTableView* tableview, int logicalIndex)
    : tableView(tableview), isValid_(true)
{
    int minIndex, maxIndex;
    if (!model->groupIndexes(logicalIndex, minIndex, maxIndex))
    {
        isValid_ = false;
        return;
    }

    tableView->clearSelection();
    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    for (int index=maxIndex; index>=minIndex; index--)
    {
        tableView->selectRow(index);
    }
}

ObservationEditor::SelectGroup::~SelectGroup()
{
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->clearSelection();
}

bool ObservationEditor::SelectGroup::isValid() const
{
    return isValid_;
}

ObservationEditor::ObservationEditor(QWidget *parent) :
    QWidget(parent),
    tableView(new QTableView),
    model(nullptr), readonly(true)
{
    LineEditDelegate* item = new LineEditDelegate(tableView);
    tableView->setItemDelegate(item);
    tableView->setStyleSheet(QGama::delegate_style_sheet);

    tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    observationMenu = new QMenu(this);

    QAction* menuInsertObservation = new QAction(tr("Insert observation"), this);
    observationMenu->addAction(menuInsertObservation);
    connect(menuInsertObservation, SIGNAL(triggered()), this, SLOT(insertObservation()));

    QAction* menuDeleteObservation = new QAction(tr("Delete observation"), this);
    observationMenu->addAction(menuDeleteObservation);
    connect(menuDeleteObservation, SIGNAL(triggered()), this, SLOT(deleteObservation()));

    observationMenu->addSeparator();

    QAction* menuInsertCluster = new QAction(tr("Insert cluster"), this);
    observationMenu->addAction(menuInsertCluster);
    connect(menuInsertCluster, SIGNAL(triggered()), this, SLOT(insertCluster()));

    QAction* menuDeleteCluster = new QAction(tr("Delete cluster"), this);
    observationMenu->addAction(menuDeleteCluster);
    connect(menuDeleteCluster, SIGNAL(triggered()), this, SLOT(deleteCluster()));

    connect(tableView->verticalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(observationContextMenu(QPoint)));

    enableEdit(false);

    tableView->setStyleSheet(tableViewStyle());

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(tableView);
    setLayout(layout);
}

ObservationEditor::~ObservationEditor()
{
}

void ObservationEditor::connectObservationData(GNU_gama::local::LocalNetwork *lnet)
{
    ObservationTableModel* old = model;
    model = new ObservationTableModel(lnet, this);
    tableView->setModel(model);
    delete old;

    connect(model, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
}

void ObservationEditor::enableEdit(bool edit)
{
    readonly = !edit;
    if (edit)
    {
        // implicit behaviour is "double click for editing"
        tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
    else
    {
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void ObservationEditor::observationContextMenu(QPoint p)
{
    if (readonly) return;

    observationLogicalIndex = tableView->verticalHeader()->logicalIndexAt(p);
    QModelIndex index = model->index(observationLogicalIndex, 0);
    if (!index.isValid()) return;

    observationMenu->exec(QCursor::pos());
}

void ObservationEditor::deleteCluster()
{
    QModelIndex index = model->index(observationLogicalIndex, 0);
    if (!index.isValid()) return;

    SelectCluster selectCluster(model, tableView, observationLogicalIndex);
    if (!selectCluster.isValid()) return;

    int q = QMessageBox::warning(this, tr("Delete Cluster"),
             tr("Do you want to delete selected cluster?"),
             QMessageBox::Ok|QMessageBox::Cancel);

    if (q != QMessageBox::Ok) return;

    model->deleteCluster(observationLogicalIndex);
}

void ObservationEditor::insertCluster()
{
    QModelIndex index = model->index(observationLogicalIndex, 0);
    if (!index.isValid()) return;

    SelectCluster selectCluster(model, tableView, observationLogicalIndex);

    InsertClusterDialog dialog;
    if (dialog.exec() == QDialog::Rejected) return;

    model->insertCluster(observationLogicalIndex, dialog.position(), dialog.clusterName());
}


void ObservationEditor::deleteObservation()
{
    QModelIndex index = model->index(observationLogicalIndex, 0);
    if (!index.isValid()) return;

    if (!model->isObservationRow(observationLogicalIndex)) return;

    //tableView->clearSelection();
    //tableView->selectRow(observationLogicalIndex);
    SelectGroup selectGroup(model, tableView, observationLogicalIndex);

    int q = QMessageBox::warning(this, tr("Delete Observation"),
             tr("Do you want to delete selected observation?"),
             QMessageBox::Ok|QMessageBox::Cancel);

    if (q != QMessageBox::Ok) return;

    model->deleteObservation(observationLogicalIndex);
}

void ObservationEditor::insertObservation()
{
    QModelIndex index = model->index(observationLogicalIndex, 0);
    if (!index.isValid()) return;

    //tableView->clearSelection();
    //tableView->selectRow(observationLogicalIndex);
    SelectGroup selectGroup(model, tableView, observationLogicalIndex);

    QString name = model->currentClusterName(observationLogicalIndex);
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Insert Observation Failed"),
                             tr("Observation cannot be inserted without a cluster."));
        return;
    }
    InsertObservationDialog dialog(name);
    if (dialog.exec() == QDialog::Rejected) return;

    model->insertObservation(observationLogicalIndex, dialog);
    tableView->clearSelection();
}
