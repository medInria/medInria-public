/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medMainWindow.h>

#include <QtGui>

#include <medBrowserArea.h>
#include <medWorkspaceArea.h>
#include <medHomepageArea.h>

#include <medTabbedViewContainers.h>

#include <medSettingsManager.h>
#include <medSettingsEditor.h>

#include <medStatusBar.h>
#include <medQuickAccessMenu.h>
#include <medSaveModifiedDialog.h>
#include <medEmptyDbWarning.h>

#include <medDatabaseNonPersistentController.h>
#include <medDatabaseController.h>

#include <medJobManager.h>

#include <medWorkspaceFactory.h>
#include <medAbstractWorkspace.h>
#include <medVisualizationWorkspace.h>

#ifdef Q_OS_MAC
# define CONTROL_KEY "Meta"
#else
# define CONTROL_KEY "Ctrl"
#endif

//--------------------------------------------------------------------------
// medMainWindowStyle

class medMainWindowStyle : public QPlastiqueStyle
{
public:
    void drawPrimitive ( PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0 ) const
    {
        switch ( element )
        {
        case PE_FrameFocusRect:
            break;
        default:
            QPlastiqueStyle::drawPrimitive ( element, option, painter, widget );
            break;
        }
    }
};

//--------------------------------------------------------------------------
// medMainWindow

class medMainWindowPrivate
{
public:
    //  Interface elements.
    QWidget *currentArea;

    QStackedWidget*           stack;
    medBrowserArea*           browserArea;
    medWorkspaceArea*         workspaceArea;
    medHomepageArea*          homepageArea;
    medSettingsEditor*        settingsEditor;
    QHBoxLayout*              statusBarLayout;
    QWidget*                  rightEndButtons;
    medStatusBar*             statusBar;
    medQuickAccessPushButton* quickAccessButton;
    QToolButton*                quitButton;
    QToolButton*              fullscreenButton;
    QToolButton*              adjustSizeButton;
    QList<QString>            importUuids;
    medQuickAccessMenu * quickAccessWidget;
    bool controlPressed;

    medQuickAccessMenu *shortcutAccessWidget;
    bool shortcutAccessVisible;
    QShortcut * shortcutShortcut;

    QToolButton *screenshotButton;
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
    this->setMinimumHeight ( 600 );
    this->setMinimumWidth ( 800 );


    //  Setting up widgets

    d->settingsEditor = NULL;
    d->currentArea = NULL;

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

    //  Setup quick access menu
    d->quickAccessButton = new medQuickAccessPushButton ( this );
    d->quickAccessButton->setFocusPolicy ( Qt::NoFocus );
    d->quickAccessButton->setMinimumHeight(31);
    d->quickAccessButton->setIcon(QIcon(":medInria.ico"));
    d->quickAccessButton->setCursor(Qt::PointingHandCursor);
    d->quickAccessButton->setText(tr("Workspaces access menu"));
    connect(d->quickAccessButton, SIGNAL(clicked()), this, SLOT(toggleQuickAccessVisibility()));

    d->quickAccessWidget = new medQuickAccessMenu(true, this);
    d->quickAccessWidget->setFocusPolicy(Qt::ClickFocus);
    d->quickAccessWidget->hide();
    d->quickAccessWidget->setMinimumWidth(180);
    d->quickAccessWidget->move(QPoint(0, this->height() - d->quickAccessWidget->height() - 30));

    connect(d->quickAccessWidget, SIGNAL(menuHidden()), this, SLOT(hideQuickAccess()));
    connect(d->quickAccessWidget, SIGNAL(homepageSelected()), this, SLOT(switchToHomepageArea()));
    connect(d->quickAccessWidget, SIGNAL(browserSelected()), this, SLOT(switchToBrowserArea()));
    connect(d->quickAccessWidget, SIGNAL(workspaceSelected(QString)), this, SLOT(showWorkspace(QString)));

