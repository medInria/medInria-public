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

#include <medDatabaseSettingsWidget.h>
#include <medHomepageButton.h>
#include <medMainWindow.h>
#include <medPluginWidget.h>
#include <medStartupSettingsWidget.h>
#include <medWorkspaceFactory.h>

#define VAL(str) #str
#define TOSTRING(str) VAL(str)

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
    actionBrowser->setIcon(QIcon::fromTheme("open"));
    connect(actionBrowser, &QAction::triggered, this, &medHomepageArea::onShowBrowser);
    menuFile->addAction(actionBrowser);

    // --- Settings menu
    QMenu *menuSettings = menu_bar->addMenu("Settings");

    QAction *actionDatabase = new QAction(tr("&Database settings"), parent);
    connect(actionDatabase, &QAction::triggered, this, &medHomepageArea::onShowDatabase);
    menuSettings->addAction(actionDatabase);

    QAction *actionAreaSettings = new QAction(tr("&Startup settings"), parent);
    connect(actionAreaSettings, &QAction::triggered, this, &medHomepageArea::onShowAreaSettings);
    menuSettings->addAction(actionAreaSettings);

    // --- Area menu
    QMenu *menuArea = menu_bar->addMenu("Workspaces");

    QAction *actionHome = new QAction(tr("&Homepage"), parent);
    connect(actionHome, &QAction::triggered, mainWindow, &medMainWindow::switchToHomepageArea);
    menuArea->addAction(actionHome);

    QAction *actionComposer = new QAction(tr("&Composer"), parent);
    connect(actionComposer, &QAction::triggered, this, &medHomepageArea::onShowComposer);
    menuArea->addAction(actionComposer);

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
    QMenu *menuView = menu_bar->addMenu("Views");

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

    QAction *actionAbout = new QAction(tr("&About"), parent);
    connect(actionAbout, &QAction::triggered, this, &medHomepageArea::onShowAbout);
    menuAbout->addAction(actionAbout);

    QAction *actionAuthors = new QAction(tr("Au&thors"), parent);
    connect(actionAuthors, &QAction::triggered, this, &medHomepageArea::onShowAuthors);
    menuAbout->addAction(actionAuthors);

    QAction *actionReleaseNotes = new QAction(tr("&Release Notes "), parent);
    connect(actionReleaseNotes, &QAction::triggered, this, &medHomepageArea::onShowReleaseNotes);
    menuAbout->addAction(actionReleaseNotes);

    QAction *actionLicense = new QAction(tr("&License"), parent);
    connect(actionLicense, &QAction::triggered, this, &medHomepageArea::onShowLicense);
    menuAbout->addAction(actionLicense);

    QAction *actionExtLicense = new QAction(tr("&External Licenses"), parent);
    connect(actionExtLicense, &QAction::triggered, this, &medHomepageArea::onShowExtLicenses);
    menuAbout->addAction(actionExtLicense);

    menuAbout->addSeparator();

    QAction *actionHelp = new QAction(tr("&Help"), parent);
    connect(actionHelp, &QAction::triggered, this, &medHomepageArea::onShowHelp);
    menuAbout->addAction(actionHelp);

    // --- Prepare right corner menu
    QMenuBar *rightMenuBar = new QMenuBar(menu_bar);
    menu_bar->setCornerWidget(rightMenuBar);

    // --- Fullscreen checkable action
    QIcon fullscreenIcon;
    fullscreenIcon.addPixmap(QIcon::fromTheme("fullscreen_on").pixmap(24,24), QIcon::Normal,QIcon::Off);
    fullscreenIcon.addPixmap(QIcon::fromTheme("fullscreen_off").pixmap(24,24),QIcon::Normal,QIcon::On);

    d->actionFullscreen = new QAction(parent);
    d->actionFullscreen->setObjectName("Fullscreen");
    d->actionFullscreen->setIcon(fullscreenIcon);
    d->actionFullscreen->setCheckable(true);
#if defined(Q_OS_MAC)
    d->actionFullscreen->setShortcut(Qt::ControlModifier + Qt::Key_F);
    connect(d->actionFullscreen, &QAction::hovered, [=]{QToolTip::showText(QCursor::pos(), "Switch to fullscreen (cmd+f)", this);});
