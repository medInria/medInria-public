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
    
    QVector<QAction *> wsActions;
    QMap<QMenu*, QVector<QAction*>> wsMenuActionsMap;
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
    QObject::connect(d->notifWindow, &medNotificationPaneWidget::expanded, medApplicationContext::instance()->getApp(), &medApplication::listenClick);
    QObject::connect(medApplicationContext::instance()->getApp(), &medApplication::mouseGlobalClick, d->notifWindow, &medNotificationPaneWidget::clicked);

    initMenuBar(parent);

}

medMainWindow::~medMainWindow()
{
    delete d;
    d = nullptr;
}


void medMainWindow::initMenuBar(QWidget * parent)
{
    // Menu bar
    QMenuBar *menu_bar = this->menuBar();
    menu_bar->setStyleSheet("QMenuBar { margin: 3px; }");
    menuFile(menu_bar);
    menuWorkspace(menu_bar);
    menuWindow(menu_bar);
    menuCapture(menu_bar);
    menuSettings(menu_bar);
    menuNotif(menu_bar);
    menuAbout(menu_bar);

    // --- Prepare right corner menu
    QMenuBar *rightMenuBar = new QMenuBar(menu_bar);
    menu_bar->setCornerWidget(rightMenuBar);

    QAction* actionNotif = rightMenuBar->addAction("");
    actionNotif->setIcon(QIcon::fromTheme("notifications"));
    connect(actionNotif, &QAction::triggered, this, &medMainWindow::toggleNotificationPanel);

    // --- Fullscreen checkable action
    QIcon fullscreenIcon;
    fullscreenIcon.addPixmap(QIcon::fromTheme("fullscreen_off").pixmap(24,24), QIcon::Normal, QIcon::Off);
    fullscreenIcon.addPixmap(QIcon::fromTheme("fullscreen_on").pixmap(24,24),  QIcon::Normal, QIcon::On);

    d->actionFullscreen = new QAction(parent);
    d->actionFullscreen->setObjectName("Fullscreen");
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
    rightMenuBar->addAction(d->actionFullscreen);
}

void medMainWindow::menuFile(QMenuBar * menu_bar)
{
    // --- File menu
    QMenu *menuFile = menu_bar->addMenu("File");
    menuFile->setToolTipsVisible(true);

    auto *actionOpenFiles = menuFile->addAction("Open File(s)");
    connect(actionOpenFiles, &QAction::triggered, this, &medMainWindow::openFromSystem);

    auto *actionOpenDicom = menuFile->addAction("Open DICOM");
    actionOpenDicom->setToolTip(tr("Choose a DICOM directory to import"));
    connect(actionOpenDicom, &QAction::triggered, this, &medMainWindow::openDicomFromSystem);
    menuFile->addAction("Save on disk");

    auto *actionBrowse     = menuFile->addAction("Browse data");
    connect(actionBrowse,    &QAction::triggered, this, &medMainWindow::switchToBrowserArea);
    menuFile->addSeparator();
    auto *actionGoHome = menuFile->addAction("Go to homepage");
    connect(actionGoHome,    &QAction::triggered, this, &medMainWindow::switchToHomepageArea);
    menuFile->addSeparator();
    auto *subMenuVisibilitySource = menuFile->addMenu("Source Visibility");
    
    QAction *virtualReprAction = subMenuVisibilitySource->addAction("Quick Access");
    medVirtualRepresentation *virtualRepresentation = medApplicationContext::instance()->getVirtualRepresentation();
    virtualReprAction->setCheckable(true);
    virtualReprAction->setChecked(true);
    connect(virtualReprAction, &QAction::triggered, virtualRepresentation, &medVirtualRepresentation::visibled);

    for (QString sourceId : medSourcesLoader::instance()->sourcesIdList())
    {
        auto *sourceAction = subMenuVisibilitySource->addAction(sourceId);
        sourceAction->setCheckable(true);
        sourceAction->setChecked(true);
        sourceAction->setData(sourceId);
        connect(sourceAction, &QAction::toggled, this, &medMainWindow::setSourceVisibility);
    }
 
    menuFile->addSeparator();
    QAction *actionQuit = menuFile->addAction(tr("Quit"));
    connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit);
}

