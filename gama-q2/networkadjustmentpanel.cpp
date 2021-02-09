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

#include "networkadjustmentpanel.h"
#include "selectconfiguration.h"
#include "constants.h"
#include "xml2txt.h"
#include "networksvg.h"
#include "drawsettings.h"
#include "parametereditor.h"
#include "pointeditor.h"
#include "observationeditor.h"
#include "adjustmentresults.h"
#include "gamaq2help.h"
#include "gamaq2interfaces.h"

#include <sstream>
#include <fstream>

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSvgRenderer>
#include <QDockWidget>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>

#include <gnu_gama/local/network.h>
#include <gnu_gama/local/localnetwork2sql.h>
#include <gnu_gama/xml/localnetworkoctave.h>

// QSettings export adjustment directory
const QString export_adjdir {"export/adjdir"};
const QString export_sqldir {"export/sqldir"};
const QString export_imgdir {"export/imgdir"};

bool        NetworkAdjustmentPanel::closeAllNetworkAdjustmentPanels = false;
QWidgetList NetworkAdjustmentPanel::allNetworkAdjustmentPanelsList;

namespace
{
    // Solution after software.rtcm-ntrip.org by Leos Mervart (C) 2013
    template<typename Interface>
    class PluginAction : public QAction {
    public:
        PluginAction(QObject* parent, Interface* intface)
            : QAction(intface->getName(), parent), interface_(intface)
        {
        }

        Interface*  interface()  { return interface_; }

    private:
        Interface* interface_;
    };

}