#else
    d->actionFullscreen->setShortcut(Qt::Key_F11);
    // On Qt5, we can't use setToolTip on an action, without using setToolTipsVisible (by default to false) on the QMenu
    // Here we have a QAction directly on a QMenuBar without a QMenu, so we display manually the tooltip
    connect(d->actionFullscreen, &QAction::hovered, [=]{QToolTip::showText(QCursor::pos(), "Switch to fullscreen (F11)", this);});
#endif
    connect(d->actionFullscreen, &QAction::toggled, mainWindow, &medMainWindow::setFullScreen);
    rightMenuBar->addAction(d->actionFullscreen);
    
    // Setup the description widget: application logo and description
    d->descriptionWidget = new QWidget(this);
    d->descriptionWidget->setProperty("pos", QPoint(10, this->height()/7));

    QVBoxLayout *descriptionLayout = new QVBoxLayout(d->descriptionWidget);
    QHBoxLayout *iconLayout = new QHBoxLayout();
    descriptionLayout->addLayout(iconLayout);

    // Themes
    QVariant themeChosen = medSettingsManager::instance()->value("startup","theme");
    int themeIndex = themeChosen.toInt();
    QString qssLogoName;
    switch (themeIndex)
    {
        case 0:
        default:
        {
            qssLogoName = TOSTRING(LARGE_LOGO_DARK_THEME);
            break;
        }
        case 1:
        case 2:
        {
            qssLogoName = TOSTRING(LARGE_LOGO_LIGHT_THEME);
            break;
        }
    }
    QPixmap medLogo(qssLogoName);
    d->applicationLabel = new QLabel(this);
    d->applicationLabel->setPixmap(medLogo);
    d->applicationLabel->setScaledContents(true);
    d->applicationLabel->setFixedSize(353, 122);
    iconLayout->addWidget(d->applicationLabel);

    if(QString(TOSTRING(APPLICATION_NAME)) != "medInria")
    {
        iconLayout->addWidget(new QLabel("powered by"));
        switch (themeIndex)
        {
            case 0:
            default:
            {
                qssLogoName = ":medInria-src/medInria-logo-dark.svg";
                break;
            }
            case 1:
            case 2:
            {
                qssLogoName = ":medInria-src/medInria-logo-light.svg";
                break;
            }
        }
        QPixmap medInriaLogo(qssLogoName);
        auto medInriaLogoLabel = new QLabel(this);
        medInriaLogoLabel->setPixmap(medInriaLogo);
        medInriaLogoLabel->setScaledContents(true);
        medInriaLogoLabel->setFixedSize(176, 61);
        iconLayout->addWidget(medInriaLogoLabel); 
    }
    iconLayout->addStretch(1);

    d->textEdit = new QTextEdit(this);
    QFile descriptionFile(QString(TOSTRING(DESCRIPTION_HOMEPAGE)));
    descriptionFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream descriptionStream(&descriptionFile);
    descriptionStream.setCodec("UTF-8");
    d->textEdit->setHtml(descriptionStream.readAll());
    d->textEdit->setReadOnly(true);
    d->textEdit->setFocusPolicy(Qt::NoFocus);
    d->textEdit->setMaximumHeight(70);
    d->textEdit->setStyleSheet("background : transparent;");
    descriptionLayout->addWidget(d->textEdit);

    // Setup the navigation widget with buttons to access workspaces
    d->navigationWidget = new QWidget(this);
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
    d->navigationWidget->setProperty("pos", QPoint(20, height()/3));

    // The description text is resized when the window is resized
    int newTextSize = width() - 40;
    d->textEdit->setMinimumWidth(newTextSize);
    d->textEdit->setMaximumWidth(newTextSize);
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
    browserButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    browserButton->setIcon ( QIcon::fromTheme("open") );
    browserButton->setText ( " Import/export files" );
    browserButton->setMinimumHeight ( 40 );
    browserButton->setMaximumWidth ( 250 );
    browserButton->setMinimumWidth ( 250 );
    browserButton->setFocusPolicy ( Qt::NoFocus );
    browserButton->setToolTip("Area to manage and import data");
    workspaceButtonsLayoutBasicGrid->addWidget(browserButton, 0, 0);
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SLOT ( onShowBrowser() ) );

    medHomepageButton * composerButton = new medHomepageButton ( this );
    composerButton->setText ("       Composer");
    composerButton->setFocusPolicy ( Qt::NoFocus );
    composerButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    composerButton->setIcon(QIcon(":/icons/composer.png"));
    composerButton->setMinimumHeight ( 40 );
    composerButton->setMaximumWidth ( 250 );
    composerButton->setMinimumWidth ( 250 );
    composerButton->setToolTip("Open the Composer workspace");
    composerButton->setIdentifier("composer");
    workspaceButtonsLayoutBasicGrid->addWidget(composerButton, 1, 0);
    QObject::connect ( composerButton, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowComposer() ) );

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
    QFile file(TOSTRING(ABOUT_FILE));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    if (QString(TOSTRING(EXPIRATION_TIME)) != "0")
    {
        QDate expiryDate = QDate::fromString(QString(MEDINRIA_BUILD_DATE), "dd_MM_yyyy")
                                            .addMonths(EXPIRATION_TIME);

        text += "<br><b style='color: #ED6639;'> This binary is going to expire on ";
        text += QLocale(QLocale::English).toString(expiryDate, "d MMMM yyyy");
        text += "</b>";
    }

    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

