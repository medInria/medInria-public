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

#include <medHomepageArea.h>

// #include <medDatabaseSettingsWidget.h>
#include <medHomepageButton.h>
#include <medMainWindow.h>
#include <medPluginWidget.h>
#include <medStartupSettingsWidget.h>
#include <medWorkspaceFactory.h>

#include <medSourcesLoader.h>
#include <medSourcesLoaderPresenter.h>

class medHomepageAreaPrivate
{
public:
    QWidget *navigationWidget;
    QWidget *descriptionWidget;
    QLabel *applicationLabel;
    QTextEdit *textEdit;

    QAction *actionFullscreen;
};

medHomepageArea::medHomepageArea ( QWidget * parent ) : QWidget ( parent ), d ( new medHomepageAreaPrivate )
{
    // Menu bar
    medMainWindow *mainWindow = qobject_cast <medMainWindow *> (parent);
    QMenuBar *menu_bar = mainWindow->menuBar();

    // --- File menu
    QMenu *menuFile = menu_bar->addMenu("File");

    QAction *actionBrowser = new QAction(tr("&Import/export files"), parent);
    connect(actionBrowser, &QAction::triggered, this, &medHomepageArea::onShowBrowser);
    menuFile->addAction(actionBrowser);

    // QAction *actionDatabase = new QAction(tr("&Database settings"), parent);
    // connect(actionDatabase, &QAction::triggered, this, &medHomepageArea::onShowDatabase);
    // menuFile->addAction(actionDatabase);

    // --- Area menu
    QMenu *menuArea = menu_bar->addMenu("Switch to area");

    QAction *actionAreaSettings = new QAction(tr("&Startup settings"), parent);
    connect(actionAreaSettings, &QAction::triggered, this, &medHomepageArea::onShowAreaSettings);
    menuArea->addAction(actionAreaSettings);

    menuArea->addSeparator();

    QAction *actionHome = new QAction(tr("&Homepage"), parent);
    connect(actionHome, &QAction::triggered, mainWindow, &medMainWindow::switchToHomepageArea);
    menuArea->addAction(actionHome);

    // Visualization workspace is a "Basic" area
    QAction *actionVisu = new QAction("&Visualization", parent);
    actionVisu->setData("medVisualizationWorkspace");
    connect(actionVisu, &QAction::triggered, this, &medHomepageArea::onSwitchToWorkspace);
    menuArea->addAction(actionVisu);

    menuArea->addSeparator();

    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);
    for( medWorkspaceFactory::Details* detail : workspaceDetails)
    {
        if (detail->name != "Visualization")
        {
            QAction *actionWorkspace = new QAction(detail->name, parent);
            actionWorkspace->setData(detail->identifier);
            connect(actionWorkspace, &QAction::triggered, this, &medHomepageArea::onSwitchToWorkspace);
            menuArea->addAction(actionWorkspace);
        }
    }

    // --- View menu
    QMenu *menuView = menu_bar->addMenu("View");

    QAction *actionAjust = new QAction(tr("&Ajust containers size"), parent);
    connect(actionAjust, &QAction::triggered, mainWindow, &medMainWindow::adjustContainersSize);
    menuView->addAction(actionAjust);

    QAction *actionScreenshot = new QAction(tr("Capture screenshot"), parent);
    connect(actionScreenshot, &QAction::triggered, mainWindow, &medMainWindow::captureScreenshot);
    menuView->addAction(actionScreenshot);

    QAction *actionMovie = new QAction(tr("Capture movie"), parent);
    connect(actionMovie, &QAction::triggered, mainWindow, &medMainWindow::captureVideo);
    menuView->addAction(actionMovie);

    // --- Tools menu
    QMenu *menuTools = menu_bar->addMenu("Tools");

    QAction *actionSearch = new QAction(tr("&Search a toolbox"), parent);
    connect(actionSearch, &QAction::triggered, mainWindow, &medMainWindow::switchToSearchArea);
    menuTools->addAction(actionSearch);

    // --- Log menu
    QMenu *menuLog = menu_bar->addMenu("Log");

    QAction *actionLog = new QAction(tr("&Log File"), parent);
    connect(actionLog, &QAction::triggered, this, &medHomepageArea::openLogDirectory);
    menuLog->addAction(actionLog);

    QAction *actionPluginLogs = new QAction(tr("&Plugin Logs"), parent);
    connect(actionPluginLogs, &QAction::triggered, this, &medHomepageArea::onShowPluginLogs);
    menuLog->addAction(actionPluginLogs);

    // --- About menu
    QMenu *menuAbout = menu_bar->addMenu("Info");

    QAction *actionAbout = new QAction(tr("&About the application"), parent);
    connect(actionAbout, &QAction::triggered, this, &medHomepageArea::onShowAbout);
    menuAbout->addAction(actionAbout);

    QAction *actionAuthors = new QAction(tr("Au&thors"), parent);
    connect(actionAuthors, &QAction::triggered, this, &medHomepageArea::onShowAuthors);
    menuAbout->addAction(actionAuthors);

    QAction *actionReleaseNotes = new QAction(tr("&Release Notes"), parent);
    connect(actionReleaseNotes, &QAction::triggered, this, &medHomepageArea::onShowReleaseNotes);
    menuAbout->addAction(actionReleaseNotes);

    QAction *actionLicense = new QAction(tr("&License"), parent);
    connect(actionLicense, &QAction::triggered, this, &medHomepageArea::onShowLicense);
    menuAbout->addAction(actionLicense);

    menuAbout->addSeparator();

    QAction *actionHelp = new QAction(tr("&Help"), parent);
    connect(actionHelp, &QAction::triggered, this, &medHomepageArea::onShowHelp);
    menuAbout->addAction(actionHelp);

    // --- Prepare right corner menu
    QMenuBar *rightMenuBar = new QMenuBar(menu_bar);
    menu_bar->setCornerWidget(rightMenuBar);

    // --- Fullscreen checkable action
    QIcon fullscreenIcon;
    fullscreenIcon.addPixmap(QPixmap(":icons/fullscreenExpand.png"),QIcon::Normal,QIcon::Off);
    fullscreenIcon.addPixmap(QPixmap(":icons/fullscreenReduce.png"),QIcon::Normal,QIcon::On);

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
    connect(d->actionFullscreen, &QAction::toggled, mainWindow, &medMainWindow::setFullScreen);
    // On Qt5, QAction in menubar does not seem to show the Off and On icons, so we do it manually
    connect(d->actionFullscreen, &QAction::toggled, this, &medHomepageArea::switchOffOnFullscreenIcons);
    rightMenuBar->addAction(d->actionFullscreen);
    
    // Setup the description widget: application logo and description
    d->descriptionWidget = new QWidget(this);
    d->descriptionWidget->setProperty("pos", QPoint(10, this->height()/7));

    QHBoxLayout *descriptionLayout = new QHBoxLayout(d->descriptionWidget);

    d->applicationLabel = new QLabel(this);
    QPixmap applicationLogo( ":pixmaps/medInria-logo-homepage.png" );
    d->applicationLabel->setPixmap(applicationLogo);
    descriptionLayout->addWidget(d->applicationLabel);
    descriptionLayout->addStretch();

    d->textEdit = new QTextEdit(this);
    QFile descriptionFile(":DESCRIPTION.txt");
    descriptionFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream descriptionStream(&descriptionFile);
    descriptionStream.setCodec("UTF-8");
    d->textEdit->setHtml(descriptionStream.readAll());
    d->textEdit->setReadOnly(true);
    d->textEdit->setFocusPolicy(Qt::NoFocus);
    d->textEdit->setMinimumWidth(500);
    d->textEdit->setMinimumHeight(150);
    d->textEdit->setMaximumHeight(150);
    descriptionLayout->addWidget(d->textEdit);

    // Setup the navigation widget with buttons to access workspaces
    d->navigationWidget = new QWidget(this);
    d->navigationWidget->setProperty("pos", QPoint(20, this->height()/4));
}

