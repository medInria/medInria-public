/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medBrowserArea.h"
#include "medMainWindow.h"
#include "medWorkspaceArea.h"
#include "medHomepageArea.h"

#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkScript/dtkScriptInterpreter.h>
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
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseModel.h>
#include <medDatabaseItem.h>

#include <medWorkspace.h>
#include <medToolBoxFactory.h>
#include <medWorkspaceFactory.h>
#include <medSettingsWidgetFactory.h>
#include <medSystemSettingsWidget.h>
#include <medStartupSettingsWidget.h>
#include <medDatabaseSettingsWidget.h>
#include <medInteractionSettingsWidget.h>
#include <medSettingsEditor.h>
#include <medEmptyDbWarning.h>

#include "medSeedPointAnnotationData.h"

#include "medVisualizationWorkspace.h"
#include "medRegistrationWorkspace.h"
#include "medDiffusionWorkspace.h"
#include "medFilteringWorkspace.h"
#include "medSegmentationWorkspace.h"

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

// /////////////////////////////////////////////////////////////////
// medMainWindow
// /////////////////////////////////////////////////////////////////

class medMainWindowPrivate
{
public:

    //  Interface elements.

    QStackedWidget*           stack;
    medBrowserArea*           browserArea;
    medWorkspaceArea*         workspaceArea;
    medHomepageArea*          homepageArea;
    medSettingsEditor*        settingsEditor;
    QHBoxLayout*              statusBarLayout;
    QWidget*                  rightEndButtons;
    medStatusBar*             statusBar;
    medQuickAccessPushButton* quickAccessButton;
    QPropertyAnimation*       quickAccessAnimation;
    QToolButton*                quitButton;
    QToolButton*              fullscreenButton;
    QList<QString>            importUuids;

    medQuickAccessMenu * quickAccessWidget;
    bool quickAccessVisible;
    bool controlPressed;
    
    medQuickAccessMenu *shortcutAccessWidget;
    bool shortcutAccessVisible;
    
    QToolButton *screenshotButton;
};