    d->shortcutAccessWidget = new medQuickAccessMenu( false, this );
    d->shortcutAccessWidget->setFocusPolicy(Qt::ClickFocus);
    d->shortcutAccessWidget->setProperty("pos", QPoint(0, -500));

    connect(d->shortcutAccessWidget, SIGNAL(menuHidden()), this, SLOT(hideShortcutAccess()));
    connect(d->shortcutAccessWidget, SIGNAL(homepageSelected()), this, SLOT(switchToHomepageArea()));
    connect(d->shortcutAccessWidget, SIGNAL(browserSelected()), this, SLOT(switchToBrowserArea()));
    connect(d->shortcutAccessWidget, SIGNAL(workspaceSelected(QString)), this, SLOT(showWorkspace(QString)));

    d->shortcutAccessVisible = false;
    d->controlPressed = false;

    //Add quit button
    QIcon quitIcon;
    quitIcon.addPixmap(QPixmap(":/icons/quit.png"), QIcon::Normal);
    d->quitButton = new QToolButton(this);
    d->quitButton->setIcon(quitIcon);
    d->quitButton->setObjectName("quitButton");
    connect(d->quitButton, SIGNAL( pressed()), this, SLOT (close()));
    d->quitButton->setToolTip(tr("Close medInria"));

    //  Setup Fullscreen Button
    QIcon fullscreenIcon ;
    fullscreenIcon.addPixmap(QPixmap(":icons/fullscreenExpand.png"),QIcon::Normal,QIcon::Off);
    fullscreenIcon.addPixmap(QPixmap(":icons/fullscreenReduce.png"),QIcon::Normal,QIcon::On);

    //  Setting up shortcuts
    //  we use a toolbutton, which has shorcuts,
    //  so we don't need the application shortcut anymore.
    d->fullscreenButton = new QToolButton(this);
    d->fullscreenButton->setIcon(fullscreenIcon);
    d->fullscreenButton->setCheckable(true);
    d->fullscreenButton->setChecked(false);
    d->fullscreenButton->setObjectName("fullScreenButton");
#if defined(Q_WS_MAC)
    d->fullscreenButton->setShortcut(Qt::ControlModifier + Qt::Key_F);
    d->fullscreenButton->setToolTip(tr("Switch FullScreen state (Cmd+f)"));
#else
    d->fullscreenButton->setShortcut(Qt::Key_F11);
    d->fullscreenButton->setToolTip(tr("Switch FullScreen state (F11)"));
#endif
    connect ( d->fullscreenButton, SIGNAL ( toggled(bool) ),
                       this, SLOT ( setFullScreen(bool) ) );

    QIcon cameraIcon;
    cameraIcon.addPixmap(QPixmap(":icons/camera.png"),QIcon::Normal);
    cameraIcon.addPixmap(QPixmap(":icons/camera_grey.png"),QIcon::Disabled);
    d->screenshotButton = new QToolButton(this);
    d->screenshotButton->setIcon(cameraIcon);
    d->screenshotButton->setObjectName("screenshotButton");
    d->screenshotButton->setShortcut(Qt::AltModifier + Qt::Key_S);
    d->screenshotButton->setToolTip(tr("Capture screenshot"));
    QObject::connect(d->screenshotButton, SIGNAL(clicked()), this, SLOT(captureScreenshot()));

    QIcon adjustIcon;
    adjustIcon.addPixmap(QPixmap(":icons/adjust_size.png"),QIcon::Normal);
    d->adjustSizeButton = new QToolButton(this);
    d->adjustSizeButton->setIcon(adjustIcon);
    d->adjustSizeButton->setObjectName("adjustSizeButton");
    d->adjustSizeButton->setShortcut(Qt::AltModifier + Qt::Key_A);
    d->adjustSizeButton->setToolTip(tr("Adjust containers size"));
    QObject::connect(d->adjustSizeButton, SIGNAL(clicked()), this, SLOT(adjustContainersSize()));