NetworkAdjustmentPanel::NetworkAdjustmentPanel(QString connectionName, QWidget *parent) :
    QMainWindow(parent),
    par(nullptr), ped(nullptr), obs(nullptr), svg(nullptr), res(nullptr), connection_name(connectionName)
{
    allNetworkAdjustmentPanelsList.push_back(this);

    QAction* action {nullptr};

    // File menu

    menuFile  = new QMenu(tr("&File"), this);
    action = menuFile->addAction(tr("&Save"));
    connect(action, &QAction::triggered, this, [this](){action_Save_into_db();});
    action = menuFile->addAction(tr("Save &As"));
    connect(action, &QAction::triggered, this, [this](){action_Save_As_into_db();});
    action = menuFile->addAction(tr("&Export as SQL file"));
    connect(action, &QAction::triggered, this, [this](){action_Save_as_SQL_file();});
    action = menuFile->addAction(tr("&Print"));
    connect(action, &QAction::triggered, this, [this](){action_Print();});

    // ... Export submenu

    menuAdjExport = menuFile->addMenu(tr("Export Adjustment Results As"));
    //menuAdjExport->setEnabled(false);
    action = menuAdjExport->addAction(tr("Gama-local &XML adjustment format"));
    connect(action, &QAction::triggered, this, [this](){action_Export_XML_adjustment_results();});
    action = menuAdjExport->addAction(tr("&HTML file"));
    connect(action, &QAction::triggered, this, [this](){action_Export_adjustment_results_as_HTML();});
    action = menuAdjExport->addAction(tr("Plain &text"));
    connect(action, &QAction::triggered, this, [this](){action_Export_adjustment_results_as_plain_text();});
    action = menuAdjExport->addAction(tr("Octave &matrix file"));
    connect(action, &QAction::triggered, this, [this](){action_Export_adjustment_results_as_octave_file();});

    action = menuFile->addAction(tr("Save network configuration &outline"));
    connect(action, &QAction::triggered, this, [this](){action_Save_network_configuration_outline();});
    menuFile->addSeparator();
    action = menuFile->addAction(tr("&Close"));
    connect(action, &QAction::triggered, this, [this](){action_Close();});

    // Adjustment menu

    menuAdjustment = new QMenu(tr("&Adjustment"), this);
    action = menuAdjustment->addAction(tr("&Run"));
    connect(action, &QAction::triggered, this, [this](){action_Run();});

    // View menu

    menuView  = new QMenu(tr("&View"), this);

    // Edit menu

    menuEdit  = new QMenu(tr("&Edit"), this);
    actionParameters = menuEdit->addAction(tr("Pa&rameters"));
    actionParameters->setCheckable(true);
    connect(actionParameters, &QAction::toggled, this, [this](){action_Parameters_changed();});
    actionPoints = menuEdit->addAction(tr("&Points"));
    actionPoints->setCheckable(true);
    connect(actionPoints, &QAction::toggled, this, [this](){action_Points_changed();});
    actionObservations = menuEdit->addAction(tr("&Observations"));
    actionObservations->setCheckable(true);
    connect(actionObservations, &QAction::toggled, this, [this](){action_Observations_changed();});

    // Settings menu

    menuSetup = new QMenu(tr("&Settings"), this);
    action = menuSetup->addAction(tr("Outline &draw"));
    connect(action, &QAction::triggered, this, [this](){action_Outline_draw();});

    menuTools = new QMenu(tr("&Tools"), this);
    {
        QMap<QString, AdjustmentInterface*> map;

        QDir gamaq2plugins(qApp->applicationDirPath());
        gamaq2plugins.cd("plugins");
        for (QString& fileName : gamaq2plugins.entryList(QDir::Files)) {
            QPluginLoader pluginLoader(gamaq2plugins.absoluteFilePath(fileName));
            QObject *plugin = pluginLoader.instance();
            if (plugin) {
                if (AdjustmentInterface* adjinterface = qobject_cast<AdjustmentInterface*>(plugin))
                {
                    map[adjinterface->getName() + fileName] = adjinterface;
                }
            }
        }

        if (!map.empty())
        {
            menuTools->setEnabled(true);
            for (auto i : qAsConst(map))
            {
                PluginAction<AdjustmentInterface>* action = new PluginAction<AdjustmentInterface>(this, i);
                menuTools->addAction(action);
                connect(action, SIGNAL(triggered()), SLOT(AdjustmentPluginSlot()));
            }
        }
    }

    // Help menu

    menuHelp  = new QMenu(tr("&Help"), this);
    action = menuHelp->addAction(tr("Gama-q2 &Help"));
    connect(action, &QAction::triggered, this, [this](){action_Gama_q2_help();});
    /*
    menuBar()->addMenu(menuFile);
    menuBar()->addMenu(menuAdjustment);
    menuBar()->addMenu(menuView);
    menuBar()->addMenu(menuEdit);
    menuBar()->addMenu(menuSetup);
    menuBar()->addMenu(menuTools);
    menuBar()->addMenu(menuHelp);
    */
    status_bar("");
    set_gui_adjustment_functions_status(false);
}

NetworkAdjustmentPanel::~NetworkAdjustmentPanel()
{
    for (auto w : qAsConst(localPluginsList)) delete w;
    allNetworkAdjustmentPanelsList.removeOne(this);
}

void NetworkAdjustmentPanel::action_Close()
{
    close();
}

void NetworkAdjustmentPanel::draw_network_configuration()
{
    std::string s = adj.svg();
    QByteArray  b(s.c_str(), s.length());
    svg->getSVG(b);
}

void NetworkAdjustmentPanel::update_point_editor_table()
{
    ped->connectPointData      (&adj.get_local_network()->PD);
}

void NetworkAdjustmentPanel::update_observation_editor_table()
{
    obs->connectObservationData(adj.get_local_network());
}

void NetworkAdjustmentPanel::update_adjustment_results(bool solved)
{
    if (!solved) return;

    if (!res->has_local_network())
    {
        res->set_local_network(adj.get_local_network());
    }
    res->update_adjustment_results();
}

void NetworkAdjustmentPanel::exec()
{
    SelectConfiguration* sc = new SelectConfiguration(connection_name, true, true, this);
    connect(sc, SIGNAL(selectConfiguration(QString,bool)), this, SLOT(getConfigurationName(QString,bool)));
    sc->select();
}