void medMainWindow::menuWorkspace(QMenuBar * menu_bar)
{
    // --- Area menu
    QMenu *menuWorkspaces = menu_bar->addMenu("Workspaces");

    auto searchEdit = new QWidgetAction(menuWorkspaces);
    QLineEdit * researchWorkSpace = new QLineEdit();
    researchWorkSpace->setPlaceholderText("Search...");
    searchEdit->setDefaultWidget(researchWorkSpace);
    menuWorkspaces->addAction(searchEdit);
    menuWorkspaces->addSeparator();

    connect(researchWorkSpace, &QLineEdit::textEdited, this, &medMainWindow::filterWSMenu);


    medToolBoxFactory *tbFactory = medToolBoxFactory::instance();
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);
   
    QAction *visuAction = menuWorkspaces->addAction("Visualization");
    visuAction->setData(medVisualizationWorkspace::staticIdentifier());
    connect(visuAction, &QAction::triggered, this, &medMainWindow::onSwitchToWorkspace);
    d->wsActions.push_back(visuAction);

    for (medWorkspaceFactory::Details* detail : workspaceDetails)
    {
        if (detail->name == "Visualization")
        {
            continue;
        }
        if (tbFactory->toolBoxesFromCategory(detail->name).size() > 1)
        {
            QMenu *menuActionTmp = menuWorkspaces->addMenu(detail->name);
            QAction * openAction = menuActionTmp->addAction("Open " + detail->name);
            openAction->setData(detail->identifier);
            connect(openAction, &QAction::triggered, this, &medMainWindow::onSwitchToWorkspace);

            menuActionTmp->addSeparator();
            auto toolboxHashTable = tbFactory->toolBoxDetailsFromCategory(detail->name);
            for (QString toolboxId : toolboxHashTable.keys())
            {
                medToolBoxDetails *toolboxDetails = toolboxHashTable[toolboxId];
                QAction * subWorkSpaceAction = menuActionTmp->addAction(toolboxDetails->name);
                QStringList infos;
                infos << detail->identifier << toolboxId << toolboxDetails->name;
                QVariant varData = QVariant(infos);
                subWorkSpaceAction->setData(varData);
                connect(subWorkSpaceAction, &QAction::triggered, this, &medMainWindow::onSwitchToProcess);

                d->wsMenuActionsMap[menuActionTmp].push_back(subWorkSpaceAction);
            }
        }
        else
        {
            QAction *openAction = menuWorkspaces->addAction(detail->name);
            openAction->setData(detail->identifier);
            connect(openAction, &QAction::triggered, this, &medMainWindow::onSwitchToWorkspace);

            d->wsActions.push_back(openAction);
        }
    }
}

void medMainWindow::menuWindow(QMenuBar * menu_bar)
{
    // --- Window menu
    QMenu *menuWindow = menu_bar->addMenu("Window");

    menuWindow->addAction("Maximize");
    menuWindow->addSeparator();
    auto *actionAjust = menuWindow->addAction(tr("Adjust containers size"));
    menuWindow->addAction(tr("Vertical split"));
    menuWindow->addAction(tr("Horizontal split"));
    menuWindow->addAction(tr("4 split"));

    connect(actionAjust, &QAction::triggered, this, &medMainWindow::adjustContainersSize);
}

void medMainWindow::menuCapture(QMenuBar * menu_bar)
{
    QMenu *menuCapture = menu_bar->addMenu("Capture");

    QAction *actionScreenshot = menuCapture->addAction(tr("Screenshot"));
    QAction *actionMovie = menuCapture->addAction(tr("Movie"));

    connect(actionScreenshot, &QAction::triggered, this, &medMainWindow::captureScreenshot);
    connect(actionMovie, &QAction::triggered, this, &medMainWindow::captureVideo);

}