    //  QuitMessage and rightEndButtons will switch hidden and shown statuses.
    d->rightEndButtons = new QWidget(this);
    QHBoxLayout * rightEndButtonsLayout = new QHBoxLayout(d->rightEndButtons);
    rightEndButtonsLayout->setContentsMargins ( 5, 0, 5, 0 );
    rightEndButtonsLayout->setSpacing ( 5 );
    rightEndButtonsLayout->addWidget( d->adjustSizeButton );
    rightEndButtonsLayout->addWidget( d->screenshotButton );
    rightEndButtonsLayout->addWidget( d->fullscreenButton );
    rightEndButtonsLayout->addWidget( d->quitButton );

    //  Setting up status bar
    d->statusBarLayout = new QHBoxLayout;
    d->statusBarLayout->setMargin(0);
    d->statusBarLayout->setSpacing(5);
    d->statusBarLayout->addWidget ( d->quickAccessButton );
    d->statusBarLayout->addStretch();
    d->statusBarLayout->addWidget( d->rightEndButtons );

    //  Create a container widget for the status bar
    QWidget * statusBarWidget = new QWidget ( this );
    statusBarWidget->setContentsMargins ( 5, 0, 5, 0 );
    statusBarWidget->setLayout ( d->statusBarLayout );

    //  Setup status bar
    d->statusBar = new medStatusBar(this);
    d->statusBar->setStatusBarLayout(d->statusBarLayout);
    d->statusBar->setSizeGripEnabled(false);
    d->statusBar->setContentsMargins(5, 0, 5, 0);
    d->statusBar->setFixedHeight(31);
    d->statusBar->addPermanentWidget(statusBarWidget, 1);

    this->setStatusBar(d->statusBar);
    connect(d->statusBar, SIGNAL(initializeAvailableSpace()), this,  SLOT(availableSpaceOnStatusBar()));

    //  Init homepage with workspace buttons
    d->homepageArea->initPage();
    connect(d->homepageArea, SIGNAL(showBrowser()), this, SLOT(switchToBrowserArea()));
    connect(d->homepageArea, SIGNAL(showWorkspace(QString)), this, SLOT(showWorkspace(QString)));

    this->setCentralWidget ( d->stack );
    this->setWindowTitle("medInria");
    //  Connect the messageController with the status for notification messages management
    connect(medMessageController::instance(), SIGNAL(addMessage(medMessage*)), d->statusBar, SLOT(addMessage(medMessage*)));
    connect(medMessageController::instance(), SIGNAL(removeMessage(medMessage*)), d->statusBar, SLOT(removeMessage(medMessage*)));

    d->shortcutShortcut = new QShortcut(QKeySequence(tr(CONTROL_KEY "+Space")),
                                                     this,
                                                     SLOT(showShortcutAccess()),
                                                     SLOT(showShortcutAccess()),
                                                     Qt::ApplicationShortcut);
    this->restoreSettings();

    // medQuickAccessMenu loads default workspace to open, so we can switch to it now
    d->quickAccessWidget->switchToCurrentlySelected();
}

medMainWindow::~medMainWindow()
{
    delete d;
    d = NULL;
}

void medMainWindow::mousePressEvent ( QMouseEvent* event )
{
    QWidget::mousePressEvent ( event );
    this->hideQuickAccess();
    this->hideShortcutAccess();
}

void medMainWindow::restoreSettings()
{
    medSettingsManager * mnger = medSettingsManager::instance();

    this->restoreState(mnger->value("medMainWindow", "state").toByteArray());
    this->restoreGeometry(mnger->value("medMainWindow", "geometry").toByteArray());
}

void medMainWindow::saveSettings() {
    if(!this->isFullScreen())
    {
        medSettingsManager * mnger = medSettingsManager::instance();
        mnger->setValue("medMainWindow", "state", this->saveState());
        mnger->setValue("medMainWindow", "geometry", this->saveGeometry());
    }
}

