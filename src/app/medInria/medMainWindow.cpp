/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include <medBrowserArea.h>
//#include <medDatabaseNonPersistentController.h>
#include <medDataManager.h>
#include <medEmptyDbWarning.h>
#include <medHomepageArea.h>
#include <medJobManagerL.h>
#include <medLogger.h>
#include <medMainWindow.h>
#include <medQuickAccessMenu.h>
//#include <medSaveModifiedDialog.h>
#include <medSearchToolboxDialog.h>
#include <medSelectorToolBox.h>
#include <medSelectorWorkspace.h>
#include <medSettingsManager.h>
#include <medStatusBar.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medVisualizationWorkspace.h>
#include <medWorkspaceArea.h>
#include <medWorkspaceFactory.h>

#include <medApplicationContext.h>
#include <medNotificationPresenter.h>
#include <medNotifWindow.h>

#include <medSourcesLoader.h>
#include <medSourcesLoaderPresenter.h>

#include <medStartupSettingsWidget.h>
#include <medPluginWidget.h>

#include <QtGui>
#include <QtWidgets>

#ifdef Q_OS_MAC
# define CONTROL_KEY "Meta"
#else
# define CONTROL_KEY "Ctrl"
#endif

//--------------------------------------------------------------------------
// medMainWindow

class medMainWindowPrivate
{
public:
    //  Interface elements.
    QWidget *currentArea;

    QStackedWidget*            stack;
    medBrowserArea*            browserArea;
    medWorkspaceArea*          workspaceArea;
    medHomepageArea*           homepageArea;
    QHBoxLayout*               statusBarLayout;
    medNotificationPaneWidget* notifWindow;
    
    medQuickAccessMenu *shortcutAccessWidget;
    bool shortcutAccessVisible;
    bool closeEventProcessed;
    QShortcut * shortcutShortcut;

    QList<QString> importUuids;
    QList<QUuid> expectedUuids;

    QAction *actionFullscreen;
};

medMainWindow::medMainWindow ( QWidget *parent ) : QMainWindow ( parent ), d ( new medMainWindowPrivate )
{
    //  Etch-disabled-text stylesheet property was deprecated.
    //  This is the only way I found to avoid the label's text look like etched when disabled
    //  also not puting the opacity to 0 works very well, except for tooltips
    //  but only tooltips in a QGroupBox, others are fine...
    //  Solution: put a transparent color to the etching palette (Light).

    QPalette p = QApplication::palette();
    p.setColor(QPalette::Disabled, QPalette::Light, QColor(100,100,100,0));
    QApplication::setPalette(p);

    //  To avoid strange behaviours with the homepageshifter
    this->setMinimumHeight(700);
    this->setMinimumWidth(950);

    d->closeEventProcessed = false;

    //  Setting up widgets
    d->currentArea = nullptr;

    //  Browser area.
    d->browserArea = new medBrowserArea(this);
    d->browserArea->setObjectName("medBrowserArea");

    //  Workspace area.
    d->workspaceArea = new medWorkspaceArea (this);
    d->workspaceArea->setObjectName("medWorkspaceArea");

    //  Home page
    d->homepageArea = new medHomepageArea( this );
    d->homepageArea->setObjectName("medHomePageArea");


    //  Stack
    d->stack = new QStackedWidget(this);
    d->stack->addWidget(d->homepageArea);
    d->stack->addWidget(d->browserArea);
    d->stack->addWidget(d->workspaceArea);

    // Shortcut to workspaces through CTRL+Space
    d->shortcutAccessWidget = new medQuickAccessMenu(this);
    d->shortcutAccessWidget->setFocusPolicy(Qt::ClickFocus);
    d->shortcutAccessWidget->setProperty("pos", QPoint(0, -500));

    connect(d->shortcutAccessWidget, SIGNAL(menuHidden()), this, SLOT(hideShortcutAccess()));
    connect(d->shortcutAccessWidget, SIGNAL(homepageSelected()), this, SLOT(switchToHomepageArea()));
    connect(d->shortcutAccessWidget, SIGNAL(browserSelected()), this, SLOT(switchToBrowserArea()));
    connect(d->shortcutAccessWidget, SIGNAL(workspaceSelected(QString)), this, SLOT(showWorkspace(QString)));

    d->shortcutAccessVisible = false;

    statusBar()->setVisible(false);
    statusBar()->setMaximumHeight(false);

    //  Init homepage with workspace buttons
    d->homepageArea->initPage();
    connect(d->homepageArea, SIGNAL(showBrowser()), this, SLOT(switchToBrowserArea()));
    connect(d->homepageArea, SIGNAL(showWorkspace(QString)), this, SLOT(showWorkspace(QString)));

    this->setCentralWidget ( d->stack );
    this->setWindowTitle(qApp->applicationName());

    d->shortcutShortcut = new QShortcut(QKeySequence(tr(CONTROL_KEY "+Space")),
                                                     this,
                                                     SLOT(showShortcutAccess()),
                                                     SLOT(showShortcutAccess()),
                                                     Qt::ApplicationShortcut);
    this->restoreSettings();

    // Switch to the default workspace at application start
    switchToDefaultWorkSpace();

    this->setAcceptDrops(true);

    auto * notifSys = medApplicationContext::instance()->getNotifSys();
    d->notifWindow = static_cast<medNotificationPaneWidget*>(medNotifSysPresenter(notifSys).buildNotificationWindow());
    d->notifWindow->setParent(this);
    QObject::connect(this, &medMainWindow::resized, d->notifWindow, &medNotificationPaneWidget::windowGeometryUpdate);

    initMenuBar(parent);

    //installEventFilter(this);
}