void NetworkAdjustmentPanel::tabIndexChanged(int n)
{
    if (n==3) draw_network_configuration();
}

void NetworkAdjustmentPanel::getConfigurationName(QString conf, bool tabbed)
{
    if (conf.isEmpty())
    {
        close();
        return;
    }

    useTabbedWidgets   = tabbed;
    if (tabbed) {
        menuBar()->addMenu(menuFile);
        menuBar()->addMenu(menuAdjustment);
        //menuBar()->addMenu(menuView);
        menuBar()->addMenu(menuEdit);
        menuBar()->addMenu(menuSetup);
        if (!menuTools->isEmpty()) menuBar()->addMenu(menuTools);
        menuBar()->addMenu(menuHelp);
    }
    else {
        menuBar()->addMenu(menuFile);
        menuBar()->addMenu(menuAdjustment);
        menuBar()->addMenu(menuView);
        menuBar()->addMenu(menuEdit);
        menuBar()->addMenu(menuSetup);
        if (!menuTools->isEmpty()) menuBar()->addMenu(menuTools);
        menuBar()->addMenu(menuHelp);
    }

    configuration_name = conf;
    setWindowTitle(tr("Network Adjustment '%1'").arg(conf));

    QSqlDatabase db = QSqlDatabase::database(GamaQ2::connection_implicit_db);
    QSqlQuery query(db);

    try
    {
        query.exec("select conf_name FROM gnu_gama_local_configurations "
                   " where conf_name = '" + conf + "'");

        if (!query.next()) {
            db.transaction();
            query.exec(" insert into gnu_gama_local_configurations (conf_id, conf_name) "
                       " select new_id, '" + conf + "' from (select coalesce(max(conf_id), 0)+1 as new_id from gnu_gama_local_configurations)x");
            db.commit();
        }

        adj.read_configuration(query, configuration_name);
    }
    catch (...)
    {
        QString err = tr("Critical error on reading configuration %1");
        QMessageBox::critical(this,
                              err.arg(conf),
                              tr("Database schema is probably corrupted"));
        destroy();
        return;
    }

    par = new ParameterEditor(this);
    ped = new PointEditor(this);
    obs = new ObservationEditor(this);
    svg = new NetworkSVG(this);
    res = new AdjustmentResults(this);

    // parameters and description are not dependent on emitted signals
    par->connectParameters (adj.get_local_network());

    if (useTabbedWidgets)
    {
        QTabWidget* tab = new QTabWidget;

        tab->addTab(par, tr("Parameters"));
        tab->addTab(ped, tr("Points"));
        tab->addTab(obs, tr("Observations"));
        tab->addTab(svg, tr("Configuration"));
        tab->addTab(res, tr("Results"));

        setCentralWidget(tab);

        connect(tab, SIGNAL(currentChanged(int)),this,SLOT(tabIndexChanged(int)));
    }
    else
    {
        // addDockWidget for the first widget ...
        QDockWidget* dockpar = new QDockWidget(tr("Parameters"), this);
        dockpar->setWidget(par);
        addDockWidget(Qt::LeftDockWidgetArea, dockpar);
        menuView->addAction(dockpar->toggleViewAction());

        // ... others are included using tabifyDockWidget for the first one
        QDockWidget* dockped = new QDockWidget(tr("Points"), this);
        dockped->setWidget(ped);
        tabifyDockWidget(dockpar, dockped);
        menuView->addAction(dockped->toggleViewAction());

        QDockWidget* dockoed = new QDockWidget(tr("Observations"), this);
        dockoed->setWidget(obs);
        tabifyDockWidget(dockpar, dockoed);
        menuView->addAction(dockoed->toggleViewAction());

        QDockWidget* docksvg = new QDockWidget(tr("Configuration"), this);
        docksvg->setWidget(svg);
        tabifyDockWidget(dockpar, docksvg);
        menuView->addAction(docksvg->toggleViewAction());

        QDockWidget* dockres = new QDockWidget(tr("Results"), this);
        dockres->setWidget(res);
        tabifyDockWidget(dockpar, dockres);
        menuView->addAction(dockres->toggleViewAction());

        connect(docksvg, SIGNAL(visibilityChanged(bool)),this,SLOT(draw_network_configuration()));
    }

    connect(par,  SIGNAL(angular_units_changed()), this, SLOT(update_observation_editor_table()));
    connect(par,  SIGNAL(angular_units_changed()), res,  SLOT(update_adjustment_results()));

    connect(ped,  SIGNAL(warning(QString)), this,  SLOT(status_bar(QString)));
    connect(obs,  SIGNAL(warning(QString)), this,  SLOT(status_bar(QString)));

    connect(&adj, SIGNAL(acord_signal()),          this, SLOT(update_point_editor_table()));
    connect(&adj, SIGNAL(adjustment_signal(bool)), this, SLOT(draw_network_configuration()));
    connect(&adj, SIGNAL(adjustment_signal(bool)), this, SLOT(update_adjustment_results (bool)));
    connect(&adj, SIGNAL(adjustment_signal(bool)), this, SLOT(update_point_editor_table()));
    connect(&adj, SIGNAL(adjustment_signal(bool)), this, SLOT(update_observation_editor_table()));

    update_point_editor_table();
    update_observation_editor_table();

    emit networkAdjustmentPanel(true);
    show();
}

