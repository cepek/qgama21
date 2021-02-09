/*  Qt based GUI for GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2014, 2015, 2020  Ales Cepek <cepek@gnu.org>

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

#include "gamaq2controlpanel.h"

#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QMenuBar>
#include <QStatusBar>
#include <QFontDialog>
#include <QDebug>

#include "constants.h"
#include "dbconnectdialog.h"
#include "dbfunctions.h"
#include "importconfigurationfile.h"
#include "networkadjustmentpanel.h"
#include "gamaq2interfaces.h"
#include "selectadjresultslanguage.h"
#include "gamaq2help.h"

#include <gnu_gama/local/localnetwork2sql.h>
#include <gnu_gama/version.h>
#include <gnu_gama/local/language.h>
#include <sstream>

namespace
{
    GamaQ2ControlPanel* controlPanel;

    // Solution after software.rtcm-ntrip.org by Leos Mervart (C) 2013
    template<typename Interface>
    struct PluginAction : public QAction {
        Interface* interface_;

        PluginAction(QObject* parent, Interface* intface)
            : QAction(intface->getName(), parent), interface_(intface)
        {
        }
    };

}

void ShowMessage(QString message)
{
    controlPanel->statusBar()->showMessage(message);
}


GamaQ2ControlPanel::GamaQ2ControlPanel(QWidget *parent) :
    QMainWindow(parent),
    init_language_list(true)
{
    controlPanel = this;

    init_schema_lists();

    GamaQ2::name      = "gama-q2";
    GamaQ2::version   = "1.02.2";
    GamaQ2::copyright = "2021";

    // setting implicit adjustment results language
    set_adjustment_results_language();

    setWindowTitle(GamaQ2::name);
    load_plugins();
    build_menus();
    setMinimumSize(400, 150);
}

GamaQ2ControlPanel::~GamaQ2ControlPanel()
{
    // save options into database
    QSqlDatabase db = QSqlDatabase::database(GamaQ2::connection_implicit_db);
    if (db.isOpen())
    {
        QSqlQuery q(db);
        q.exec("DELETE FROM gnu_gama_local_options "
               "WHERE opt_key='AdjResultsLanguage'");
        q.exec("INSERT INTO gnu_gama_local_options (opt_key, opt_val) "
               "VALUES ('AdjResultsLanguage','" + GamaQ2::currentLanguage + "')");
    }
}

void GamaQ2ControlPanel::on_action_Exit_triggered()
{
    /* emit */ adjustmentPanel(false);
    close();
}

void GamaQ2ControlPanel::init_schema_lists()
{
    QStringList& schema = GamaQ2::gama_local_schema;
    QStringList& tables = GamaQ2::gama_local_schema_table_names;

    QFile rfile(":/lib/gama-local-schema.sql");
    rfile.open(QIODevice::ReadOnly);

    QTextStream sql(&rfile);

    // skip introductory comments
    while (!sql.atEnd())
    {
        QString line = sql.readLine();
        if (line.contains("*/")) break;
    }

    // read sql commands
    QStringList tmptab;
    while (!sql.atEnd())
    {
        QString command;
        while (!sql.atEnd())
        {
            QString line = sql.readLine();
            if (line.simplified().isEmpty()) continue;

            command += line;
            command += "\n";

            // extract table name
            if (line.contains("create", Qt::CaseInsensitive) &&
                line.contains("table",  Qt::CaseInsensitive) )
            {
                // create table "table name" (
                tmptab << line.split(QString(" "),
                    Qt::SkipEmptyParts, Qt::CaseInsensitive)[2];
            }

            if (line.contains(";")) break;  // end of the SQL command
        }

        if (!command.isEmpty()) schema << command;
    }

    // put all tables in the order needed for sequential dropping/deleting
    tables << "gnu_gama_local_covmat";
    tmptab.removeOne("gnu_gama_local_covmat");
    tmptab.removeOne("gnu_gama_local_clusters");
    tmptab.removeOne("gnu_gama_local_configurations");
    tables << tmptab;
    tables << "gnu_gama_local_clusters";
    tables << "gnu_gama_local_configurations";
}

