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
 
#include <dtkComposerWidget.h>

#include <medBrowserArea.h>
#include <medComposerArea.h>
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

    QStackedWidget*           stack;
    medComposerArea*          composerArea;
    medBrowserArea*           browserArea;
    medWorkspaceArea*         workspaceArea;
    medHomepageArea*          homepageArea;
    QHBoxLayout*              statusBarLayout;
    medStatusBar*             statusBar;
    
    medQuickAccessMenu *shortcutAccessWidget;
    bool shortcutAccessVisible;
    bool closeEventProcessed;
    QShortcut * shortcutShortcut;

    QList<QString> importUuids;
    QList<QUuid> expectedUuids;
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

    //Composer
    d->composerArea = new medComposerArea(this);
    d->composerArea->setObjectName("medComposerArea");

    //  Stack
    d->stack = new QStackedWidget(this);
    d->stack->addWidget(d->homepageArea);
    d->stack->addWidget(d->browserArea);
    d->stack->addWidget(d->workspaceArea);
    d->stack->addWidget(d->composerArea);

    // Shortcut to workspaces through CTRL+Space
    d->shortcutAccessWidget = new medQuickAccessMenu( false, this );
    d->shortcutAccessWidget->setFocusPolicy(Qt::ClickFocus);
    d->shortcutAccessWidget->setProperty("pos", QPoint(0, -500));

    connect(d->shortcutAccessWidget, SIGNAL(menuHidden()), this, SLOT(hideShortcutAccess()));
    connect(d->shortcutAccessWidget, SIGNAL(homepageSelected()), this, SLOT(switchToHomepageArea()));
    connect(d->shortcutAccessWidget, SIGNAL(browserSelected()), this, SLOT(switchToBrowserArea()));
    connect(d->shortcutAccessWidget, SIGNAL(composerSelected()), this, SLOT(switchToComposerArea()));
    connect(d->shortcutAccessWidget, SIGNAL(workspaceSelected(QString)), this, SLOT(showWorkspace(QString)));

    d->shortcutAccessVisible = false;

    //  Setting up status bar
    d->statusBarLayout = new QHBoxLayout;
    d->statusBarLayout->setMargin(0);
    d->statusBarLayout->setSpacing(5);
    d->statusBarLayout->addStretch();

    //  Create a container widget for the status bar
    QWidget * statusBarWidget = new QWidget ( this );
    statusBarWidget->setContentsMargins ( 5, 0, 0, 0 );
    statusBarWidget->setLayout ( d->statusBarLayout );

    //  Setup status bar
    d->statusBar = new medStatusBar(this);
    d->statusBar->setStatusBarLayout(d->statusBarLayout);
    d->statusBar->setSizeGripEnabled(false);
    d->statusBar->setContentsMargins(5, 0, 0, 0);
    d->statusBar->setFixedHeight(31);
    d->statusBar->addPermanentWidget(statusBarWidget, 1);

    this->setStatusBar(d->statusBar);
    connect(d->statusBar, SIGNAL(initializeAvailableSpace()), this,  SLOT(availableSpaceOnStatusBar()));

    //  Init homepage with workspace buttons
    d->homepageArea->initPage();
    connect(d->homepageArea, SIGNAL(showBrowser()), this, SLOT(switchToBrowserArea()));
    connect(d->homepageArea, SIGNAL(showWorkspace(QString)), this, SLOT(showWorkspace(QString)));
    connect(d->homepageArea, SIGNAL(showComposer()), this, SLOT(showComposer()));

    this->setCentralWidget ( d->stack );
    this->setWindowTitle(qApp->applicationName());

    //  Connect the messageController with the status for notification messages management
    connect(medMessageController::instance(), SIGNAL(addMessage(medMessage*)), d->statusBar, SLOT(addMessage(medMessage*)));
    connect(medMessageController::instance(), SIGNAL(removeMessage(medMessage*)), d->statusBar, SLOT(removeMessage(medMessage*)));

    d->shortcutShortcut = new QShortcut(QKeySequence(tr(CONTROL_KEY "+Space")),
                                                     this,
                                                     SLOT(showShortcutAccess()),
                                                     SLOT(showShortcutAccess()),
                                                     Qt::ApplicationShortcut);
    this->restoreSettings();

    // Switch to the default workspace at application start
    switchToDefaultWorkSpace();

    this->setAcceptDrops(true);
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
    else if (startupWorkspace == "Composer")
    {
        switchToComposerArea();
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

    case medMainWindow::Composer:
        this->switchToComposerArea();
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
    return d->notifButton;
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

void medMainWindow::switchToComposerArea()
{
    if(d->currentArea != d->composerArea)
    {
        d->currentArea = d->composerArea;

        d->shortcutAccessWidget->updateSelected("Composer");

        if (d->shortcutAccessVisible)
        {
            this->hideShortcutAccess();
        }

        d->stack->setCurrentWidget(d->composerArea);

        // The View menu is dedicated to "view workspaces"
        enableMenuBarItem("View", false);
    }
}

void medMainWindow::showWorkspace(QString workspace)
{
    switchToWorkspaceArea();
    medWorkspaceFactory::Details* details = medWorkspaceFactory::instance()->workspaceDetailsFromId(workspace);

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

void medMainWindow::showComposer()
{
    this->switchToComposerArea();
    this->hideShortcutAccess();
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

void medMainWindow::availableSpaceOnStatusBar()
{
    d->statusBar->setAvailableSpace(width());
}

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