void NetworkAdjustmentPanel::closeEvent(QCloseEvent* event)
{
    if (configuration_name.isEmpty())  return;

    if (closeAllNetworkAdjustmentPanels)
    {
        event->accept();
        return;
    }

    QMessageBox confirm;
    confirm.setWindowTitle(configuration_name);
    confirm.setText(tr("Do you really want to exit the adjustment panel?"));
    confirm.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    if (allNetworkAdjustmentPanelsList.size() > 1) confirm.addButton(QMessageBox::YesAll);
    confirm.setDefaultButton  (QMessageBox::Cancel);
    int dialogCode  = confirm.exec();

    if (dialogCode == QMessageBox::Yes)
    {
        emit networkAdjustmentPanel(false);
        event->accept();
    }
    else if (dialogCode == QMessageBox::YesAll)
    {
        NetworkAdjustmentPanel::closeAllNetworkAdjustmentPanels = true;
        for (int i=0; i<NetworkAdjustmentPanel::allNetworkAdjustmentPanelsList.size(); i++)
        {
            NetworkAdjustmentPanel::allNetworkAdjustmentPanelsList[i]->close();
            emit networkAdjustmentPanel(false);
        }
        event->accept();
        NetworkAdjustmentPanel::closeAllNetworkAdjustmentPanels = false;
    }
    else
    {
        event->ignore();
    }
}

void NetworkAdjustmentPanel::action_Export_XML_adjustment_results()
{
    QSettings settings;
    QString adjdir = settings.value(export_adjdir).toString();
    QFileDialog fileDialog(nullptr,tr("Open XML Output File"));
    if (!adjdir.isEmpty()) fileDialog.setDirectory(adjdir);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDefaultSuffix("xml");
    fileDialog.setNameFilter(tr("Gama XML adjustment results file (*.xml)"));
    fileDialog.setViewMode(QFileDialog::Detail);

    if (!fileDialog.exec()) return;

    settings.setValue(export_adjdir, fileDialog.directory().absolutePath());
    const QStringList& flist = fileDialog.selectedFiles();
    if (!flist.empty())
      {
        QFile file(flist[0]);
        file.open(QIODevice::WriteOnly);
        QTextStream stream(&file);

        stream << adj.xml();
      }
}