void medMainWindow::initMenuBar(QWidget * parent)
{
    // Menu bar
    QMenuBar *menu_bar = this->menuBar();

    // --- File menu
    QMenu *menuFile = menu_bar->addMenu("File");

    QAction *actionBrowser = new QAction(tr("&Import/export files"), parent);
    connect(actionBrowser, &QAction::triggered, this, &medMainWindow::switchToBrowserArea);
    menuFile->addAction(actionBrowser);

    // --- Area menu
    QMenu *menuArea = menu_bar->addMenu("Switch to area");

    QAction *actionAreaSettings = new QAction(tr("&Startup settings"), parent);
    connect(actionAreaSettings, &QAction::triggered, this, &medMainWindow::onShowAreaSettings);
    menuArea->addAction(actionAreaSettings);

    menuArea->addSeparator();

    QAction *actionHome = new QAction(tr("&Homepage"), parent);
    connect(actionHome, &QAction::triggered, this, &medMainWindow::switchToHomepageArea);
    menuArea->addAction(actionHome);

    // Visualization workspace is a "Basic" area
    QAction *actionVisu = new QAction("&Visualization", parent);
    actionVisu->setData("medVisualizationWorkspace");
    connect(actionVisu, &QAction::triggered, [this]() {this->showWorkspace("medVisualizationWorkspace"); });
    menuArea->addAction(actionVisu);

    menuArea->addSeparator();

    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);
    for (medWorkspaceFactory::Details* detail : workspaceDetails)
    {
        if (detail->name != "Visualization")
        {
            QAction *actionWorkspace = new QAction(detail->name, parent);
            actionWorkspace->setData(detail->identifier);
            connect(actionWorkspace, &QAction::triggered, this, &medMainWindow::onSwitchToWorkspace);
            menuArea->addAction(actionWorkspace);
        }
    }

    // --- View menu
    QMenu *menuView = menu_bar->addMenu("View");

    QAction *actionAjust = new QAction(tr("&Ajust containers size"), parent);
    connect(actionAjust, &QAction::triggered, this, &medMainWindow::adjustContainersSize);
    menuView->addAction(actionAjust);

    QAction *actionScreenshot = new QAction(tr("Capture screenshot"), parent);
    connect(actionScreenshot, &QAction::triggered, this, &medMainWindow::captureScreenshot);
    menuView->addAction(actionScreenshot);

    QAction *actionMovie = new QAction(tr("Capture movie"), parent);
    connect(actionMovie, &QAction::triggered, this, &medMainWindow::captureVideo);
    menuView->addAction(actionMovie);

    // --- Tools menu
    QMenu *menuTools = menu_bar->addMenu("Tools");

    QAction *actionSearch = new QAction(tr("&Search a toolbox"), parent);
    connect(actionSearch, &QAction::triggered, this, &medMainWindow::switchToSearchArea);
    menuTools->addAction(actionSearch);

    // --- Log menu
    QMenu *menuLog = menu_bar->addMenu("Log");

    QAction *actionLog = new QAction(tr("&Log File"), parent);
    connect(actionLog, &QAction::triggered, this, &medMainWindow::openLogDirectory);
    menuLog->addAction(actionLog);

    QAction *actionPluginLogs = new QAction(tr("&Plugin Logs"), parent);
    connect(actionPluginLogs, &QAction::triggered, this, &medMainWindow::onShowPluginLogs);
    menuLog->addAction(actionPluginLogs);

    // --- About menu
    QMenu *menuAbout = menu_bar->addMenu("Info");

    QAction *actionAbout = new QAction(tr("&About the application"), parent);
    connect(actionAbout, &QAction::triggered, this, &medMainWindow::onShowAbout);
    menuAbout->addAction(actionAbout);

    QAction *actionAuthors = new QAction(tr("Au&thors"), parent);
    connect(actionAuthors, &QAction::triggered, this, &medMainWindow::onShowAuthors);
    menuAbout->addAction(actionAuthors);

    QAction *actionReleaseNotes = new QAction(tr("&Release Notes"), parent);
    connect(actionReleaseNotes, &QAction::triggered, this, &medMainWindow::onShowReleaseNotes);
    menuAbout->addAction(actionReleaseNotes);

    QAction *actionLicense = new QAction(tr("&License"), parent);
    connect(actionLicense, &QAction::triggered, this, &medMainWindow::onShowLicense);
    menuAbout->addAction(actionLicense);

    menuAbout->addSeparator();

    QAction *actionHelp = new QAction(tr("&Help"), parent);
    connect(actionHelp, &QAction::triggered, this, &medMainWindow::onShowHelp);
    menuAbout->addAction(actionHelp);

    // --- File menu
    QMenu *menuSettings = menu_bar->addMenu("Settings");

    QAction *actionDataSources = new QAction(tr("&Data Sources"), parent);
    connect(actionDataSources, &QAction::triggered, this, &medMainWindow::onShowDataSources);
    menuSettings->addAction(actionDataSources);

    // --- Notif Action
    QMenu* menuNotif = menu_bar->addMenu("Notif");
    QAction* showHideNotifs = menuNotif->addAction("Show / Hide");
    QAction* clearNotifs = menuNotif->addAction("Clear all notifications");
    connect(showHideNotifs, &QAction::triggered, this, &medMainWindow::toggleNotificationPanel);

    // --- Prepare right corner menu
    QMenuBar *rightMenuBar = new QMenuBar(menu_bar);
    menu_bar->setCornerWidget(rightMenuBar);

    QAction* actionNotif = rightMenuBar->addAction("");
    actionNotif->setIcon(QIcon(":/icons/button-notifications - withe.png"));
    connect(actionNotif, &QAction::triggered, this, &medMainWindow::toggleNotificationPanel);

    //QAction *actionNotif = new QAction(tr("&Data Sources"), parent);
    connect(actionDataSources, &QAction::triggered, this, &medMainWindow::onShowDataSources);
    menuSettings->addAction(actionDataSources);

    // --- Fullscreen checkable action
    QIcon fullscreenIcon;
    fullscreenIcon.addPixmap(QPixmap(":icons/fullscreenExpand.png"), QIcon::Normal, QIcon::Off);
    fullscreenIcon.addPixmap(QPixmap(":icons/fullscreenReduce.png"), QIcon::Normal, QIcon::On);

    d->actionFullscreen = new QAction(parent);
    d->actionFullscreen->setIcon(fullscreenIcon);
    d->actionFullscreen->setCheckable(true);
    d->actionFullscreen->setChecked(false);