/**
 * If one tries to launch a new instance of medInria, the QtSingleApplication bypass it and receive 
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
    d->expectedUuids.append(uuid);
    connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)),
            this, SLOT(open_waitForImportedSignal(medDataIndex,QUuid)));
    while( d->expectedUuids.contains(uuid)) {
        loop.processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}


void medMainWindow::open_waitForImportedSignal(medDataIndex index, QUuid uuid)
{
    if(d->expectedUuids.contains(uuid)) {
        d->expectedUuids.removeAll(uuid);
        disconnect(medDataManager::instance(),SIGNAL(dataImported(medDataIndex,QUuid)),
                   this,SLOT(open_waitForImportedSignal(medDataIndex,QUuid)));
        if (index.isValid()) {
            this->showWorkspace(medVisualizationWorkspace::staticIdentifier());
            d->workspaceArea->currentWorkspace()->open(index);
        }
    }
}

void medMainWindow::resizeEvent ( QResizeEvent* event )
{
    QWidget::resizeEvent ( event );
    d->quickAccessWidget->move(QPoint (0, this->height() - d->quickAccessWidget->height() - 30 ));
    this->hideQuickAccess();
}

//TODO hide it, it is only usefull for immersive romm - RDE
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
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save screenshot as"),
                                                    QDir::home().absolutePath(),
                                                    QString(), 0, QFileDialog::HideNameFilterDetails);

    QByteArray format = fileName.right(fileName.lastIndexOf('.')).toUpper().toAscii();
    if ( ! QImageWriter::supportedImageFormats().contains(format) )
        format = "PNG";

    QImage transparentImage = screenshot.toImage();
    QImage outImage(transparentImage.size(), QImage::Format_RGB32);
    outImage.fill(QColor(Qt::black).rgb());

    QPainter painter(&outImage);
    painter.drawImage(0,0,transparentImage);
    outImage.save(fileName, format.constData());
}

void medMainWindow::showFullScreen()
{
    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(true);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showFullScreen();
    this->setAcceptDrops(true);
}

void medMainWindow::showNormal()
{
    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(false);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showNormal();
}

void medMainWindow::showMaximized()
{
    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(false);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showMaximized();
}

QWidget* medMainWindow::currentArea() const
{
    return d->currentArea;
}

void medMainWindow::switchToHomepageArea()
{
    if(d->currentArea == d->homepageArea)
        return;

    d->currentArea = d->homepageArea;

    d->shortcutAccessWidget->updateSelected("Homepage");
    d->quickAccessWidget->updateSelected("Homepage");
    d->quickAccessButton->setText(tr("Workspaces access menu"));
    d->quickAccessButton->setMinimumWidth(170);
    if (d->quickAccessWidget->isVisible())
        this->hideQuickAccess();

    if (d->shortcutAccessVisible)
        this->hideShortcutAccess();

    d->stack->setCurrentWidget(d->homepageArea);
    d->homepageArea->onShowInfo();

    d->screenshotButton->setEnabled(false);
    d->adjustSizeButton->setEnabled(false);

    if (d->homepageArea->getAnimation())
        d->homepageArea->getAnimation()->start();
}

void medMainWindow::switchToBrowserArea()
{
    if(d->currentArea == d->browserArea)
        return;

    d->currentArea = d->browserArea;

    d->shortcutAccessWidget->updateSelected("Browser");
    d->quickAccessWidget->updateSelected("Browser");

    d->quickAccessButton->setText(tr("Workspace: Browser"));
    d->quickAccessButton->setMinimumWidth(170);
    if (d->quickAccessWidget->isVisible())
        this->hideQuickAccess();

    if (d->shortcutAccessVisible)
        this->hideShortcutAccess();

    d->screenshotButton->setEnabled(false);
    d->adjustSizeButton->setEnabled(false);
    d->stack->setCurrentWidget(d->browserArea);
}

void medMainWindow::switchToWorkspaceArea()
{

    if(d->currentArea == d->workspaceArea)
        return;

    d->currentArea = d->workspaceArea;

    this->hideQuickAccess();
    this->hideShortcutAccess();

    d->screenshotButton->setEnabled(true);
    d->adjustSizeButton->setEnabled(true);
    d->stack->setCurrentWidget(d->workspaceArea);

    // Dialog window to recall users if database is empty
    // but only if the warning is enabled in medSettings
    bool showWarning = medSettingsManager::instance()->value(
                "system",
                "showEmptyDbWarning",
                QVariant(true)).toBool();
    if ( showWarning )
    {
        QList<medDataIndex> indexes = medDatabaseNonPersistentController::instance()->availableItems();
        QList<medDataIndex> patients = medDatabaseController::instance()->patients();
        if( indexes.isEmpty() )
            if( patients.isEmpty())
            {
                medEmptyDbWarning* msgBox = new medEmptyDbWarning(this);
                msgBox->exec();
            }
    }
}

void medMainWindow::showWorkspace(QString workspace)
{
    d->quickAccessButton->setMinimumWidth(170);

    this->switchToWorkspaceArea();
    medWorkspaceFactory::Details* details = medWorkspaceFactory::instance()->workspaceDetailsFromId(workspace);

    d->quickAccessButton->setText(tr("Workspace: ") + details->name);
    d->shortcutAccessWidget->updateSelected(workspace);
    d->quickAccessWidget->updateSelected(workspace);
    d->workspaceArea->setCurrentWorkspace(workspace);

    this->hideQuickAccess();
    this->hideShortcutAccess();
}

/**
 * Slot to show bottom left menu
 */