/**
 * @brief The Settings menu holds application settings such as start-up or data sources settings
 * 
 * @param menu_bar 
 */
void medMainWindow::menuSettings(QMenuBar * menu_bar)
{
    QMenu *menuSettings = menu_bar->addMenu("Settings");

    QAction *actionDataSources = menuSettings->addAction(tr("Data Sources"));
    connect(actionDataSources, &QAction::triggered, this, &medMainWindow::onShowDataSources);

    QAction *actionAreaSettings = menuSettings->addAction(tr("&Startup"));
    connect(actionAreaSettings, &QAction::triggered, this, &medMainWindow::onShowAreaSettings);
}

void medMainWindow::menuAbout(QMenuBar * menu_bar)
{
    // --- About menu
    QMenu *menuAbout = menu_bar->addMenu("Help");

    QAction *actionAbout = menuAbout->addAction(tr("About the application"));
    QAction *actionAuthors = menuAbout->addAction(tr("Au&thors"));
    QAction *actionReleaseNotes = menuAbout->addAction(tr("&Release Notes"));
    QAction *actionLicense = menuAbout->addAction(tr("&License"));
    menuAbout->addSeparator();
    QAction *actionHelp = menuAbout->addAction(tr("&Online Documentation"));

    connect(actionAbout, &QAction::triggered, this, &medMainWindow::onShowAbout);
    connect(actionAuthors, &QAction::triggered, this, &medMainWindow::onShowAuthors);
    connect(actionReleaseNotes, &QAction::triggered, this, &medMainWindow::onShowReleaseNotes);
    connect(actionLicense, &QAction::triggered, this, &medMainWindow::onShowLicense);
    connect(actionHelp, &QAction::triggered, this, &medMainWindow::onShowHelp);
}

void medMainWindow::menuNotif(QMenuBar * menu_bar)
{
    // --- Notif Action
    QMenu   *menuNotif = menu_bar->addMenu("Notifications");
    QAction *actionShowHideNotifs = menuNotif->addAction("Show / Hide");
    QAction *actionClearNotifs = menuNotif->addAction("Clear all");
    menuNotif->addSeparator();
    QMenu   *menuLog = menuNotif->addMenu("Logs");
    QAction *actionLog = menuLog->addAction(tr("Show"));
    QAction *actionPluginLogs = menuLog->addAction(tr("Plugins"));

    connect(actionShowHideNotifs, &QAction::triggered, this, &medMainWindow::toggleNotificationPanel);
    connect(actionClearNotifs, &QAction::triggered, medNotifSys::instance(), &medNotifSys::clear);
    connect(actionLog, &QAction::triggered, this, &medMainWindow::openLogDirectory);
    connect(actionPluginLogs, &QAction::triggered, this, &medMainWindow::onShowPluginLogs);
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
    else if (startupWorkspace == "Browse data")
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
    d->notifWindow->switchVisibility();
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











void medMainWindow::openFromSystem()
{
    //  get last directory opened in settings.
    QString path;
    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.restoreState(medSettingsManager::instance()->value("state", "openFromSystem").toByteArray());
    dialog.restoreGeometry(medSettingsManager::instance()->value("geometry", "openFromSystem").toByteArray());
    if (dialog.exec())
        path = dialog.selectedFiles().first();

    medSettingsManager::instance()->setValue("state", "openFromSystem", dialog.saveState());
    medSettingsManager::instance()->setValue("geometry", "openFromSystem", dialog.saveGeometry());

    if (!path.isEmpty())
    {
        open(path);
    }
}

void medMainWindow::openDicomFromSystem()
{
    //  get last directory opened in settings.
    QString path;
    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::Directory);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.restoreState(medSettingsManager::instance()->value("state", "openFromSystem").toByteArray());
    dialog.restoreGeometry(medSettingsManager::instance()->value("geometry", "openFromSystem").toByteArray());
    if (dialog.exec())
        path = dialog.selectedFiles().first();

    medSettingsManager::instance()->setValue("state", "openFromSystem", dialog.saveState());
    medSettingsManager::instance()->setValue("geometry", "openFromSystem", dialog.saveGeometry());

    if (!path.isEmpty())
    {
        open(path);
    }
}