#if defined(Q_OS_MAC)
    d->actionFullscreen->setShortcut(Qt::ControlModifier + Qt::Key_F);
    d->actionFullscreen->setToolTip(tr("Switch to fullscreen (cmd+f)"));
#else
    d->actionFullscreen->setShortcut(Qt::Key_F11);
    d->actionFullscreen->setToolTip(tr("Switch to fullscreen (F11)"));
#endif
    connect(d->actionFullscreen, &QAction::toggled, this, &medMainWindow::setFullScreen);
    // On Qt5, QAction in menubar does not seem to show the Off and On icons, so we do it manually
    connect(d->actionFullscreen, &QAction::toggled, this, &medMainWindow::switchOffOnFullscreenIcons);
    rightMenuBar->addAction(d->actionFullscreen);
}

medMainWindow::~medMainWindow()
{
    delete d;
    d = nullptr;
}

void medMainWindow::mousePressEvent ( QMouseEvent* event )
{
    QWidget::mousePressEvent ( event );
    this->hideShortcutAccess();
}

void medMainWindow::restoreSettings()
{
    medSettingsManager * mnger = medSettingsManager::instance();

    this->restoreState(mnger->value("medMainWindow", "state").toByteArray());
    this->restoreGeometry(mnger->value("medMainWindow", "geometry").toByteArray());
}

