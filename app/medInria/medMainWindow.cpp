/* medMainWindow.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:48:07 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:25:37 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 503
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medHomepageButton.h"
#include "medBrowserArea.h"
#include "medMainWindow.h"
#include "medViewerArea.h"
#include "medHomepageArea.h"

#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkScript/dtkScriptInterpreter.h>
#include <dtkScript/dtkScriptInterpreterPool.h>
#include <dtkScript/dtkScriptInterpreterPython.h>
#include <dtkScript/dtkScriptInterpreterTcl.h>

#include <dtkGui/dtkSpacer.h>

#include <medMessageController.h>
#include <medStatusBar.h>
#include <medSettingsManager.h>
#include <medDbControllerFactory.h>
#include <medJobManager.h>
#include <medDataManager.h>

#include <medButton.h>
#include <medQuickAccessMenu.h>
#include <medWorkspaceShifter.h>

#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseModel.h>
#include <medDatabaseItem.h>

#include <medViewerWorkspace.h>
#include <medViewerWorkspaceFactory.h>
#include <medSettingsWidgetFactory.h>
#include <medSystemSettingsWidget.h>
#include <medStartupSettingsWidget.h>
#include <medDatabaseSettingsWidget.h>
#include <medInteractionSettingsWidget.h>
#include <medSettingsEditor.h>
#include <medEmptyDbWarning.h>

#include "medSeedPointAnnotationData.h"

#include "medViewerWorkspaceVisualization.h"
#include "medViewerWorkspaceRegistration.h"
#include "medViewerWorkspaceDiffusion.h"
#include "medViewerWorkspaceFiltering.h"
#include "medViewerWorkspaceSegmentation.h"

#include "medSaveModifiedDialog.h"

#include <QtGui>

// Simple new function used for factories.
namespace  {
    template< class T >
    dtkAbstractData * dtkAbstractDataCreateFunc() { return new T; }
}

// /////////////////////////////////////////////////////////////////
// medMainWindowStyle
// /////////////////////////////////////////////////////////////////

class medMainWindowStyle: public QPlastiqueStyle {
public:

    void drawPrimitive(PrimitiveElement element,const QStyleOption *option,QPainter *painter,const QWidget *widget = 0) const {

        switch (element) {
            case PE_FrameFocusRect:
                break;
            default:
                QPlastiqueStyle::drawPrimitive(element,option,painter,widget);
                break;
        }
    }
};

// /////////////////////////////////////////////////////////////////
// medMainWindow
// /////////////////////////////////////////////////////////////////

class medMainWindowPrivate {
public:

    //  Interface elements.

    QStackedWidget*           stack;
    medBrowserArea*           browserArea;
    medViewerArea*            viewerArea;
    medHomepageArea*          homepageArea;
    medSettingsEditor*        settingsEditor;
    QHBoxLayout*              statusBarLayout;
    QWidget*                  rightEndButtons;
    medStatusBar*             statusBar;
    medQuickAccessMenu*       quickAccessWidget;
    medQuickAccessPushButton* quickAccessButton;
    QPropertyAnimation*       quickAccessAnimation;
    QWidget*                  quitMessage;
    medButton*                quitButton;
    QToolButton*              fullscreenButton;

    QList<QString>            importUuids;
    bool                      quickAccessVisible;
};

#if defined(HAVE_SWIG) && defined(HAVE_PYTHON)
extern "C" int init_core();               // -- Initialization core layer python wrapped functions
#endif

#if defined(HAVE_SWIG) && defined(HAVE_TCL)
extern "C" int Core_Init(Tcl_Interp *interp); // -- Initialization core layer tcl    wrapped functions
#endif

medMainWindow::medMainWindow(QWidget *parent): QMainWindow(parent),d(new medMainWindowPrivate) {

    //  Etch-disabled-text stylesheet property was deprecated.
    //  This is the only way I found to avoid the label's text look like etched when disabled
    //  also not puting the opacity to 0 works very well, except for tooltips
    //  but only tooltips in a QGroupBox, others are fine...
    //  Solution: put a transparent color to the etching palette (Light).

    QPalette p = QApplication::palette();
    p.setColor(QPalette::Disabled,QPalette::Light,QColor(100,100,100,0));
    QApplication::setPalette(p);

    //  To avoid strange behaviours with the homepageshifter

    this->setMinimumHeight(600);
    this->setMinimumWidth(800);

    //  Register controller, workspaces etc

    this->registerToFactories();

    //  Setting up database connection

    if (!medDatabaseController::instance()->createConnection())
        qDebug() << "Unable to create a connection to the database";

    connect(medDatabaseNonPersistentController::instance(),SIGNAL(updated(const medDataIndex &,const QString&)),
            this,SLOT(onOpenFile(const medDataIndex&,const QString&)));

    //  Setting up widgets

    d->settingsEditor = NULL;

    //  Browser area.

    d->browserArea = new medBrowserArea(this);
    d->browserArea->setObjectName("Browser");
    connect(d->browserArea,SIGNAL(open(const QString&)),this,SLOT(open(const QString&)));
    connect(d->browserArea,SIGNAL(load(const QString&)),this,SLOT(load(const QString&)));
    connect(d->browserArea,SIGNAL(open(const medDataIndex&)),this,SLOT(open(const medDataIndex&)));

    //  Viewer area.

    d->viewerArea = new medViewerArea(this);
    d->viewerArea->setObjectName("Viewer");

    //  Home page

    d->homepageArea = new medHomepageArea(this);
    d->homepageArea->setObjectName("Homepage");

    //  Stack

    d->stack = new QStackedWidget(this);
    d->stack->addWidget(d->homepageArea);
    d->stack->addWidget(d->browserArea);
    d->stack->addWidget(d->viewerArea);

    //  Setup quick access menu

    d->quickAccessButton = new medQuickAccessPushButton(this);
    d->quickAccessButton->setFocusPolicy(Qt::NoFocus);
    d->quickAccessButton->setMinimumHeight(31);
    d->quickAccessButton->setStyleSheet("border: 0px;");
    d->quickAccessButton->setIcon(QIcon(":medInria.ico"));
    d->quickAccessButton->setCursor(Qt::PointingHandCursor);
    d->quickAccessButton->setText(tr("Workspaces access menu"));
    connect(d->quickAccessButton,SIGNAL(clicked()),this,SLOT(onShowQuickAccess()));

    d->quickAccessWidget = new medQuickAccessMenu(this);
    d->quickAccessWidget->setFocusPolicy(Qt::ClickFocus);
    d->quickAccessWidget->setProperty("pos",QPoint(0,-500));
    d->quickAccessWidget->setMinimumWidth(180);
    connect(d->quickAccessWidget,SIGNAL(hideMenu()),this,SLOT(onHideQuickAccess()));

    d->quickAccessVisible = false;
    d->quickAccessAnimation = new QPropertyAnimation(d->quickAccessWidget,"pos",this);

    //  Add quit button

    d->quitButton = new medButton(this,":/icons/quit.png",tr("Quit Application"));
    connect(d->quitButton,SIGNAL(triggered()),this,SLOT(onQuit()));
    connect(d->quitButton,SIGNAL(triggered()),this,SLOT(onSaveModified()));
    d->quitButton->setMaximumWidth(31);
    d->quitButton->setToolTip(tr("Close medInria"));

    //  Setup quit message

    d->quitMessage = new QWidget(this);
    QHBoxLayout * quitLayout = new QHBoxLayout;
    QLabel *icon = new QLabel(this);
    icon->setMinimumHeight(30);
    icon->setPixmap(QPixmap(":/icons/information.png"));
    QLabel *info = new QLabel(this);
    info->setMinimumHeight(30);
    info->setText(tr("Are you sure you want to quit ?"));
    QPushButton *ok_button = new QPushButton(tr("Yes"),this);
    ok_button->setFocusPolicy(Qt::NoFocus);
    QObject::connect(ok_button,SIGNAL(clicked()),this,SLOT(close()));
    QPushButton *no_button = new QPushButton(tr("No"),this);
    no_button->setFocusPolicy(Qt::NoFocus);
    QObject::connect(no_button,SIGNAL(clicked()),this,SLOT(onNoQuit()));

    quitLayout->setContentsMargins(5,0,5,0);
    quitLayout->setSpacing(5);
    quitLayout->addWidget(icon);
    quitLayout->addWidget(info);
    quitLayout->addWidget(ok_button);
    quitLayout->addWidget(no_button);

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

    QObject::connect(d->fullscreenButton,SIGNAL(toggled(bool)),this,SLOT(setFullScreen(bool)));

    d->quitMessage->setLayout(quitLayout);

    //  QuitMessage and rightEndButtons will switch hidden and shown statuses.

    d->rightEndButtons = new QWidget(this);
    QHBoxLayout * rightEndButtonsLayout = new QHBoxLayout(d->rightEndButtons);
    rightEndButtonsLayout->setContentsMargins(5,0,5,0);
    rightEndButtonsLayout->setSpacing(5);
    rightEndButtonsLayout->addWidget(d->fullscreenButton);
    rightEndButtonsLayout->addWidget(d->quitButton);

    //  Setting up status bar

    d->statusBarLayout = new QHBoxLayout;
    d->statusBarLayout->setMargin(0);
    d->statusBarLayout->setSpacing(5);

    d->statusBarLayout->addWidget(d->quickAccessButton);
    d->statusBarLayout->addStretch();
    d->statusBarLayout->addWidget(d->quitMessage);
    d->statusBarLayout->addWidget(d->rightEndButtons);

    d->quitMessage->hide();

    //  Create a container widget for the status bar

    QWidget * statusBarWidget = new QWidget(this);
    statusBarWidget->setContentsMargins(5,0,5,0);
    statusBarWidget->setLayout(d->statusBarLayout);

    //  Setup status bar

    d->statusBar = new medStatusBar(this);
    d->statusBar->setStatusBarLayout(d->statusBarLayout);
    d->statusBar->setSizeGripEnabled(false);
    d->statusBar->setContentsMargins(5,0,5,0);
    d->statusBar->setFixedHeight(31);
    d->statusBar->addPermanentWidget(statusBarWidget,1);

    this->setStatusBar(d->statusBar);

//     this->statusBar()->setSizeGripEnabled(false);
//     this->statusBar()->setContentsMargins(5,0,5,0);
//     this->statusBar()->setFixedHeight(31);
//     this->statusBar()->addPermanentWidget(statusBarWidget,1);

    this->readSettings();

    //  Init homepage with workspace buttons

    d->homepageArea->initPage();
    QObject::connect(d->homepageArea,SIGNAL(showBrowser()),this,SLOT(switchToBrowserArea()));
    QObject::connect(d->homepageArea,SIGNAL(showWorkspace(QString)),this,SLOT(onShowWorkspace(QString)));
    QObject::connect(d->homepageArea,SIGNAL(showSettings()),this,SLOT(onEditSettings()));

    //  Add workspace button to the quick access menu
    this->updateQuickAccessMenu();

    this->setCentralWidget(d->stack);

    //  Now use the Qt preferred method by setting the Application style instead.
    //  The ownership of the style object is not transferred.
    //  this->setStyle(new QPlastiqueStyle());

    this->setWindowTitle("medInria");

    //  Connect the messageController with the status for notification messages management

    QObject::connect(medMessageController::instance(),SIGNAL(addMessage(QWidget*)),d->statusBar,SLOT(addMessage(QWidget*)));
    QObject::connect(medMessageController::instance(),SIGNAL(removeMessage(QWidget*)),d->statusBar,SLOT(removeMessage(QWidget*)));

    d->viewerArea->setupWorkspace("Visualization");

    connect(qApp,SIGNAL(aboutToQuit()),this,SLOT(close()));

    //  If directly opening the file was mandated on the command line, ask for it now.
}

medMainWindow::~medMainWindow() {
    delete d;
    d = NULL;
}

void medMainWindow::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    this->onHideQuickAccess();
}

void medMainWindow::readSettings() {

    //  If the user configured a default area we need to show it

    medSettingsManager* mnger = medSettingsManager::instance();

    //  If nothing is configured then Browser is the default area

    const int areaIndex = mnger->value("startup","default_starting_area",0).toInt();
    qDebug() << "areaIndex" << areaIndex;

    switchToArea(areaIndex);

    //  Restore size

    if (!this->isFullScreen()) {
        QPoint pos = mnger->value("application","pos",QPoint(200,200)).toPoint();
        QSize size = mnger->value("application","size",QSize(600,400)).toSize();
        move(pos);
        resize(size);
    }
}

void medMainWindow::writeSettings() {
    // if the app is in full screen mode we do not want to save the pos and size
    // as there is a setting that defines either the user wants to open the app in FS or not
    // so, if he/she chose not to and quit the app while in FS mode, we skip the settings saving
    if (!this->isFullScreen()) {
        medSettingsManager::instance()->setValue("application","pos",pos());
        medSettingsManager::instance()->setValue("application","size",size());
    }
}

void medMainWindow::setStartup(const int areaIndex,const QString& filename) {
    switchToArea(areaIndex);
    qDebug() << "HERE" << filename;
    open(filename);
}

void medMainWindow::switchToArea(const int areaIndex) {
    switch (areaIndex) {
        case 0:
            this->switchToHomepageArea();
            break;

        case 1:
            this->switchToBrowserArea();
            break;

        case 2: {
            this->switchToViewerArea();
            break;
        }

        default:
            this->switchToHomepageArea();
            break;
        }

}

void medMainWindow::updateQuickAccessMenu()
{
     QHash<QString,medViewerWorkspaceDetails*> workspaceDetails =
             medViewerWorkspaceFactory::instance()->workspaceDetails();
    QVBoxLayout * workspaceButtonsLayout = new QVBoxLayout;
    workspaceButtonsLayout->setMargin(0);
    workspaceButtonsLayout->setSpacing(0);

    //Setup quick access menu title
    QLabel * workspaceLabel = new QLabel(tr("<b>Switch to workspaces</b>"));
    workspaceLabel->setMaximumWidth(300);
    workspaceLabel->setFixedHeight(25);
    workspaceLabel->setAlignment(Qt::AlignHCenter);
    workspaceLabel->setTextFormat(Qt::RichText);
    //It's more easy to set the stylesheet here than in the qss file
    workspaceLabel->setStyleSheet("border-image: url(:/pixmaps/toolbox-header.png) 16 16 0 16 repeat-x;\
                                       border-left-width: 0px;\
                                       border-right-width: 0px;\
                                       border-top-width: 8px;\
                                       border-bottom-width: 0px;");
    workspaceButtonsLayout->addWidget(workspaceLabel);

    //Setup homepage access button
    medHomepagePushButton * homeButton = new medHomepagePushButton(this);
    homeButton->setText("Home");
    homeButton->setIcon(QIcon(":icons/home.png"));
    homeButton->setFixedHeight(40);
    homeButton->setMaximumWidth(250);
    homeButton->setMinimumWidth(250);
    homeButton->setStyleSheet("border: 0px;");
    homeButton->setFocusPolicy(Qt::NoFocus);
    homeButton->setCursor(Qt::PointingHandCursor);
    workspaceButtonsLayout->addWidget(homeButton);
    QObject::connect(homeButton,SIGNAL(clicked()),this,SLOT(switchToHomepageArea()));

    //Setup browser access button
    medHomepagePushButton * browserButton = new medHomepagePushButton(this);
    browserButton->setCursor(Qt::PointingHandCursor);
    browserButton->setStyleSheet("border: 0px;");
    browserButton->setIcon(QIcon(":/icons/folder.png"));
    browserButton->setText("Browser");
    browserButton->setFixedHeight(40);
    browserButton->setMaximumWidth(250);
    browserButton->setMinimumWidth(250);
    browserButton->setFocusPolicy(Qt::NoFocus);
    workspaceButtonsLayout->addWidget(browserButton);
    QObject::connect(browserButton,SIGNAL(clicked()),this,SLOT(switchToBrowserArea()));

    //  Dynamically setup workspaces access button

    foreach(QString id,workspaceDetails.keys()) {
        medHomepagePushButton * button = new medHomepagePushButton(this);
        medViewerWorkspaceDetails* detail = workspaceDetails.value(id);
        button->setText(detail->name);
        button->setFocusPolicy(Qt::NoFocus);
        button->setCursor(Qt::PointingHandCursor);
        button->setStyleSheet("border: 0px;");
        button->setFixedHeight(40);
        button->setMaximumWidth(250);
        button->setMinimumWidth(250);;
        button->setToolTip(detail->description);
        button->setIdentifier(id);
        workspaceButtonsLayout->addWidget(button);
        QObject::connect(button,SIGNAL(clicked(QString)),this,SLOT(onShowWorkspace(QString)));
    }
    workspaceButtonsLayout->addStretch();
    d->quickAccessAnimation->setEndValue(QPoint(0,this->height() - d->quickAccessWidget->height() - 30));
    d->quickAccessWidget->setMinimumHeight(20 + 40 *(2 + workspaceDetails.size()));
    d->quickAccessWidget->setLayout(workspaceButtonsLayout);
}

void medMainWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    d->quickAccessWidget->setProperty("pos",QPoint(0,this->height() - 30));
    this->onHideQuickAccess();
}


void medMainWindow::setWallScreen(const bool full) {
    if (full) {
        //  Very, very ad hoc.... Probably needs to be revisited or remove from non-INRIA versions.
        this->move(0,-30);
        this->resize(3528,1200);
    } else {
        this->showNormal();
    }
}

void medMainWindow::setFullScreen(const bool full) {
    if (full)
        this->showFullScreen();
    else
        this->showNormal();
}

void medMainWindow::switchFullScreen() {
    setFullScreen(!this->isFullScreen());
}

void medMainWindow::showFullScreen() {

    //  Update the icon, but do not trigger too many events

    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(true);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showFullScreen();
}

void medMainWindow::showNormal() {

    //  Update the icon, but do not trigger too many events

    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(false);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showNormal();
}

void medMainWindow::showMaximized() {

    //  Update the icon, but do not trigger too many events

    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(false);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showMaximized();
}

void medMainWindow::switchToHomepageArea() {

    d->quickAccessButton->setText(tr("Workspaces access menu"));
    d->quickAccessButton->setMinimumWidth(170);
    if (d->quickAccessVisible)
        this->onHideQuickAccess();
    d->stack->setCurrentWidget(d->homepageArea);
    d->homepageArea->onShowInfo();

    if (d->homepageArea->getAnimation())
        d->homepageArea->getAnimation()->start();
}

void medMainWindow::switchToBrowserArea() {

    d->quickAccessButton->setText(tr("Workspace: Browser"));
    d->quickAccessButton->setMinimumWidth(170);
    if (d->quickAccessVisible)
        this->onHideQuickAccess();
    d->browserArea->setup(this->statusBar());
    d->viewerArea->setdw(this->statusBar());

    d->stack->setCurrentWidget(d->browserArea);
}

void medMainWindow::switchToViewerArea() {

    if (d->quickAccessVisible)
        this->onHideQuickAccess();
    d->browserArea->setdw(this->statusBar());
    d->viewerArea->setup(this->statusBar());

    d->stack->setCurrentWidget(d->viewerArea);

    //  Dialog window to recall users if database is empty
    //  but only if the warning is enabled in medSettings

    const bool showWarning = medSettingsManager::instance()->value("system","showEmptyDbWarning",QVariant(true)).toBool();
    if (showWarning) {
        QList<medDataIndex> indexes = medDatabaseNonPersistentController::instance()->availableItems();
        QList<medDataIndex> patients = medDatabaseController::instance()->patients();

        if (indexes.isEmpty())
            if (patients.isEmpty()) {
                //            QMessageBox msgBox(this);
                //            msgBox.setIcon(QMessageBox::Warning);
                //            msgBox.setText(tr("The database is empty. Switch to the Browser workspace to import data"));
                medEmptyDbWarning* msgBox = new medEmptyDbWarning(this);
                msgBox->exec();
            }
    }
}

void medMainWindow::onShowWorkspace(const QString& workspace) {
    d->quickAccessButton->setMinimumWidth(170);
    d->viewerArea->setupWorkspace(workspace);
    this->switchToViewerArea();
    medViewerWorkspaceDetails* details =
            medViewerWorkspaceFactory::instance()->workspaceDetailsFromId(workspace);

    d->quickAccessButton->setText(tr("Workspace: ")+details->name);
}

void medMainWindow::onShowQuickAccess() {

    if (d->quickAccessVisible) {
        this->onHideQuickAccess();
        return;
    }
    d->quickAccessWidget->setFocus();
    d->quickAccessVisible = true;
    d->quickAccessAnimation->setDuration(100);
    d->quickAccessAnimation->setStartValue(QPoint(0,this->height()-30));
    d->quickAccessAnimation->setEndValue(QPoint(0,this->height()-d->quickAccessWidget->height()-30));

    d->quickAccessAnimation->start();
}

void medMainWindow::onHideQuickAccess() {

    if (!d->quickAccessVisible)
        return;
    d->quickAccessVisible = false;
    d->quickAccessAnimation->setDuration(100);
    d->quickAccessAnimation->setStartValue(QPoint(0,this->height()-d->quickAccessWidget->height()-30));
    d->quickAccessAnimation->setEndValue(QPoint(0,this->height()-30));
    d->quickAccessAnimation->start();
}

void medMainWindow::onWorkspaceTriggered(QAction *action) {
    d->viewerArea->setupWorkspace(action->text());
}

void medMainWindow::onNoQuit() {
    d->quitMessage->hide();
    d->rightEndButtons->show();
}

void medMainWindow::onQuit() {
    d->quitMessage->show();
    d->rightEndButtons->hide();
}

void medMainWindow::onSaveModified() {
    QList<medDataIndex> indexes =
            medDatabaseNonPersistentController::instance()->availableItems();

    if(!indexes.isEmpty()) {
        medSaveModifiedDialog *saveDialog = new medSaveModifiedDialog(this);
        saveDialog->show();
    }
}

void medMainWindow::onEditSettings() {

    //  Should not be called anymore, now embedded in homepage.

    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle(tr("medInria Settings"));
    dialog->setMinimumHeight(400);
    dialog->setMinimumWidth(500);
    dialog->setMaximumHeight(400);
    dialog->setMaximumWidth(500);

    QVBoxLayout * layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(0,0,0,0);;

    d->settingsEditor = new medSettingsEditor(this,true);
    d->settingsEditor->setGeometry(100,100,500,500);
    d->settingsEditor->setWindowFlags(Qt::Tool);
    d->settingsEditor->initialize();
    d->settingsEditor->queryWidgets();

    layout->addWidget(d->settingsEditor);
    dialog->setLayout(layout);

    connect(d->settingsEditor,SIGNAL(finished()),dialog,SLOT(close()));

    dialog->exec();
}

void medMainWindow::open(const medDataIndex& index) {
    if (d->viewerArea->openInTab(index)) {
        d->quickAccessButton->setText(tr("Workspace: Visualization"));
        d->quickAccessButton->setMinimumWidth(170);
        this->switchToViewerArea();
    }
}

void medMainWindow::open(const QString& file) {
    QString importUuid = QUuid::createUuid().toString();
    d->importUuids.append(importUuid);
    qDebug() << "about to open file" << file;
    qDebug()<< "with uuid:" << importUuid;
    medDataManager::instance()->importNonPersistent (file,importUuid);
}

void medMainWindow::onOpenFile(const medDataIndex& index,const QString& importUuid) {

//    qDebug() << "Opened file from uuid" << importUuid;
//    qDebug() << "uuids in list" << d->importUuids;

    if (!importUuid.isEmpty() && d->importUuids.contains(importUuid)) {
        if (index.isValid()) {
            this->switchToViewerArea();
            d->viewerArea->openInTab(index);
            d->quickAccessButton->setText(tr("Workspace: Visualization"));
            d->quickAccessButton->setMinimumWidth(170);
        } else {
            qDebug() << "Could not Load file with uuid" << importUuid;
        }
        d->importUuids.removeOne(importUuid);
    }
}

void medMainWindow::load(const QString& file) {
    qDebug() << "DEBUG : entering medMainWindow::load";
    medDataManager::instance()->importNonPersistent(file);
}

void medMainWindow::closeEvent(QCloseEvent *event) {

    if (QThreadPool::globalInstance()->activeThreadCount()>0) {
        const int ws_id = QMessageBox::information(this,tr("System message"),
                                         tr("Running background jobs detected! Quit anyway?"),
                                         tr("Quit"),tr("WaitForDone"),
                                         tr("Cancel"),0,1);

        switch (ws_id) {
            case 0:
                // send cancel request to all running jobs, then wait for them
                // Note: most Jobs don't have the cancel method implemented, so this will be effectively the same as waitfordone.
                this->hide();
                medJobManager::instance()->dispatchGlobalCancelEvent();
                QThreadPool::globalInstance()->waitForDone();
                event->accept();
                break;

            case 1:
                // just hide the window and wait
                this->hide();
                QThreadPool::globalInstance()->waitForDone();
                event->accept();
                break;

            default:
                event->ignore();
                return;
                break;
        }
    }

    this->writeSettings();

    medDatabaseController::destroy();
    medDatabaseNonPersistentController::destroy();
    medDataManager::destroy();
}

void medMainWindow::registerToFactories() {

    //Register dbController
    medDbControllerFactory::instance()->registerDbController("DbController",createDbController);
    medDbControllerFactory::instance()->registerDbController("NonPersistentDbController",createNonPersistentDbController);


#if defined(HAVE_SWIG) && defined(HAVE_PYTHON)
    // Setting up core python module

    dtkScriptInterpreterPythonModuleManager::instance()->registerInitializer(&init_core);
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand("import core");
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "dataFactory    = core.dtkAbstractDataFactory.instance()"
       );
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "processFactory = core.dtkAbstractProcessFactory.instance()"
       );
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "viewFactory    = core.dtkAbstractViewFactory.instance()"
       );
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "pluginManager  = core.dtkPluginManager.instance()"
       );
#endif
#if defined(HAVE_SWIG) && defined(HAVE_TCL)
    // Setting up core tcl module

    dtkScriptInterpreterTclModuleManager::instance()->registerInitializer(&Core_Init);
    dtkScriptInterpreterTclModuleManager::instance()->registerCommand(
        "set dataFactory    [dtkAbstractDataFactory_instance]"
       );
    dtkScriptInterpreterTclModuleManager::instance()->registerCommand(
        "set processFactory [dtkAbstractProcessFactory_instance]"
       );
    dtkScriptInterpreterTclModuleManager::instance()->registerCommand(
        "set viewFactory    [dtkAbstractViewFactory_instance]"
       );
    dtkScriptInterpreterTclModuleManager::instance()->registerCommand(
        "set pluginManager  [dtkPluginManager_instance]"
       );
#endif

    // Registering different workspaces
    medViewerWorkspaceFactory * viewerWSpaceFactory = medViewerWorkspaceFactory::instance();
    //TODO: move the register method to the classes maybe??
    viewerWSpaceFactory->
            registerWorkspace<medViewerWorkspaceVisualization>
            ("Visualization",tr("Visualization"),
             tr("Visualize images, Mesh and other data types"));
    viewerWSpaceFactory->
            registerWorkspace<medViewerWorkspaceRegistration>
            ("Registration",tr("Registration"),
             tr("Register a moving image to a fixed image"));
    viewerWSpaceFactory->registerWorkspace
            <medViewerWorkspaceDiffusion>("Diffusion",
                                              tr("Diffusion"),
                                              tr("Diffusion Tensor Images"));
    viewerWSpaceFactory->registerWorkspace
            <medViewerWorkspaceFiltering>("Filtering",
                                              tr("Filtering"),
                                              tr("Filter workspace"));
    medViewerWorkspaceSegmentation::registerWithViewerWorkspaceFactory();

    //Register settingsWidgets
    //TODO: get rid of the setTabName, or make it staic and use it here...
    //or else create a static register method in the widgets...
    medSettingsWidgetFactory * settingsWidgetFactory = medSettingsWidgetFactory::instance();
    settingsWidgetFactory->registerSettingsWidget
            <medSystemSettingsWidget>("System",
                                                  tr("System"),
                                                  tr("System Settings"));
    settingsWidgetFactory->registerSettingsWidget
            <medStartupSettingsWidget>("Startup",
                                       tr("Start Up"),
                                       tr("Start up time settings"));
    settingsWidgetFactory->registerSettingsWidget
            <medDatabaseSettingsWidget>("Database",
                                        tr("Database"),
                                        tr("Database related settings"));
    settingsWidgetFactory->registerSettingsWidget
            <medInteractionSettingsWidget>("Interaction",
                                           tr("Interaction"),
                                           tr("View Interaction settings"));

    //Register annotations
    dtkAbstractDataFactory * datafactory = dtkAbstractDataFactory::instance();
    datafactory->registerDataType(medSeedPointAnnotationData::s_identifier(),dtkAbstractDataCreateFunc<medSeedPointAnnotationData>);
}