#if defined(HAVE_SWIG) && defined(HAVE_PYTHON)
extern "C" int init_core();               // -- Initialization core layer python wrapped functions
#endif

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

    //  Register controller, workspaces etc

    this->registerToFactories();

    //  Setting up database connection
    if ( !medDatabaseController::instance()->createConnection() )
        qDebug() << "Unable to create a connection to the database";

    connect (medDatabaseNonPersistentController::instance(),SIGNAL(updated(const medDataIndex &, const QString&)),
             this,SLOT(onOpenFile(const medDataIndex&,const QString&)));

    //  Setting up widgets

    d->settingsEditor = NULL;

    //  Browser area.

    d->browserArea = new medBrowserArea(this);
    d->browserArea->setObjectName("Browser");
    connect(d->browserArea,SIGNAL(open(const QString&)),this,SLOT(open(const QString&)));
    connect(d->browserArea,SIGNAL(load(const QString&)),this,SLOT(load(const QString&)));
    connect(d->browserArea,SIGNAL(open(const medDataIndex&)),this,SLOT(open(const medDataIndex&)));

    //  Workspace area.

    d->workspaceArea = new medWorkspaceArea ( this );
    d->workspaceArea->setObjectName ( "Viewer" );

    //  Home page

    d->homepageArea = new medHomepageArea ( this );
    d->homepageArea->setObjectName ( "Homepage" );

    //  Stack

    d->stack = new QStackedWidget ( this );
    d->stack->addWidget ( d->homepageArea );
    d->stack->addWidget ( d->browserArea );
    d->stack->addWidget ( d->workspaceArea );

    connect(d->browserArea, SIGNAL(openRequested(const medDataIndex&, int)), this, SLOT(open(const medDataIndex&, int)));
    
    //  Setup quick access menu

    d->quickAccessButton = new medQuickAccessPushButton ( this );
    d->quickAccessButton->setFocusPolicy ( Qt::NoFocus );
    d->quickAccessButton->setMinimumHeight(31);
    d->quickAccessButton->setStyleSheet("border: 0px;");
    d->quickAccessButton->setIcon(QIcon(":medInria.ico"));
    d->quickAccessButton->setCursor(Qt::PointingHandCursor);
    d->quickAccessButton->setText ( tr("Workspaces access menu") );
    connect ( d->quickAccessButton,  SIGNAL ( clicked() ), this, SLOT ( showQuickAccess() ) );

    d->quickAccessWidget = new medQuickAccessMenu( true, this );
    d->quickAccessWidget->setFocusPolicy(Qt::ClickFocus);
    d->quickAccessWidget->setProperty ( "pos", QPoint ( 0, -500 ) );
    d->quickAccessWidget->setMinimumWidth(180);
    connect(d->quickAccessWidget, SIGNAL(menuHidden()), this, SLOT(hideQuickAccess()));
    connect(d->quickAccessWidget, SIGNAL(homepageSelected()), this, SLOT(switchToHomepageArea()));
    connect(d->quickAccessWidget, SIGNAL(browserSelected()), this, SLOT(switchToBrowserArea()));
    connect(d->quickAccessWidget, SIGNAL(workspaceSelected(QString)), this, SLOT(showWorkspace(QString)));

    d->quickAccessVisible = false;
    d->quickAccessAnimation = new QPropertyAnimation ( d->quickAccessWidget, "pos",this );

    d->shortcutAccessWidget = new medQuickAccessMenu( false, this );
    d->shortcutAccessWidget->setFocusPolicy(Qt::ClickFocus);
    d->shortcutAccessWidget->setProperty ( "pos", QPoint ( 0, -500 ) );
    d->shortcutAccessWidget->setStyleSheet("border-radius: 10px;background-color: rgba(200,200,200,150);");
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
    QObject::connect ( d->fullscreenButton, SIGNAL ( toggled(bool) ),
                       this, SLOT ( setFullScreen(bool) ) );

    QIcon cameraIcon;
    cameraIcon.addPixmap(QPixmap(":icons/camera.png"),QIcon::Normal);
    cameraIcon.addPixmap(QPixmap(":icons/camera_grey.png"),QIcon::Disabled);
    d->screenshotButton = new QToolButton(this);
    d->screenshotButton->setIcon(cameraIcon);
    d->screenshotButton->setObjectName("screenshotButton");
    d->screenshotButton->setShortcut(Qt::AltModifier + Qt::Key_S);
    d->screenshotButton->setToolTip(tr("Capture screenshot"));
    QObject::connect ( d->screenshotButton, SIGNAL ( clicked() ),
                      this, SLOT ( captureScreenshot() ) );

    //  QuitMessage and rightEndButtons will switch hidden and shown statuses.

    d->rightEndButtons = new QWidget(this);
    QHBoxLayout * rightEndButtonsLayout = new QHBoxLayout(d->rightEndButtons);
    rightEndButtonsLayout->setContentsMargins ( 5, 0, 5, 0 );
    rightEndButtonsLayout->setSpacing ( 5 );
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
    d->statusBar->setSizeGripEnabled ( false );
    d->statusBar->setContentsMargins ( 5, 0, 5, 0 );
    d->statusBar->setFixedHeight ( 31 );
    d->statusBar->addPermanentWidget ( statusBarWidget, 1 );

    this->setStatusBar(d->statusBar);
    QObject::connect(d->statusBar, SIGNAL(initializeAvailableSpace()), this,  SLOT(availableSpaceOnStatusBar()));

    this->readSettings();

    //  Init homepage with workspace buttons

    d->homepageArea->initPage();
    QObject::connect ( d->homepageArea, SIGNAL ( showBrowser() ), this, SLOT ( switchToBrowserArea() ) );
    QObject::connect ( d->homepageArea, SIGNAL ( showWorkspace ( QString ) ), this, SLOT ( showWorkspace ( QString ) ) );
    QObject::connect ( d->homepageArea,SIGNAL ( showSettings() ), this, SLOT ( onEditSettings() ) );

    this->setCentralWidget ( d->stack );

    //  Now use the Qt preferred method by setting the Application style instead.
    //  The ownership of the style object is not transferred.
    //  this->setStyle(new QPlastiqueStyle());

    this->setWindowTitle ( "medInria" );

    //  Connect the messageController with the status for notification messages management

    QObject::connect(medMessageController::instance(), SIGNAL(addMessage(medMessage*)), d->statusBar, SLOT(addMessage(medMessage*)));
    QObject::connect(medMessageController::instance(), SIGNAL(removeMessage(medMessage*)), d->statusBar, SLOT(removeMessage(medMessage*)));

    d->workspaceArea->setupWorkspace ( "Visualization" );

    connect ( qApp, SIGNAL ( aboutToQuit() ), this, SLOT ( close() ) );
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
}