void medMainWindow::saveSettings()
{
    if(!this->isFullScreen())
    {
        medSettingsManager * mnger = medSettingsManager::instance();
        mnger->setValue("medMainWindow", "state", this->saveState());
        mnger->setValue("medMainWindow", "geometry", this->saveGeometry());

        // Keep the current screen for multiple-screens display
        mnger->setValue("medMainWindow", "currentScreen", QApplication::desktop()->screenNumber(this));
    }
}

void medMainWindow::switchToDefaultWorkSpace()
{
    QString startupWorkspace = medSettingsManager::instance()->value("startup", "default_starting_area").toString();

    if (startupWorkspace == "Homepage")
    {
        switchToHomepageArea();
    }
    else if (startupWorkspace == "Import/export files")
    {
        switchToBrowserArea();
    }
    else
    {
        QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);
        for( medWorkspaceFactory::Details *detail : workspaceDetails )
        {
            if (startupWorkspace == detail->name)
            {
                showWorkspace(detail->identifier);
                break;
            }
        }
    }
}

void medMainWindow::toggleNotificationPanel()
{
    d->notifWindow->swithVisibility();
}

/**
 * If one tries to launch a new instance of the application, QtSingleApplication bypass it and receive
 * the command line argument used to launch it.
 * See QtSingleApplication::messageReceived(const QString &message).
 * This method processes a message received by the QtSingleApplication from a new instance.
 */
void medMainWindow::processNewInstanceMessage(const QString& message)
{
    if (message.toLower().startsWith("/open "))
    {
        const QString filename = message.mid(6);
        this->setStartup(medMainWindow::WorkSpace, QStringList() << filename);
    }
}

void medMainWindow::setStartup(const AreaType areaIndex,const QStringList& filenames) {
    switchToArea(areaIndex);
    for (QStringList::const_iterator i= filenames.constBegin();i!=filenames.constEnd();++i)
        open(i->toLocal8Bit().constData());
}

// bool medMainWindow::eventFilter(QObject * object, QEvent * event)
// {
//     auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
//     if (mouseEvent)
//     {
//         auto type = event->type();
//         if (type == QEvent::MouseButtonRelease)
//         {
//             QPoint pos = mouseEvent->pos();
//             qDebug() << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG" << pos;
//         }
//     }
// 
//     return false;
// }

void medMainWindow::switchToArea(const AreaType areaIndex)
{
    switch (areaIndex)
    {
    case medMainWindow::HomePage:
        this->switchToHomepageArea();
        break;

    case medMainWindow::Browser:
        this->switchToBrowserArea();
        break;

    case medMainWindow::WorkSpace:
        this->switchToWorkspaceArea();
        break;

    default:
        this->switchToHomepageArea();
        break;
    }
}

void medMainWindow::open(const medDataIndex &index)
{
    this->showWorkspace(medVisualizationWorkspace::staticIdentifier());
    d->workspaceArea->currentWorkspace()->open(index);
}

void medMainWindow::open(const QString & path)
{
    QEventLoop loop;
    QUuid uuid = medDataManager::instance()->importPath(path, false);
    if (!uuid.isNull())
    {
        d->expectedUuids.append(uuid);
        connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)), this, SLOT(open_waitForImportedSignal(medDataIndex,QUuid)));
        while( d->expectedUuids.contains(uuid))
        {
            loop.processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(path + " is not valid");
        msgBox.exec();
    }
}


void medMainWindow::open_waitForImportedSignal(medDataIndex index, QUuid uuid)
{
    if(d->expectedUuids.contains(uuid))
    {
        d->expectedUuids.removeAll(uuid);
        disconnect(medDataManager::instance(),SIGNAL(dataImported(medDataIndex,QUuid)), this,SLOT(open_waitForImportedSignal(medDataIndex,QUuid)));
        if (index.isValid())
        {
            this->showWorkspace(medVisualizationWorkspace::staticIdentifier());
            d->workspaceArea->currentWorkspace()->open(index);
        }
    }
}