void NetworkAdjustmentPanel::action_Export_adjustment_results_as_plain_text()
{
    QSettings settings;
    QString adjdir = settings.value(export_adjdir).toString();
    QFileDialog fileDialog(nullptr,tr("Open Adjustment Output File"));
    if (!adjdir.isEmpty()) fileDialog.setDirectory(adjdir);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDefaultSuffix("txt");
    fileDialog.setNameFilter(tr("Gama XML adjustment results file (*.txt)"));
    fileDialog.setViewMode(QFileDialog::Detail);

    if (!fileDialog.exec()) return;
    settings.setValue(export_adjdir, fileDialog.directory().absolutePath());

    GNU_gama::LocalNetworkAdjustmentResults adjres;
    {
        std::string data = adj.xml().toStdString();
        std::istringstream xml(data);
        adjres.read_xml(xml);

        QSqlDatabase db = QSqlDatabase::database(GamaQ2::connection_implicit_db);
        QSqlQuery query(db);
        query.exec(QString("SELECT * "
                           "FROM gnu_gama_local_configurations "
                           "WHERE conf_name='%1'").arg(configuration_name));
        while (query.next())
        {
            QSqlRecord rec = query.record();

            adjres.gons = (rec.value("ang_units").toInt() == 400);
        }
    }

    const QStringList& selected = fileDialog.selectedFiles();
    if (!selected.empty())
      {
        std::ofstream cout(selected[0].toStdString().c_str());

        GNU_gama::xml2txt_general_parameters   (cout, adjres);
        GNU_gama::xml2txt_adjusted_parameters  (cout, adjres);
        GNU_gama::xml2txt_adjusted_observations(cout, adjres);
      }
}

void NetworkAdjustmentPanel::action_Export_adjustment_results_as_octave_file()
{
    QSettings settings;
    QString adjdir = settings.value(export_adjdir).toString();
    QFileDialog fileDialog(nullptr,tr("Open Octave file"));
    if (!adjdir.isEmpty()) fileDialog.setDirectory(adjdir);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDefaultSuffix("m");
    fileDialog.setNameFilter(tr("Octave file (*.m)"));
    fileDialog.setViewMode(QFileDialog::Detail);

    if (!fileDialog.exec()) return;
    settings.setValue(export_adjdir, fileDialog.directory().absolutePath());

    GNU_gama::LocalNetworkOctave octave(adj.get_local_network());

    const auto& selected = fileDialog.selectedFiles();
    std::ofstream out(selected[0].toStdString().c_str());
    octave.write(out);
}

void NetworkAdjustmentPanel::action_Save_as_SQL_file()
{
    QSettings settings;
    QString sqldir = settings.value(export_sqldir).toString();
    QFileDialog fileDialog(nullptr,tr("Export current configuration as a SQL file"));
    if (!sqldir.isEmpty()) fileDialog.setDirectory(sqldir);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);   // a single file only
    fileDialog.setDefaultSuffix("sql");
    QStringList filters;
    filters << "Sqlite batch files (*.sql)"
            << "All files (*.*)";
    fileDialog.setNameFilters(filters);
    fileDialog.setViewMode(QFileDialog::Detail);

    if (!fileDialog.exec()) return;
    settings.setValue(export_sqldir, fileDialog.directory().absolutePath());

    GNU_gama::local::LocalNetwork2sql ln2sql(*adj.get_local_network());
    std::ostringstream sqlbatch;
    ln2sql.write(sqlbatch, configuration_name.toUtf8().data());

    const QStringList& selected = fileDialog.selectedFiles();
    const QString& fileName = selected[0];
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << sqlbatch.str().c_str();

    status_bar(tr("SQL batch file was written: %1").arg(fileName));
}

void NetworkAdjustmentPanel::action_Run()
{
    status_bar(tr("Runnig adjustement '%1'").arg(configuration_name));
    try {
        adj.exec();
    }
    catch (QGama::Exception e)
    {
        QMessageBox::critical(this, e.title(), e.text());
    }
    catch (...)
    {
        QMessageBox::critical(this, tr("Adjustment"), tr("unnkown exception"));
    }
    set_gui_adjustment_functions_status(true);
    status_bar(tr("Network '%1' adjusted").arg(configuration_name));
}