void GamaQ2ControlPanel::build_menus()
{
    QMenu* menuDb = new QMenu(tr("&Database"), this);
    actionDbConnect = menuDb->addAction(tr("&Connect to Database"));
    connect(actionDbConnect, SIGNAL(triggered()), SLOT(on_action_Connect_to_database_triggered()));
    actionDbImport = menuDb->addAction(tr("&Import Configuration File"));
    connect(actionDbImport, SIGNAL(triggered()), SLOT(on_action_Import_configuration_file_triggered()));
    menuDb->addSeparator();
    actionDbDropSchema = menuDb->addAction(tr("Drop Schema &Tables"));
    connect(actionDbDropSchema, SIGNAL(triggered()), SLOT(on_action_Drop_schema_Tables_triggered()));
    actionDbDeleteData = menuDb->addAction(tr("&Delete all Data from the Schema"));
    connect(actionDbDeleteData, SIGNAL(triggered()), SLOT(on_action_Delete_all_Data_from_the_Schema_triggered()));
    actionDbDeleteConfiguration = menuDb->addAction(tr("Delete &Network Configuration"));
    connect(actionDbDeleteConfiguration, SIGNAL(triggered()), SLOT(on_action_Delete_Network_Configuration_triggered()));

    menuDb->addSeparator();
    actionImportExamples = menuDb->addAction(tr("Import Examples"));
    connect(actionImportExamples, &QAction::triggered, this, &GamaQ2ControlPanel::import_examples);

    if (!mapDbPlugins.empty())
    {
        menuDb->addSeparator();
        for (QMap<QString, DbInterface*>::iterator
             i=mapDbPlugins.begin(), e=mapDbPlugins.end(); i!=e; ++i)
        {
            PluginAction<DbInterface>* action = new PluginAction<DbInterface>(this, *i);
            menuDb->addAction(action);
            connect(action, SIGNAL(triggered()), SLOT(dbSlot()));
        }
    }
    menuDb->addSeparator();
    actionDbExit = menuDb->addAction(tr("&Exit"));
    connect(actionDbExit, SIGNAL(triggered()), SLOT(on_action_Exit_triggered()));

    QMenu* menuAdj = new QMenu(tr("&Adjustment"), this);
    actionAdjAdjustment = menuAdj->addAction(tr("&Network Adjustment"));
    actionAdjAdjustment->setDisabled(true);
    connect (actionAdjAdjustment, SIGNAL(triggered()), SLOT(on_action_Network_adjustment_triggered()));

    QMenu* menuTools = new QMenu(tr("&Tools"), this);
    actionAdjResultsLanguage = menuTools->addAction(tr("Adjustment results &language"));
    connect(actionAdjResultsLanguage, SIGNAL(triggered()), SLOT(on_action_Adjustment_results_language()));
    actionApplicationFont = menuTools->addAction(tr("Application Font"));
    connect(actionApplicationFont, SIGNAL(triggered()), SLOT(on_action_Application_Font()));

    QMenu* menuHelp = new QMenu(tr("&Help"), this);
    actionGamaQ2help = menuHelp->addAction(tr("Gama-q2 &help"));
    // the very first c++11 lambda function in gama-q2 (2015-07-29)
    // https://wiki.qt.io/New_Signal_Slot_Syntax
    connect(actionGamaQ2help, &QAction::triggered, [](){GamaQ2help::get()->show();});
    menuHelp->addSeparator();
    actionAboutGamaQ2 = menuHelp->addAction(tr("&About gama-q2"));
    connect(actionAboutGamaQ2, SIGNAL(triggered()), SLOT(on_action_About_gama_q2_triggered()));
    actionAboutQt = menuHelp->addAction(tr("About &Qt"));
    connect(actionAboutQt, SIGNAL(triggered()), SLOT(on_action_About_qt_triggered()));

    disable_input_data(false);

    menuBar()->addMenu(menuDb);
    menuBar()->addMenu(menuAdj);
    menuBar()->addMenu(menuTools);
    menuBar()->addMenu(menuHelp);
}

void GamaQ2ControlPanel::dbSlot()
{
    if (PluginAction<DbInterface>* plugin_action = dynamic_cast<PluginAction<DbInterface>*>(sender()))
    {
        if (QWidget* widget = plugin_action->interface_->create())
        {
            localPluginsList.append(widget);
            widget->show();
        }
    }
}

void GamaQ2ControlPanel::closeEvent(QCloseEvent * event)
{
    QMessageBox confirm;
    confirm.setText(tr("Do you really want to exit the program?"));
    confirm.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    if (NetworkAdjustmentPanel::allNetworkAdjustmentPanelsList.size() > 0)
    {
        confirm.addButton(QMessageBox::YesAll);
    }
    confirm.setDefaultButton  (QMessageBox::Cancel);
    int dialogCode  = confirm.exec();

    if (dialogCode == QMessageBox::Yes)
    {
        event->accept();
        GamaQ2help::get()->close();
        for (const auto& p : qAsConst(localPluginsList)) p->close();
    }
    else if (dialogCode == QMessageBox::YesAll)
    {
        NetworkAdjustmentPanel::closeAllNetworkAdjustmentPanels = true;
        for (int i=0; i<NetworkAdjustmentPanel::allNetworkAdjustmentPanelsList.size(); i++)
        {
            NetworkAdjustmentPanel::allNetworkAdjustmentPanelsList[i]->close();
        }

        event->accept();
        GamaQ2help::get()->close();
        for (const auto& p : qAsConst(localPluginsList)) p->close();
    }
    else
    {
        event->ignore();
    }
}