void medMainWindow::onShowBrowser()
{
    switchToBrowserArea();
}

void medMainWindow::onShowDataSources()
{
    QDialog *dialog = new QDialog();
    medSourcesLoaderPresenter presenter(medSourcesLoader::instance());
    auto wgt = presenter.buildWidget();
    auto layout = new QVBoxLayout();
    layout->addWidget(wgt);
    dialog->setLayout(layout);
    dialog->exec();
}


//void medMainWindow::onShowNotifPanel()
//{
//    d->notifWindow->swithVisibility();
//}

void medMainWindow::onShowAbout()
{
    QFile file(":ABOUT.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void medMainWindow::onShowAuthors()
{
    QFile file(":authors.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void medMainWindow::onShowReleaseNotes()
{
    QFile file(":RELEASE_NOTES.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText("Here is the release notes with the history of the application:            ");
    msgBox.setDetailedText(text);

    // Search the "Show Details..." button
    foreach(QAbstractButton *button, msgBox.buttons())
    {
        if (msgBox.buttonRole(button) == QMessageBox::ActionRole)
        {
            button->click(); // click it to expand the text
            break;
        }
    }

    msgBox.exec();
}

void medMainWindow::onShowLicense()
{
    QFile file(":LICENSE.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText("Here is the application License:                           ");
    msgBox.setDetailedText(text);

    // Search the "Show Details..." button
    foreach(QAbstractButton *button, msgBox.buttons())
    {
        if (msgBox.buttonRole(button) == QMessageBox::ActionRole)
        {
            button->click(); // click it to expand the text
            break;
        }
    }

    msgBox.exec();
}

void medMainWindow::onShowAreaSettings()
{
    medStartupSettingsWidget dialog(this);
    dialog.exec();
}

void medMainWindow::onShowWorkspace(QString workspace)
{
    emit showWorkspace(workspace);
}

void medMainWindow::onSwitchToWorkspace()
{
    QAction* currentAction = qobject_cast<QAction*>(sender());
    onShowWorkspace(currentAction->data().toString());
}

void medMainWindow::openLogDirectory()
{
    QString path = QFileInfo(dtkLogPath(qApp)).path();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void medMainWindow::onShowPluginLogs()
{
    medPluginWidget dialog(this);
    dialog.exec();
}

void medMainWindow::onShowHelp()
{
    QDesktopServices::openUrl(QUrl("http://med.inria.fr/help/documentation"));
}

void medMainWindow::switchOffOnFullscreenIcons(const bool checked)
{
    if (checked)
    {
        d->actionFullscreen->setIcon(QIcon(":icons/fullscreenReduce.png"));
    }
    else
    {
        d->actionFullscreen->setIcon(QIcon(":icons/fullscreenExpand.png"));
    }
}







void medMainWindow::setWallScreen (const bool full )
{
    if ( full )
    {
        this->move ( 0, -30 );
        this->resize ( 3528, 1200 );
    }
    else
    {
        this->showNormal();
    }
}

void medMainWindow::setFullScreen (const bool full)
{
    if ( full )
        this->showFullScreen();
    else
        this->showNormal();
}

void medMainWindow::toggleFullScreen()
{
    if ( !this->isFullScreen())
        this->showFullScreen();
    else
        this->showNormal();
}

void medMainWindow::captureScreenshot()
{
    QPixmap screenshot = d->workspaceArea->grabScreenshot();

    if (!screenshot.isNull())
    {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save screenshot as"),
                                                        QString(QDir::home().absolutePath() + "/screen.png"),
                                                        "Image files (*.png *.jpeg *.jpg);;All files (*.*)",
                                                        0);

        QByteArray format = fileName.right(fileName.lastIndexOf('.')).toUpper().toLatin1();
        if ( ! QImageWriter::supportedImageFormats().contains(format) )
        {
            format = "PNG";
        }

        QImage image = screenshot.toImage();
        image.save(fileName, format.constData());
    }
}

void medMainWindow::captureVideo()
{
    d->workspaceArea->grabVideo();
}

QWidget* medMainWindow::currentArea() const
{
    return d->currentArea;
}

void medMainWindow::enableMenuBarItem(QString name, bool enabled)
{
    QList<QMenu*> menus = menuBar()->findChildren<QMenu*>();

    for (QMenu *currentMenu : menus)
    {
        if (name == currentMenu->title())
        {
            currentMenu->setEnabled(enabled);
        }
    }
}

QToolButton * medMainWindow::notifButton()
{
    return nullptr; //d->notifButton;
}

void medMainWindow::switchToHomepageArea()
{
    if(d->currentArea != d->homepageArea)
    {
        d->currentArea = d->homepageArea;

        d->shortcutAccessWidget->updateSelected("Homepage");
        if (d->shortcutAccessVisible)
        {
            this->hideShortcutAccess();
        }

        d->stack->setCurrentWidget(d->homepageArea);

        // The View menu is dedicated to "view workspaces"
        enableMenuBarItem("View", false);
    }
}

void medMainWindow::switchToBrowserArea()
{
    if(d->currentArea != d->browserArea)
    {
        d->currentArea = d->browserArea;

        d->shortcutAccessWidget->updateSelected("Import/export files");
        if (d->shortcutAccessVisible)
        {
            this->hideShortcutAccess();
        }

        d->stack->setCurrentWidget(d->browserArea);

        // The View menu is dedicated to "view workspaces"
        enableMenuBarItem("View", false);

        // The Filesystem tab is by default opened
        d->browserArea->switchToIndexTab(1);
    }
}

void medMainWindow::switchToSearchArea()
{
    // Create toolbox list
    QHash<QString, QStringList> toolboxDataHash;
    medToolBoxFactory *tbFactory = medToolBoxFactory::instance();
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName();
    for( medWorkspaceFactory::Details *detail : workspaceDetails )
    {
        QString workspaceName = detail->name;

        for(QString toolboxName : tbFactory->toolBoxesFromCategory(workspaceName))
        {
            medToolBoxDetails *toolboxDetails = tbFactory->toolBoxDetailsFromId(toolboxName);

            QStringList current;
            // Displayed toolbox name from MED_TOOLBOX_INTERFACE
            current.append(toolboxDetails->name);
            // Toolbox description found in MED_TOOLBOX_INTERFACE
            current.append(toolboxDetails->description);
            // Some toolboxes have multiple categories/workspace, we only keep the first
            current.append(workspaceName);
            // Internal toolbox name, class name
            current.append(toolboxName);

            // Some toolboxes have multiple workspace categories
            if (toolboxDataHash[toolboxName].isEmpty())
            {
                toolboxDataHash[toolboxName] = current;
            }
        }
    }
    medSearchToolboxDialog dialog(this, toolboxDataHash);

    if (dialog.exec() == QDialog::Accepted)
    {
        // Get back workspace of toolbox chosen by user
        // Name, Description, Workspace, Internal Name
        QStringList chosenToolboxInfo = dialog.getFindText();

        QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName();
        for( medWorkspaceFactory::Details *detail : workspaceDetails )
        {
            if (chosenToolboxInfo.at(2) == detail->name)
            {
                showWorkspace(detail->identifier);

                // Display asked toolbox
                medSelectorToolBox *selector = static_cast<medSelectorWorkspace*>(d->workspaceArea->currentWorkspace())->selectorToolBox();
                int toolboxIndex = selector->getIndexOfToolBox(chosenToolboxInfo.at(0));
                if (toolboxIndex > 0)
                {
                    selector->comboBox()->setCurrentIndex(toolboxIndex);
                    selector->changeCurrentToolBox(toolboxIndex);
                }

                break;
            }
        }
    }
}

void medMainWindow::switchToWorkspaceArea()
{
    if(d->currentArea != d->workspaceArea)
    {
        d->currentArea = d->workspaceArea;

        this->hideShortcutAccess();

        d->stack->setCurrentWidget(d->workspaceArea);

        // Dialog window to recall users if database is empty
        // but only if the warning is enabled in medSettings
        // bool showWarning = medSettingsManager::instance()->value(
        //             "system",
        //             "showEmptyDbWarning",
        //             QVariant(true)).toBool();
        // if ( showWarning )
        // {
        //     QList<medDataIndex> indexes = medDatabaseNonPersistentController::instance()->availableItems();
        //     QList<medDataIndex> patients = medDatabaseController::instance()->patients();
        //     if( indexes.isEmpty() )
        //     {
        //         if( patients.isEmpty())
        //         {
        //             medEmptyDbWarning* msgBox = new medEmptyDbWarning(this);
        //             msgBox->exec();
        //         }
        //     }
        // }
    }
}

void medMainWindow::showWorkspace(QString workspace)
{
    switchToWorkspaceArea();
    medWorkspaceFactory::Details* details = medWorkspaceFactory::instance()->workspaceDetailsFromId(workspace);

    if (details)
    {
        d->shortcutAccessWidget->updateSelected(workspace);

        if (!d->workspaceArea->setCurrentWorkspace(workspace))
        {
            QString message = QString("Cannot open workspace ") + details->name;
            medMessageController::instance()->showError(message, 3000);
            switchToHomepageArea();
        }

        // The View menu is dedicated to "view workspaces"
        enableMenuBarItem("View", true);

        this->hideShortcutAccess();
    }
}

/**
 * Slot to show alt-tab like menu
 */
void medMainWindow::showShortcutAccess()
{
    if (d->shortcutAccessVisible)
    {
        d->shortcutAccessWidget->updateCurrentlySelectedRight();
        return;
    }

    d->shortcutAccessWidget->reset(true);
    d->shortcutAccessVisible = true;

    QPoint menuPosition = this->mapToGlobal(this->rect().topLeft());
    menuPosition.setX(menuPosition.rx() + (this->rect().width() - d->shortcutAccessWidget->width()) / 2);
    menuPosition.setY(menuPosition.ry() + (this->rect().height() - d->shortcutAccessWidget->height()) / 2);

    d->shortcutAccessWidget->move(menuPosition);
    d->shortcutAccessWidget->show();
    d->shortcutAccessWidget->setFocus();
    d->shortcutAccessWidget->setMouseTracking(true);
    d->shortcutAccessWidget->grabKeyboard();
}

/**
 * Slot to hide alt-tab like menu
 */
void medMainWindow::hideShortcutAccess()
{
    if (d->shortcutAccessVisible)
    {
        d->shortcutAccessWidget->releaseKeyboard();
        d->shortcutAccessWidget->setMouseTracking(false);
        d->shortcutAccessVisible = false;
        d->shortcutAccessWidget->setProperty("pos", QPoint ( 0 , -500 ));
        d->shortcutAccessWidget->hide();
        this->activateWindow();
    }
}

void medMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void medMainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void medMainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void medMainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            pathList.append(urlList.at(i).toLocalFile());
        }

        // call a function to open the files
        for (int i = 0; i < pathList.size() ; ++i)
        {
            open(pathList[i]);
        }
        event->acceptProposedAction();
    }
}

// void medMainWindow::availableSpaceOnStatusBar()
// {
//     d->statusBar->setAvailableSpace(width());
// }

void medMainWindow::closeEvent(QCloseEvent *event)
{
    if (d->closeEventProcessed)
    {
        event->ignore();
        return;
    }

    dtkInfo() << "### Application is closing...";

    if ( QThreadPool::globalInstance()->activeThreadCount() > 0 )
    {
        int res = QMessageBox::information(this,
                                           tr("System message"),
                                           tr("Running background jobs detected! Quit anyway?"),
                                           tr("Quit"),
                                           tr("WaitForDone"),
                                           tr("Cancel"),
                                           0,
                                           1);

        if(res == 0)
        {
            // send cancel request to all running jobs, then wait for them
            // Note: most Jobs don't have the cancel method implemented, so this will be effectively the same as waitfordone.
            medJobManagerL::instance()->dispatchGlobalCancelEvent();
            QThreadPool::globalInstance()->waitForDone();
        }
        else
        {
            // just hide the window and wait
            QThreadPool::globalInstance()->waitForDone();
        }
    }

    //if(this->saveModifiedAndOrValidateClosing() != QDialog::Accepted)
    //{
    //    event->ignore();
    //    return;
    //}

    d->closeEventProcessed = true;
    this->saveSettings();

    event->accept();

    dtkInfo() << "####################################";
    medLogger::finalize();
}


bool medMainWindow::event(QEvent * e)
{
    switch(e->type())
    {
    case QEvent::WindowActivate :
    {
        // gained focus
        emit mainWindowActivated();
        break ;
    }

    case QEvent::WindowDeactivate :
    {
        // lost focus
        emit mainWindowDeactivated();
        break ;
    }
    default:
        break;
    } ;
    return QMainWindow::event(e) ;
}

void medMainWindow::adjustContainersSize()
{
    d->workspaceArea->currentWorkspace()->tabbedViewContainers()->adjustContainersSize();
}