void medMainWindow::toggleQuickAccessVisibility()
{
    // Ensure one can toggle menu appearance/disapperance when it is clicked twice
    if (d->quickAccessWidget->isVisible())
    {
        this->hideQuickAccess();
        return;
    }

    d->quickAccessWidget->reset(false);
    d->quickAccessWidget->setFocus();
    d->quickAccessWidget->setMouseTracking(true);
    d->quickAccessWidget->show();
}

/**
 * Slot to hide bottom left menu
 */
void medMainWindow::hideQuickAccess()
{
    d->quickAccessWidget->hide();
    d->quickAccessWidget->setMouseTracking(false);
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
    if (!d->shortcutAccessVisible)
        return;

    d->shortcutAccessWidget->releaseKeyboard();
    d->shortcutAccessWidget->setMouseTracking(false);
    d->shortcutAccessVisible = false;
    d->shortcutAccessWidget->setProperty("pos", QPoint ( 0 , -500 ));
    d->shortcutAccessWidget->hide();
    this->activateWindow();
}

int medMainWindow::saveModified( void )
{
    QList<medDataIndex> indexes = medDatabaseNonPersistentController::instance()->availableItems();

    if(indexes.isEmpty())
        return QDialog::Accepted;

    medSaveModifiedDialog *saveDialog = new medSaveModifiedDialog(this);
    saveDialog->show();
    saveDialog->exec();

    return saveDialog->result();
}

void medMainWindow::availableSpaceOnStatusBar()
{
    QPoint workspaceButton_topRight = d->quickAccessButton->mapTo(d->statusBar, d->quickAccessButton->rect().topRight());
    QPoint screenshotButton_topLeft = d->screenshotButton->mapTo(d->statusBar, d->screenshotButton->rect().topLeft());
    //Available space = space between the spacing after workspace button and the spacing before screenshot button
    int space = (screenshotButton_topLeft.x()-d->statusBarLayout->spacing()) -  (workspaceButton_topRight.x()+d->statusBarLayout->spacing());
    d->statusBar->setAvailableSpace(space);
}

void medMainWindow::closeEvent(QCloseEvent *event)
{
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
            medJobManager::instance()->dispatchGlobalCancelEvent();
            QThreadPool::globalInstance()->waitForDone();
        }
        else
        {
            // just hide the window and wait
            QThreadPool::globalInstance()->waitForDone();
        }
    }
    if(this->saveModified() != QDialog::Accepted)
    {
        event->ignore();
        return;
    }
    this->saveSettings();
    event->accept();
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