/**
 * Key press event reimplementation to handle alt-tab like menu
 */
void medMainWindow::keyPressEvent( QKeyEvent *event )
{
#ifdef Q_OS_MAC
    if (event->key() == Qt::Key_Meta)
#else
    if (event->key() == Qt::Key_Control)
#endif
    {
        d->controlPressed = true;
        return;
    }
    
    if ((event->key() == Qt::Key_Shift)&&(d->controlPressed))
    {
        if (!d->shortcutAccessVisible)
            this->showShortcutAccess();

        d->shortcutAccessWidget->updateCurrentlySelectedRight();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

/**
 * Key release event reimplementation to handle alt-tab like menu
 */
void medMainWindow::keyReleaseEvent( QKeyEvent * event )
{
#ifdef Q_OS_MAC
    if (event->key() == Qt::Key_Meta)
#else
    if (event->key() == Qt::Key_Control)
#endif
    {
        if (d->shortcutAccessVisible)
        {
            d->shortcutAccessWidget->switchToCurrentlySelected();
            this->hideShortcutAccess();
        }
        d->controlPressed = false;
    }
    
    QMainWindow::keyReleaseEvent(event);
}

void medMainWindow::readSettings()
{
    //  If the user configured a default area we need to show it

    medSettingsManager * mnger = medSettingsManager::instance();

    //  If nothing is configured then Browser is the default area

    const AreaType areaIndex = static_cast<AreaType>(mnger->value("startup","default_starting_area",0).toInt());

    switchToArea(areaIndex);

    //  Restore size

    if ( !this->isFullScreen() )
    {
        QPoint pos = mnger->value ( "application", "pos", QPoint ( 200, 200 ) ).toPoint();
        QSize size = mnger->value ( "application", "size", QSize ( 600, 400 ) ).toSize();
        move ( pos );
        resize ( size );
    }
}

void medMainWindow::writeSettings() {
    // if the app is in full screen mode we do not want to save the pos and size
    // as there is a setting that defines either the user wants to open the app in FS or not
    // so, if he/she chose not to and quit the app while in FS mode, we skip the settings saving
    if ( !this->isFullScreen() )
    {
        medSettingsManager::instance()->setValue ( "application","pos", pos() );
        medSettingsManager::instance()->setValue ( "application","size", size() );
    }
}

void medMainWindow::setStartup(const AreaType areaIndex,const QStringList& filenames) {
    switchToArea(areaIndex);
    for (QStringList::const_iterator i= filenames.constBegin();i!=filenames.constEnd();++i)
        open(i->toLocal8Bit().constData());
}

void medMainWindow::switchToArea(const AreaType areaIndex) {
    switch (areaIndex) {
        case 0:
            this->switchToHomepageArea();
            break;

        case 1:
            this->switchToBrowserArea();
            break;

        case 2: {
            this->switchToWorkspaceArea();
            break;
        }

        default:
            this->switchToHomepageArea();
            break;
        }

}

void medMainWindow::resizeEvent ( QResizeEvent* event )
{
    QWidget::resizeEvent ( event );
    d->quickAccessWidget->setProperty ( "pos", QPoint ( 0, this->height() - 30 ));
    this->hideQuickAccess();
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

void medMainWindow::setFullScreen (const bool full )
{
    if ( full )
        this->showFullScreen();
    else
        this->showNormal();
}

void medMainWindow::switchFullScreen ( )
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
//    qDebug()<<"fullscreen!";
    //update the icon, but do not trigger too many events
    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(true);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showFullScreen();
    this->setAcceptDrops(true);
}

void medMainWindow::showNormal()
{
//    qDebug()<<"normal";
    //update the icon, but do not trigger too many events
    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(false);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showNormal();
}

void medMainWindow::showMaximized()
{
//    qDebug()<<"maximized";
    //update the icon, but do not trigger too many events
    d->fullscreenButton->blockSignals(true);
    d->fullscreenButton->setChecked(false);
    d->fullscreenButton->blockSignals(false);
    QMainWindow::showMaximized();
}

void medMainWindow::switchToHomepageArea()
{
    d->shortcutAccessWidget->updateSelected("Homepage");
    d->quickAccessWidget->updateSelected("Homepage");

    d->quickAccessButton->setText(tr("Workspaces access menu"));
    d->quickAccessButton->setMinimumWidth(170);
    if (d->quickAccessVisible)
        this->hideQuickAccess();
    
    if (d->shortcutAccessVisible)
        this->hideShortcutAccess();

    d->stack->setCurrentWidget ( d->homepageArea );
    d->homepageArea->onShowInfo();
    
    d->screenshotButton->setEnabled(false);

    if ( d->homepageArea->getAnimation() )
        d->homepageArea->getAnimation()->start();
}

void medMainWindow::switchToBrowserArea()
{
    d->shortcutAccessWidget->updateSelected("Browser");
    d->quickAccessWidget->updateSelected("Browser");

    d->quickAccessButton->setText(tr("Workspace: Browser"));
    d->quickAccessButton->setMinimumWidth(170);
    if (d->quickAccessVisible)
        this->hideQuickAccess();

    if (d->shortcutAccessVisible)
        this->hideShortcutAccess();

    d->browserArea->setup ( this->statusBar() );
    d->workspaceArea->setdw ( this->statusBar() );

    d->screenshotButton->setEnabled(false);

    d->stack->setCurrentWidget ( d->browserArea );
}

void medMainWindow::switchToWorkspaceArea()
{
    if (d->quickAccessVisible)
        this->hideQuickAccess();
    
    if (d->shortcutAccessVisible)
        this->hideShortcutAccess();

    d->browserArea->setdw ( this->statusBar() );
    d->workspaceArea->setup ( this->statusBar() );

    d->screenshotButton->setEnabled(true);

    d->stack->setCurrentWidget ( d->workspaceArea );

    // Dialog window to recall users if database is empty
    //but only if the warning is enabled in medSettings
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
                //            QMessageBox msgBox ( this );
                //            msgBox.setIcon ( QMessageBox::Warning );
                //            msgBox.setText(tr("The database is empty. Switch to the Browser workspace to import data"));
                medEmptyDbWarning* msgBox = new medEmptyDbWarning(this);
                msgBox->exec();
            }
    }
}