void NetworkAdjustmentPanel::action_Save_network_configuration_outline()
{
    // Qt supported suffixes
    //  << "svg" << "bmp" <<  "jpg" << "jpeg" << "png"
    //           << "ppm" << "tiff" << "xbm"  << "xpm"
    QStringList suffixes;
    suffixes << "svg" << "png" << "jpg" << "tiff";

    bool filters_start = true;
    QString filters;
    for (QStringList::iterator i=suffixes.begin(), e=suffixes.end(); i!=e; ++i)
    {
        if (!filters_start)
        {
            filters += " ";
        }
        filters_start = false;

        filters += "*." + *i;
    }

    QSettings settings;
    QString imgdir = settings.value(export_imgdir).toString();
    QFileDialog fileDialog(nullptr,tr("Save network outline"));
    if (!imgdir.isEmpty()) fileDialog.setDirectory(imgdir);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDefaultSuffix("svg");
    fileDialog.setNameFilter(tr("Gama network configuration outline (")
                             + filters + ")");
    fileDialog.setViewMode(QFileDialog::Detail);
    settings.setValue(export_imgdir, fileDialog.directory().absolutePath());

    QString fileName;
    QString suffix;
    bool supported_suffix = false;
    do {
        if (!fileDialog.exec()) return;

        const auto& selected = fileDialog.selectedFiles();
        fileName = selected[0];
        QFileInfo info(fileName);
        suffix = info.suffix().toLower();
        supported_suffix = suffixes.contains(suffix, Qt::CaseInsensitive);

        if (!supported_suffix)
        {
            QMessageBox::warning(this,
                    tr("Unknown suffix .%1").arg(info.suffix()),
                    tr("Image format is unknown or not supported"));
        }

    } while (!supported_suffix);

    std::string s = adj.svg();
    if (suffix == "svg")
    {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QTextStream stream(&file);
        stream <<  QString::fromUtf8(s.c_str());
    }
    else
    {
        QByteArray  b(s.c_str(), s.length());
        QSvgRenderer svg;
        svg.load(b);
        QImage  image(svg.defaultSize(), QImage::Format_ARGB32);
        //image.fill(Qt::white);
        //image.loadFromData(b, "svg");
        QPainter painter(&image);
        painter.setBackground(Qt::white);
        painter.setBackgroundMode(Qt::TransparentMode);
        svg.render(&painter);
        image.save(fileName);
    }
}

void NetworkAdjustmentPanel::action_Outline_draw()
{
    DrawSettings *draw = new DrawSettings(adj.get_svg());

    connect(this, SIGNAL(destroyed()), draw, SLOT(close()));
    connect(draw, SIGNAL(redraw()), this, SLOT(draw_network_configuration()));

    draw->setAttribute(Qt::WA_DeleteOnClose);
    draw->setWindowTitle(configuration_name);
    draw->show();
}

void NetworkAdjustmentPanel::status_bar(QString text)
{
    statusBar()->showMessage(text, 6000);
}

void NetworkAdjustmentPanel::action_Parameters_changed()
{
    bool edit = actionParameters->isChecked();
    QString text;
    if (edit)
    {
        text = tr("Editing of Parameters is enabled");
    }
    else
    {
        text = tr("Editing of Parameters is disabled");
    }
    par->enableEdit(edit);

    status_bar(text);
}

void NetworkAdjustmentPanel::action_Points_changed()
{
    bool edit = actionPoints->isChecked();
    QString text;
    if (edit)
    {
        text = tr("Editing of Points is enabled");
    }
    else
    {
        text = tr("Editing of Points is disabled");
    }
    ped->enableEdit(edit);

    status_bar(text);
}

void NetworkAdjustmentPanel::action_Observations_changed()
{
    bool edit = actionObservations->isChecked();
    QString text;
    if (edit)
    {
        text = tr("Editing of Observations is enabled");
    }
    else
    {
        text = tr("Editing of Observations is disabled");
    }
    obs->enableEdit(edit);

    status_bar(text);
}