/**
 * @brief Search the "Show Details..." button and click it to expand the text
 * 
 * @param msgBox 
 */
void medHomepageArea::expandDetailedText(QMessageBox *msgBox)
{
    foreach (auto *button, msgBox->buttons())
    {
        if (msgBox->buttonRole(button) == QMessageBox::ActionRole)
        {
            button->click();
            break;
        }
    }
}

void medHomepageArea::onShowAuthors()
{
    QFile file(":authors.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    std::string str = TOSTRING(ADDITIONAL_AUTHORS_LIST);
    str.erase(std::remove(str.begin(),str.end(),'\"'),str.end());
    QString additionalAuthors = QString::fromStdString(str);
    if (!additionalAuthors.isEmpty())
    {   
        additionalAuthors.replace(QString(","), QString("\n"));
        text += "\n *** " + QString(TOSTRING(APPLICATION_NAME)) + " ***\n";
        text += additionalAuthors;
    }

    QMessageBox msgBox;
    msgBox.setText("List of the application authors:            ");
    msgBox.setDetailedText(text);
    expandDetailedText(&msgBox);
    msgBox.exec();
}

void medHomepageArea::onShowReleaseNotes()
{
    QFile file(TOSTRING(RELEASE_NOTES));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText("Here is the release notes with the history of the application:            ");
    msgBox.setDetailedText(text);
    expandDetailedText(&msgBox);
    msgBox.exec();
}

void medHomepageArea::onShowLicense()
{
    QFile file(TOSTRING(LICENSE_FILE));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText("Here is the application license:                           ");
    msgBox.setDetailedText(text);
    expandDetailedText(&msgBox);
    msgBox.exec();
}

void medHomepageArea::onShowExtLicenses()
{
    QFile file(TOSTRING(LICENSE_EXTERNAL_FILE));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString text = file.readAll();

    QMessageBox msgBox;
    msgBox.setText("Here are the external library licenses:                           ");
    msgBox.setDetailedText(text);
    expandDetailedText(&msgBox);
    msgBox.exec();
}

void medHomepageArea::onShowDatabase()
{
    medDatabaseSettingsWidget dialog(this);
    dialog.exec();
}

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
    // DOCUMENTATION_URL needs to be passed as a string in medInria.cmake 
    //to avoid losing path after '/'
    std::string str = TOSTRING(DOCUMENTATION_URL);
    str.erase(std::remove(str.begin(),str.end(),'\"'),str.end());
    QDesktopServices::openUrl(QUrl(QString::fromStdString(str)));
}

void medHomepageArea::onShowComposer()
{
    emit showComposer();
}