void medMainWindow::showWorkspace ( QString workspace )
{
    d->quickAccessButton->setMinimumWidth(170);
    d->workspaceArea->setupWorkspace(workspace);
    this->switchToWorkspaceArea();
    medWorkspaceDetails* details =
            medWorkspaceFactory::instance()->workspaceDetailsFromId(workspace);

    d->quickAccessButton->setText(tr("Workspace: ") + details->name);
    d->shortcutAccessWidget->updateSelected(workspace);
    d->quickAccessWidget->updateSelected(workspace);
}

/**
 * Slot to show bottom left menu
 */
void medMainWindow::showQuickAccess()
{
    if ( d->quickAccessVisible )
    {
        this->hideQuickAccess();
        return;
    }
    d->quickAccessWidget->reset(false);
    d->quickAccessWidget->setFocus();
    d->quickAccessWidget->setMouseTracking(true);
    d->quickAccessVisible = true;
    d->quickAccessAnimation->setDuration ( 100 );
    d->quickAccessAnimation->setStartValue ( QPoint ( 0,this->height() - 30 ) );
    d->quickAccessAnimation->setEndValue ( QPoint ( 0,this->height() - d->quickAccessWidget->height() - 30 ) );

    d->quickAccessAnimation->start();
}

/**
 * Slot to hide bottom left menu
 */
void medMainWindow::hideQuickAccess()
{
    if (!d->quickAccessVisible)
        return;
    d->quickAccessVisible = false;
    d->quickAccessWidget->setMouseTracking(false);
    d->quickAccessAnimation->setDuration ( 100 );
    d->quickAccessAnimation->setStartValue ( QPoint ( 0,this->height() - d->quickAccessWidget->height() -30 ));
    d->quickAccessAnimation->setEndValue ( QPoint ( 0,this->height() - 30 ) );
    d->quickAccessAnimation->start();
}

/**
 * Slot to show alt-tab like menu
 */