void medMainWindow::setSourceVisibility(bool checked)
{
    QAction* currentAction = qobject_cast<QAction*>(sender());
    QString sourceInstanceId = currentAction->data().toString();
 
    emit medDataHub::instance()->sourceVisibled(sourceInstanceId, checked);
}

void medMainWindow::onShowBrowser()
{
    switchToBrowserArea();
}

void medMainWindow::onShowDataSources()
{
    QDialog *dialog = new QDialog(this);
    medSourcesLoaderPresenter presenter(medSourcesLoader::instance());
    auto wgt = presenter.buildWidget();
    auto layout = new QVBoxLayout();
    layout->addWidget(wgt);
    dialog->setLayout(layout);
    dialog->exec();
}

void medMainWindow::onShowAbout()
{
    QFile file(":ABOUT.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

/**
 * @brief Display the application authors list in a widget.
 * 
 */
void medMainWindow::onShowAuthors()
{
    QFile file(":authors.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QDialog dialog;
    dialog.setWindowTitle("Authors");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setText(text);
    textEdit->setReadOnly(true);
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(textEdit);

    dialog.resize(400, 300);
    dialog.exec();
}

/**
 * @brief Display the current release notes of the application in a widget.
 * 
 */
void medMainWindow::onShowReleaseNotes()
{
    QFile file(":RELEASE_NOTES.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QDialog dialog;
    dialog.setWindowTitle("Release Notes");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setText(text);
    textEdit->setReadOnly(true);
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(textEdit);

    dialog.resize(400, 300);
    dialog.exec();
}

/**
 * @brief Display the current license of the application in a widget.
 * 
 */
void medMainWindow::onShowLicense()
{
    QFile file(":LICENSE.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QDialog dialog;
    dialog.setWindowTitle("License");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setText(text);
    textEdit->setReadOnly(true);
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(textEdit);

    dialog.resize(400, 300);
    dialog.exec();
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

void medMainWindow::onSwitchToProcess()
{
    qDebug()<<"onSwithToProcess";
    QAction* currentAction = qobject_cast<QAction*>(sender());
    QStringList data = currentAction->data().toStringList();
    QString workspaceName = data[0];
    QString processIdentifier = data[1];
    QString processName = data[2];

    switchToWorkspaceArea();
    medWorkspaceFactory::Details* details = medWorkspaceFactory::instance()->workspaceDetailsFromId(workspaceName);

    if (details)
    {
        d->shortcutAccessWidget->updateSelected(workspaceName);

        if (!d->workspaceArea->setCurrentWorkspace(workspaceName))
        {
            QString message = QString("Cannot open workspace ") + details->name;
            medMessageController::instance()->showError(message, 3000);
            switchToHomepageArea();
        }
        else
        {
            auto * workspace = dynamic_cast<medSelectorWorkspace *>(d->workspaceArea->currentWorkspace());
            medSelectorToolBox * selectorTb = workspace->selectorToolBox();
            medComboBox *comboBox = selectorTb->comboBox();
            comboBox->setCurrentIndex(selectorTb->getIndexOfToolBox(processName));
            selectorTb->changeCurrentToolBox(processIdentifier);
        }
        // The View menu is dedicated to "view workspaces"
        enableMenuBarItem("View", true);

        this->hideShortcutAccess();
    }


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
    QDesktopServices::openUrl(QUrl("https://med.inria.fr/documentation"));
}

void medMainWindow::filterWSMenu(QString text)
{
    if (text.isEmpty())
    {
        for (auto action : d->wsActions)
        {
            action->setEnabled(true);
        }
        for (auto menu : d->wsMenuActionsMap.keys())
        {
            menu->setEnabled(true);
            for (auto action : d->wsMenuActionsMap[menu])
            {
                action->setEnabled(true);
            }
        }
    }
    else
    {
        auto textSplited = text.toLower().split(" ", QString::SkipEmptyParts);
        for (auto action : d->wsActions)
        {
            bool bActionVisible = false;

            QString id = action->data().toString().toLower();
            QString name = action->text().toLower();

            for (QString & keyword : textSplited)
            {
                bActionVisible = bActionVisible || id.contains(keyword) || name.contains(keyword);
            }

            action->setEnabled(bActionVisible);
        }

        for (auto menu : d->wsMenuActionsMap.keys())
        {
            bool bMenuVisible = false;
            QString menuName = menu->title().toLower();
            
            for (QString & keyword : textSplited)
            {
                bMenuVisible = bMenuVisible || menuName.contains(keyword);
            }

            for (auto action : d->wsMenuActionsMap[menu])
            {
                bool bActionVisible = false;

                QString id = action->data().toString().toLower();
                QString name = action->text().toLower();

                for (QString & keyword : textSplited)
                {
                    bActionVisible = bActionVisible || id.contains(keyword) || name.contains(keyword);
                }

                action->setEnabled(bActionVisible);

                bMenuVisible = bMenuVisible || bActionVisible;
            }

            menu->setEnabled(bMenuVisible);
        }
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
    auto fullscreenAction = getCornerAction("Fullscreen");
    if (full)
    {
        setFullscreenOn(fullscreenAction);
    }
    else
    {
        setFullscreenOff(fullscreenAction);
    }
}

void medMainWindow::toggleFullScreen()
{
    auto fullscreenAction = getCornerAction("Fullscreen");
    if (!this->isFullScreen())
    {
        setFullscreenOn(fullscreenAction);
    }
    else
    {
        setFullscreenOff(fullscreenAction);
    }
}

void medMainWindow::setFullscreenOn(QAction* fullscreenAction)
{
    fullscreenAction->setIcon(QIcon::fromTheme("fullscreen_off"));
    fullscreenAction->blockSignals(true);
    fullscreenAction->setChecked(true);
    fullscreenAction->blockSignals(false);
    this->showFullScreen();
}

void medMainWindow::setFullscreenOff(QAction* fullscreenAction)
{
    fullscreenAction->setIcon(QIcon::fromTheme("fullscreen_on"));
    fullscreenAction->blockSignals(true);
    fullscreenAction->setChecked(false);
    fullscreenAction->blockSignals(false);
    this->showNormal();
}

QAction* medMainWindow::getCornerAction(QString actionName)
{
    QAction* fullscreenAction;
    for(QAction *action : this->menuBar()->cornerWidget()->actions())
    {
        if (action->objectName() == actionName)
        {
            fullscreenAction = action;
        }
    }
    return fullscreenAction;
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

        d->shortcutAccessWidget->updateSelected("Browse data");
        if (d->shortcutAccessVisible)
        {
            this->hideShortcutAccess();
        }

        d->stack->setCurrentWidget(d->browserArea);

        // The View menu is dedicated to "view workspaces"
        enableMenuBarItem("View", false);
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

/**
 * @brief Run after application resize through resize buttons.
 * 
 * @param event 
 */
void medMainWindow::resizeEvent(QResizeEvent *event)
{
    d->notifWindow->windowGeometryUpdate();
}

void medMainWindow::adjustContainersSize()
{
    d->workspaceArea->currentWorkspace()->tabbedViewContainers()->adjustContainersSize();
}