void GamaQ2ControlPanel::on_action_Connect_to_database_triggered()
{
    DBConnectDialog* d = new DBConnectDialog(GamaQ2::connection_implicit_db, this);
    connect(d, SIGNAL(input_data_open(bool)), this, SLOT(disable_input_data(bool)));
    d->exec();
    delete d;

    QSqlDatabase db = QSqlDatabase::database(GamaQ2::connection_implicit_db);
    if (db.isOpen())
    {
        QSqlQuery q(db);
        q.exec("SELECT opt_key, opt_val FROM gnu_gama_local_options");
        while (q.next())
        {
            QString key = q.value(0).toString();
            QString val = q.value(1).toString();

            if (key == "AdjResultsLanguage") set_adjustment_results_language(val);
        }
    }
}

void GamaQ2ControlPanel::disable_input_data(bool yes)
{
    actionDbConnect->setDisabled(yes);

    actionDbDropSchema->setEnabled(yes);
    actionDbDeleteData->setEnabled(yes);
    actionDbDeleteConfiguration->setEnabled(yes);

    actionDbImport->setEnabled(yes);
    actionImportExamples->setEnabled(yes);
    actionAdjAdjustment->setEnabled(yes);
    actionAdjResultsLanguage->setEnabled(yes);

    if (yes) ShowMessage(tr("Connected to database"));
}

void GamaQ2ControlPanel::on_action_Import_configuration_file_triggered()
{
    ImportConfigurationFile* icf = new ImportConfigurationFile(0);
    icf->exec();
}

void GamaQ2ControlPanel::on_action_Network_adjustment_triggered()
{
    NetworkAdjustmentPanel* nap = new NetworkAdjustmentPanel(GamaQ2::connection_implicit_db);
    connect(nap,  SIGNAL(networkAdjustmentPanel(bool)), this, SLOT(adjustmentPanel(bool)));
    connect(this, SIGNAL(gamaCloseSignal()), nap, SLOT(close()));
    nap->setAttribute(Qt::WA_DeleteOnClose);
    nap->exec();
}

void GamaQ2ControlPanel::on_action_Drop_schema_Tables_triggered()
{
    DB_DropTables* dbf = new DB_DropTables(this);
    if (dbf->exec())
    {
        QSqlDatabase::removeDatabase(GamaQ2::connection_implicit_db);
        disable_input_data(false);
    }
}

void GamaQ2ControlPanel::on_action_Delete_all_Data_from_the_Schema_triggered()
{
    DB_DeleteAllData* dbf = new DB_DeleteAllData(this);
    dbf->exec();
}

void GamaQ2ControlPanel::on_action_Delete_Network_Configuration_triggered()
{
    DB_DeleteNetworkConfiguration* dbf = new DB_DeleteNetworkConfiguration(this);
    dbf->exec();
}

void GamaQ2ControlPanel::load_plugins()
{
    QDir gamaq2plugins(qApp->applicationDirPath());
    gamaq2plugins.cd("plugins");
    auto qdirfiles = gamaq2plugins.entryList(QDir::Files);
    for (QString& fileName : qdirfiles) {
        QPluginLoader pluginLoader(gamaq2plugins.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            if (DbInterface* dbinterface = qobject_cast<DbInterface*>(plugin))
            {
                mapDbPlugins[dbinterface->getName() + fileName] = dbinterface;
            }
        }
    }
}

void GamaQ2ControlPanel::on_action_About_gama_q2_triggered()
{
    QMessageBox about(this);
    about.setIconPixmap(windowIcon().pixmap(iconSize().width()*2));

    about.setWindowTitle(tr("About gama-q2"));
    about.setTextFormat(Qt::RichText);
    about.setText(tr("<p>Adjustment of geodetic networks with database support</p>") +

                  "<p><b>" + GamaQ2::name + "&nbsp;" + GamaQ2::version + "</b>"
                  " Copyright (C) " + GamaQ2::copyright + " Aleš Čepek <i>et al.</i></p>"
                  "<p>" +
                  QString(tr("Based on <a href='http://www.gnu.org/software/gama'>"
                             "GNU gama</a> version %1")
                         ).arg(GNU_gama::GNU_gama_version().c_str()) +
                  "</p>"

                  "<p>This program is free software: you can redistribute it and/or modify "
                  "it under the terms of the GNU General Public License as published by "
                  "the Free Software Foundation, either version 3 of the License, or "
                  "(at your option) any later version.</p>"

                  "<p>This program is distributed in the hope that it will be useful, "
                  "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                  "GNU General Public License for more details.</p>"

                  "<p>You should have received a copy of the GNU General Public License "
                  " along with this program.  If not, see <a "
                  "href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a>.</p>"
                  );
    about.exec();
}