void medMainWindow::showShortcutAccess()
{
    if ( d->shortcutAccessVisible )
    {
        this->hideShortcutAccess();
        return;
    }
    
    d->shortcutAccessWidget->reset(true);
    d->shortcutAccessVisible = true;

    QPoint menuPosition = this->mapToGlobal(this->rect().topLeft());
    menuPosition.setX(menuPosition.rx() + (this->rect().width() - d->shortcutAccessWidget->width()) / 2);
    menuPosition.setY(menuPosition.ry() + (this->rect().height() - d->shortcutAccessWidget->height()) / 2);
    
    d->shortcutAccessWidget->setProperty ( "pos", menuPosition );
    d->shortcutAccessWidget->show();
    d->shortcutAccessWidget->setFocus();
    d->shortcutAccessWidget->setMouseTracking(true);
}

/**
 * Slot to hide alt-tab like menu
 */
void medMainWindow::hideShortcutAccess()
{
    if (!d->shortcutAccessVisible)
        return;
    
    d->shortcutAccessWidget->setMouseTracking(false);
    d->shortcutAccessVisible = false;
    d->shortcutAccessWidget->setProperty ( "pos", QPoint ( 0 , -500 ) );
    d->shortcutAccessWidget->hide();
}

void medMainWindow::onWorkspaceTriggered ( QAction *action )
{
    d->workspaceArea->setupWorkspace ( action->text() );
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

void medMainWindow::onEditSettings()
{
    //Should not be called anymore, now embedded in homepage.
    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle(tr("medInria Settings"));
    dialog->setMinimumHeight(400);
    dialog->setMinimumWidth(500);
    dialog->setMaximumHeight(400);
    dialog->setMaximumWidth(500);

    QVBoxLayout * layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(0,0,0,0);;

    d->settingsEditor = new medSettingsEditor ( this, true );
    d->settingsEditor->setGeometry ( 100,100, 500, 500 );
    d->settingsEditor->setWindowFlags ( Qt::Tool );
    d->settingsEditor->initialize();
    d->settingsEditor->queryWidgets();

    layout->addWidget(d->settingsEditor);
    dialog->setLayout(layout);

    connect ( d->settingsEditor, SIGNAL ( finished() ), dialog, SLOT ( close() ) );

    dialog->exec();
}

void medMainWindow::open ( const medDataIndex& index , int slice )
{
   connect (this, SIGNAL(sliceSelected(int)), d->workspaceArea, SIGNAL(sliceSelected(int)));
   if(d->workspaceArea->openInTab(index))
    {
        d->quickAccessButton->setText(tr("Workspace: Visualization"));
        d->quickAccessButton->setMinimumWidth(170);
        this->switchToWorkspaceArea();
        emit sliceSelected(slice);  //to display the selected slice
    }
}

void medMainWindow::availableSpaceOnStatusBar()
{
    QPoint workspaceButton_topRight = d->quickAccessButton->mapTo(d->statusBar, d->quickAccessButton->rect().topRight());
    QPoint screenshotButton_topLeft = d->screenshotButton->mapTo(d->statusBar, d->screenshotButton->rect().topLeft());
    //Available space = space between the spacing after workspace button and the spacing before screenshot button
    int space = (screenshotButton_topLeft.x()-d->statusBarLayout->spacing()) -  (workspaceButton_topRight.x()+d->statusBarLayout->spacing()); 
    d->statusBar->setAvailableSpace(space);
}

void medMainWindow::onNewInstance(const QString& message) {
    if (message.toLower().startsWith("/open ")) {
        const QString filename = message.mid(6);
        open(filename);
    }
}

void medMainWindow::open ( const medDataIndex& index )
{
    if(d->workspaceArea->openInTab(index))
    {
        d->quickAccessButton->setText(tr("Workspace: Visualization"));
        d->quickAccessButton->setMinimumWidth(170);
        this->switchToWorkspaceArea();
    }
}

void medMainWindow::open ( const QString& file )
{
    QString importUuid = QUuid::createUuid().toString();
    d->importUuids.append(importUuid);
    qDebug() << "about to open file" << file;
    qDebug()<< "with uuid:" << importUuid;
    medDataManager::instance()->importNonPersistent (file,importUuid);
}

void medMainWindow::onOpenFile(const medDataIndex & index,const QString& importUuid)
{
    if (!importUuid.isEmpty() && d->importUuids.contains(importUuid))
    {
        if (index.isValid())
        {
            this->switchToWorkspaceArea();
            d->workspaceArea->openInTab(index);
            d->quickAccessButton->setText(tr("Workspace: Visualization"));
            d->quickAccessButton->setMinimumWidth(170);
        }
        else
        {
            qDebug() << "Could not Load file with uuid" << importUuid;
        }
        d->importUuids.removeOne(importUuid);
    }
}


void medMainWindow::load(const QString& file)
{
    qDebug() << "DEBUG : entering medMainWindow::load";
    medDataManager::instance()->importNonPersistent (file);
}

void medMainWindow::closeEvent(QCloseEvent *event)
{
    if ( QThreadPool::globalInstance()->activeThreadCount() > 0 )
    {
        switch ( QMessageBox::information ( this, tr("System message"),
                                            tr("Running background jobs detected! Quit anyway?"),
                                            tr("Quit"), tr("WaitForDone"),
                                            tr("Cancel"),0, 1 ) )
        {
        case 0:
            // send cancel request to all running jobs, then wait for them
            // Note: most Jobs don't have the cancel method implemented, so this will be effectively the same as waitfordone.
            medJobManager::instance()->dispatchGlobalCancelEvent();
            QThreadPool::globalInstance()->waitForDone();
            break;

        case 1:
            // just hide the window and wait
            QThreadPool::globalInstance()->waitForDone();
            break;

        default:
            event->ignore();
            return;
            break;
        }

    }
    
    if (this->saveModified() != QDialog::Accepted)
    {
        event->ignore();
        return;
    }
    
    this->hide();
    event->accept();

    this->writeSettings();

    medDatabaseController::destroy();
    medDatabaseNonPersistentController::destroy();
    medDataManager::destroy();
}

void medMainWindow::registerToFactories()
{
    //Register dbController
    medDbControllerFactory::instance()->registerDbController("DbController", createDbController);
    medDbControllerFactory::instance()->registerDbController("NonPersistentDbController", createNonPersistentDbController);


#if defined(HAVE_SWIG) && defined(HAVE_PYTHON)
    // Setting up core python module

    dtkScriptInterpreterPythonModuleManager::instance()->registerInitializer(&init_core);
    dtkScriptInterpreterPythonModuleManager::instance()->registerCommand(
        "import core"
        );
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

    // Registering different workspaces
    medWorkspaceFactory * viewerWSpaceFactory = medWorkspaceFactory::instance();
    //TODO: move the register method to the classes maybe??
    viewerWSpaceFactory->
            registerWorkspace<medVisualizationWorkspace>
            ("Visualization",tr("Visualization"),
             tr("Visualize images, Mesh and other data types"));
    viewerWSpaceFactory->
            registerWorkspace<medRegistrationWorkspace>
            ("Registration", tr("Registration"),
             tr("Register a moving image to a fixed image"));
    viewerWSpaceFactory->registerWorkspace
            <medDiffusionWorkspace>("Diffusion",
                                              tr("Diffusion"),
                                              tr("Diffusion Tensor Images"));
    viewerWSpaceFactory->registerWorkspace
            <medFilteringWorkspace>("Filtering",
                                              tr("Filtering"),
                                              tr("Filter workspace"));
    medSegmentationWorkspace::registerWithViewerWorkspaceFactory();

    //Register settingsWidgets
    //TODO: get rid of the setTabName, or make it static and use it here...
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
    datafactory->registerDataType( medSeedPointAnnotationData::s_identifier(), dtkAbstractDataCreateFunc<medSeedPointAnnotationData> );
}

bool medMainWindow::eventFilter(QObject * obj, QEvent *ev)
{
    if (ev->type() == QEvent::KeyPress)
    {
        QKeyEvent * keyEvent = static_cast<QKeyEvent*>(ev);
        this->keyPressEvent(keyEvent);
    }

    if (ev->type()==QEvent::KeyRelease)
    {
        QKeyEvent * keyEvent = static_cast<QKeyEvent*>(ev);
        this->keyReleaseEvent(keyEvent);
    }
    
    // For the time being, We do not use this function to filter but only to send event to the MainWindow.
    // Therefore, we always return false;

    return false; 
}

bool medMainWindow::event(QEvent * e)
{
    switch(e->type())
    {
        // ...
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
        // ...
    } ;
    return QMainWindow::event(e) ;
}   