medHomepageArea::~medHomepageArea()
{
    delete d;
    d = nullptr;
}

void medHomepageArea::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);

    // Recompute the widget position when the window is resized
    d->navigationWidget->setProperty("pos", QPoint(20, height()/4));

    // The description text is resized when the window is resized:
    // Total width of the app, minus the logo and the spacings
    int newTextSize = width() - d->applicationLabel->pixmap()->width() - 40;
    d->textEdit->setMinimumWidth(newTextSize);
    d->textEdit->setMaximumWidth(newTextSize);
    d->descriptionWidget->resize(width() - 30, d->applicationLabel->pixmap()->height());
}

void medHomepageArea::initPage()
{
    //Initialization of the navigation widget with available workspaces
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);

    //--- Basic grid
    int spacingBetweenHeaderAndColumn = 10;
    int spacingBetweenColumnButtons = 10;

    QVBoxLayout * workspaceButtonsLayoutBasic = new QVBoxLayout;
    workspaceButtonsLayoutBasic->setSpacing(spacingBetweenHeaderAndColumn);
    QGridLayout * workspaceButtonsLayoutBasicGrid = new QGridLayout;
    workspaceButtonsLayoutBasicGrid->setSpacing(spacingBetweenColumnButtons);
    QLabel * workspaceLabelBasic = new QLabel ( "<b>Basic Area</b>" );
    workspaceLabelBasic->setTextFormat(Qt::RichText);
    workspaceLabelBasic->setAlignment(Qt::AlignLeft);
    workspaceButtonsLayoutBasic->addWidget(workspaceLabelBasic);
    workspaceButtonsLayoutBasic->addLayout(workspaceButtonsLayoutBasicGrid);

    medHomepageButton * browserButton = new medHomepageButton ( this );
    browserButton->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
    browserButton->setIcon ( QIcon ( ":/icons/folder.png" ) );
    browserButton->setText ( "Import/export files" );
    browserButton->setMinimumHeight ( 40 );
    browserButton->setMaximumWidth ( 250 );
    browserButton->setMinimumWidth ( 250 );
    browserButton->setFocusPolicy ( Qt::NoFocus );
    browserButton->setToolTip("Area to manage and import data");
    workspaceButtonsLayoutBasicGrid->addWidget(browserButton, 0, 0);
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SLOT ( onShowBrowser() ) );

    //--- Workspace grids

    QVBoxLayout * workspaceButtonsLayoutMethodology = new QVBoxLayout;
    workspaceButtonsLayoutMethodology->setSpacing(spacingBetweenHeaderAndColumn);
    QGridLayout * workspaceButtonsLayoutMethodologyGrid = new QGridLayout;
    workspaceButtonsLayoutMethodologyGrid->setSpacing(spacingBetweenColumnButtons);
    QLabel * workspaceLabelMethodology = new QLabel ( "<b>Methodology</b>" );
    workspaceLabelMethodology->setTextFormat(Qt::RichText);
    workspaceLabelMethodology->setAlignment(Qt::AlignLeft);
    workspaceButtonsLayoutMethodology->addWidget(workspaceLabelMethodology);
    workspaceButtonsLayoutMethodology->addLayout(workspaceButtonsLayoutMethodologyGrid);

    QVBoxLayout * workspaceButtonsLayoutClinical = new QVBoxLayout;
    workspaceButtonsLayoutClinical->setSpacing(spacingBetweenHeaderAndColumn);
    QGridLayout * workspaceButtonsLayoutClinicalGrid = new QGridLayout;
    workspaceButtonsLayoutClinicalGrid->setSpacing(spacingBetweenColumnButtons);
    QLabel * workspaceLabelClinical = new QLabel ( "<b>Clinical</b>" );
    workspaceLabelClinical->setTextFormat(Qt::RichText);
    workspaceLabelClinical->setAlignment(Qt::AlignLeft);
    workspaceButtonsLayoutClinical->addWidget(workspaceLabelClinical);
    workspaceButtonsLayoutClinical->addLayout(workspaceButtonsLayoutClinicalGrid);

    QVBoxLayout * workspaceButtonsLayoutOther = new QVBoxLayout;
    workspaceButtonsLayoutOther->setSpacing(spacingBetweenHeaderAndColumn);
    QGridLayout * workspaceButtonsLayoutOtherGrid = new QGridLayout;
    workspaceButtonsLayoutOtherGrid->setSpacing(spacingBetweenColumnButtons);
    QLabel * workspaceLabelOther = new QLabel ( "<b>Other</b>" );
    workspaceLabelOther->setTextFormat(Qt::RichText);
    workspaceLabelOther->setAlignment(Qt::AlignLeft);
    workspaceButtonsLayoutOther->addWidget(workspaceLabelOther);
    workspaceButtonsLayoutOther->addLayout(workspaceButtonsLayoutOtherGrid);

    // If there are too many buttons in a category (more than maximumButtonsPerColumn), split them in several columns
    int maximumButtonsPerColumn = 7;

    for( medWorkspaceFactory::Details* detail : workspaceDetails)
    {
        medHomepageButton * button = new medHomepageButton ( this );
        button->setText ( detail->name );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setMinimumHeight ( 40 );
        button->setMaximumWidth ( 250 );
        button->setMinimumWidth ( 250 );
        button->setToolTip(detail->description);
        button->setIdentifier(detail->identifier);
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowWorkspace ( QString ) ) );
        if (!(medWorkspaceFactory::instance()->isUsable(detail->identifier)))
        {
            button->setDisabled(true);
            button->setToolTip("No useful plugin has been found for this workspace.");
        }
        if(!detail->category.compare("Basic")) 
        {
            // Workspaces as "Visualization"
            int row    = workspaceButtonsLayoutBasicGrid->count() % maximumButtonsPerColumn;
            int column = std::ceil(workspaceButtonsLayoutBasicGrid->count() / maximumButtonsPerColumn);
            workspaceButtonsLayoutBasicGrid->addWidget(button, row, column);
        }
        else if(!detail->category.compare("Methodology")) 
        {
            int row    = workspaceButtonsLayoutMethodologyGrid->count() % maximumButtonsPerColumn;
            int column = std::ceil(workspaceButtonsLayoutMethodologyGrid->count() / maximumButtonsPerColumn);
            workspaceButtonsLayoutMethodologyGrid->addWidget(button, row, column);
        }
        else if(!detail->category.compare("Clinical")) 
        {
            int row    = workspaceButtonsLayoutClinicalGrid->count() % maximumButtonsPerColumn;
            int column = std::ceil(workspaceButtonsLayoutClinicalGrid->count() / maximumButtonsPerColumn);
            workspaceButtonsLayoutClinicalGrid->addWidget(button, row, column);
        }
        else
        {
            int row    = workspaceButtonsLayoutOtherGrid->count() % maximumButtonsPerColumn;
            int column = std::ceil(workspaceButtonsLayoutOtherGrid->count() / maximumButtonsPerColumn);
            workspaceButtonsLayoutOtherGrid->addWidget(button, row, column);
        }
    }
    workspaceButtonsLayoutBasic->addStretch();
    workspaceButtonsLayoutMethodology->addStretch();
    workspaceButtonsLayoutClinical->addStretch();
    workspaceButtonsLayoutOther->addStretch();

    QGridLayout* workspaceButtonsLayout = new QGridLayout;
    workspaceButtonsLayout->setSpacing(40); // Spacing between categories

    // Hide the empty categories 
    std::vector<QLayout*> oLayoutVect;
    if (workspaceButtonsLayoutBasicGrid->count() > 0)
    {
        oLayoutVect.push_back(workspaceButtonsLayoutBasic);
    }
    if (workspaceButtonsLayoutMethodologyGrid->count() > 0)
    {
        oLayoutVect.push_back(workspaceButtonsLayoutMethodology);
    }
    if (workspaceButtonsLayoutClinicalGrid->count() > 0)
    {
        oLayoutVect.push_back(workspaceButtonsLayoutClinical);
    }
    if (workspaceButtonsLayoutOtherGrid->count() > 0)
    {
        oLayoutVect.push_back(workspaceButtonsLayoutOther);
    }
    for (int i = 0; i < static_cast<int>(oLayoutVect.size()); ++i)
    {
        workspaceButtonsLayout->addLayout(oLayoutVect[i], 0, i);
    }

    d->navigationWidget->setLayout(workspaceButtonsLayout);
}