void GamaQ2ControlPanel::on_action_About_qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void GamaQ2ControlPanel::set_adjustment_results_language(QString language)
{
    using namespace  GNU_gama::local;

    if      (cmp(language, "en")) set_gama_language(en);
    else if (cmp(language, "ca")) set_gama_language(ca);
    else if (cmp(language, "cs")) set_gama_language(cz);
    else if (cmp(language, "cz")) set_gama_language(cz);
    else if (cmp(language, "du")) set_gama_language(du);
    else if (cmp(language, "es")) set_gama_language(es);
    else if (cmp(language, "fr")) set_gama_language(fr);
    else if (cmp(language, "fi")) set_gama_language(fi);
    else if (cmp(language, "hu")) set_gama_language(hu);
    else if (cmp(language, "ru")) set_gama_language(ru);
    else if (cmp(language, "ua")) set_gama_language(ua);
    else if (cmp(language, "zh")) set_gama_language(zh);
    else
    {
        init_language_list = false;

        // redefine implicit language in Qt Linguist during localisation
        set_adjustment_results_language(tr("en"));
    }
    init_language_list = false;
}

void GamaQ2ControlPanel::on_action_Application_Font()
{
  bool ok {false};
  QFont font = QFontDialog::getFont(&ok, this);
  if (ok)
    {
       QApplication::setFont(font);
       QSettings settings;
       settings.setValue("gama-q2-font", QApplication::font().toString());
    }
}

bool GamaQ2ControlPanel::cmp(QString s, QString c)
{
    if (init_language_list) GamaQ2::languages.push_back(c);

    if (s != c) return false;

    GamaQ2::currentLanguage = c;
    return true;
}

void GamaQ2ControlPanel::on_action_Adjustment_results_language()
{
    SelectAdjResultsLanguage dialog(this);
    dialog.exec();
}

void GamaQ2ControlPanel::adjustmentPanel(bool newPanel)
{
    static int count;
    if (newPanel) count++;
    else          count--;

    actionDbDropSchema->setDisabled(count > 0);
}

void GamaQ2ControlPanel::import_examples()
{
    QString exdir { ":/examples/" };
    int count {0};

    QDir examples (exdir);
    auto qdirfiles = examples.entryList(QDir::Files);
    for (QString& fileName : qdirfiles) {

        count++;
        //QString confName = fileName.left(10);
        QString confName;
        for (auto c : fileName) {
            if (c == '.') break;
            confName += c;
        }
        QFile file(exdir + fileName);
        file.open(QIODevice::ReadOnly);

        std::istringstream xml { file.readAll().toStdString() };

        GNU_gama::local::LocalNetwork lnet;
        GNU_gama::local::LocalNetwork2sql imp(lnet);
        imp.readGkf(xml);

        std::stringstream sql;
        imp.write(sql, confName.toStdString());

        QSqlDatabase db = QSqlDatabase::database(GamaQ2::connection_implicit_db);
        QSqlQuery query(db);

        db.transaction();

        std::string text;
        while (std::getline(sql, text))
        {
            // skip empty lines from imp.write(sql, confName.toStdString());
            if (text.length() == 0) continue;

            while (sql && text.back() != ';') text += char(sql.get());
            if (text.back() != ';') break;

            // avoid nested transactions
            if (text.find("begin;" ) != std::string::npos) continue;
            if (text.find("commit;") != std::string::npos) continue;

            query.exec(text.c_str());
            if (query.lastError().isValid())
            {
                QMessageBox::critical(this, tr("Database error"),
                                      query.lastError().databaseText() + "\n" + text.c_str());
                db.rollback();
                close();
                return;
            }
        }

        db.commit();
    }

    /*
    QMessageBox msgBox;
    msgBox.setText(tr("All %1 examples imported into the database").arg(count));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("Import Examples"));
    msgBox.button(QMessageBox::Ok);

    QTimer::singleShot(2000, &msgBox, SLOT(close()));
    msgBox.exec();
    */
    controlPanel->statusBar()->showMessage(tr("All %1 examples imported").arg(count), 0);
}