void NetworkAdjustmentPanel::action_Save_into_db()
{
    save_configuration(configuration_name);
}

void NetworkAdjustmentPanel::action_Save_As_into_db()
{
    SelectConfiguration* sc = new SelectConfiguration(connection_name, false, true, this);
    connect(sc, SIGNAL(selectConfiguration(QString)), SLOT(save_configuration(QString)));
    sc->select();
}

void NetworkAdjustmentPanel::save_configuration(QString confname)
{
    GNU_gama::local::LocalNetwork2sql ln2sql(*adj.get_local_network());
    std::ostringstream sqlbatch;
    ln2sql.write(sqlbatch, confname.toUtf8().data());
    QStringList sql = QString::fromUtf8(sqlbatch.str().c_str())
            .split(QRegularExpression(";[\\s+]"),Qt::SkipEmptyParts);

    QSqlDatabase db = QSqlDatabase::database(GamaQ2::connection_implicit_db);
    db.transaction();
    QSqlQuery q(db);

    for (int i=1; i<sql.size()-1; i++)
    {
        /* regexps are replaced by ignoring the first and the last item */
        //if (sql[i].contains(QRegExp("\nbegin$" ,Qt::CaseInsensitive))) continue;
        //if (sql[i].contains(QRegExp("\ncommit$",Qt::CaseInsensitive))) continue;

        q.exec(sql[i]);
    }

    if (q.lastError().isValid())
    {
        QMessageBox::critical(this, tr("Database error"), q.lastError().databaseText());
        db.rollback();
    }
    else
    {
        status_bar(tr("Configuration '%1' saved in the database").arg(confname));
        db.commit();
    }
}

void NetworkAdjustmentPanel::action_Print()
{
    QPrintDialog dialog;
    dialog.setWindowTitle(tr("Print Adjustment Results ") + configuration_name);
    if (dialog.exec() != QDialog::Accepted) return;

    QPrinter printer;
    res->textEdit->print(&printer);
}

void NetworkAdjustmentPanel::action_Export_adjustment_results_as_HTML()
{
    QSettings settings;
    QString adjdir = settings.value(export_adjdir).toString();
    QFileDialog fileDialog(nullptr,tr("Save adjustment results as HTML file"));
    if (!adjdir.isEmpty()) fileDialog.setDirectory(adjdir);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);   // a single file only
    fileDialog.setDefaultSuffix("html");
    QStringList filters;
    filters << "HTML files (*.html *.htm *.xhtml)"
            << "All files (*.*)";
    fileDialog.setNameFilters(filters);
    fileDialog.setViewMode(QFileDialog::Detail);

    if (!fileDialog.exec()) return;
    settings.setValue(export_adjdir, fileDialog.directory().absolutePath());

    const auto& selected = fileDialog.selectedFiles();
    QFile file(selected[0]);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << res->getHtmlUtf8();

    status_bar(tr("HTML adjustment results written"));
}

void NetworkAdjustmentPanel::set_gui_adjustment_functions_status(bool isvalid)
{
    valid = isvalid;
    /*
    ui->actionPrint->setEnabled(valid);
    ui->actionPrint_as_PDF->setEnabled(valid);
    ui->actionSave_adjustment_results_as_HTML_file->setEnabled(valid);
    ui->actionSave_adjustment_results_as_plain_text->setEnabled(valid);
    ui->actionSave_network_configuration_outline->setEnabled(valid);
    ui->actionSave_XML_adjustment_results->setEnabled(valid);
    */
}

void NetworkAdjustmentPanel::action_Gama_q2_help()
{
    GamaQ2help::get()->show();
}

void NetworkAdjustmentPanel::AdjustmentPluginSlot()
{
    if (PluginAction<AdjustmentInterface>* plugin_action = dynamic_cast<PluginAction<AdjustmentInterface>*>(sender()))
    {
        if (QWidget* widget = plugin_action->interface()->create(this))
        {
            localPluginsList.append(widget);
            widget->show();
        }
    }
}