void medHomepageArea::onShowBrowser()
{
    emit showBrowser();
}

void medHomepageArea::onShowAbout()
{
    QFile file(":ABOUT.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void medHomepageArea::onShowAuthors()
{
    QFile file(":authors.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void medHomepageArea::onShowReleaseNotes()
{
    QFile file(":RELEASE_NOTES.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText("Here is the release notes with the history of the application:            ");
    msgBox.setDetailedText(text);

    // Search the "Show Details..." button
    foreach (QAbstractButton *button, msgBox.buttons())
    {
        if (msgBox.buttonRole(button) == QMessageBox::ActionRole)
        {
            button->click(); // click it to expand the text
            break;
        }
    }

    msgBox.exec();
}

void medHomepageArea::onShowLicense()
{
    QFile file(":LICENSE.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText("Here is the application License:                           ");
    msgBox.setDetailedText(text);

    // Search the "Show Details..." button
    foreach (QAbstractButton *button, msgBox.buttons())
    {
        if (msgBox.buttonRole(button) == QMessageBox::ActionRole)
        {
            button->click(); // click it to expand the text
            break;
        }
    }

    msgBox.exec();
}

// void medHomepageArea::onShowDatabase()
// {
//     medDatabaseSettingsWidget dialog(this);
//     dialog.exec();
// }

void medHomepageArea::onShowAreaSettings()
{
    medStartupSettingsWidget dialog(this);
    dialog.exec();
}

void medHomepageArea::onShowWorkspace(QString workspace)
{
    emit showWorkspace(workspace);
}

void medHomepageArea::onSwitchToWorkspace()
{
    QAction* currentAction = qobject_cast<QAction*>(sender());
    onShowWorkspace(currentAction->data().toString());
}

void medHomepageArea::openLogDirectory()
{
    QString path = QFileInfo(dtkLogPath(qApp)).path();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void medHomepageArea::onShowPluginLogs()
{
    medPluginWidget dialog(this);
    dialog.exec();
}

void medHomepageArea::onShowHelp()
{
    QDesktopServices::openUrl(QUrl("http://med.inria.fr/help/documentation"));
}

void medHomepageArea::switchOffOnFullscreenIcons(const bool checked)
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
