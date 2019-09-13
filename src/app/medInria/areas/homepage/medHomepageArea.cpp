/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medHomepageArea.h>

#include <medHomepageButton.h>
#include <medPluginWidget.h>
#include <medSettingsEditor.h>
#include <medWorkspaceFactory.h>

class medHomepageAreaPrivate
{
public:
    QStackedWidget* stackedWidget;
    QWidget * navigationWidget;
    QWidget * userWidget;
    QWidget * infoWidget;
    QWidget * aboutWidget;
    QTabWidget * aboutTabWidget;
    QWidget * pluginWidget;
    QWidget * settingsWidget;
    medSettingsEditor* settingsEditor;
};

medHomepageArea::medHomepageArea ( QWidget * parent ) : QWidget ( parent ), d ( new medHomepageAreaPrivate )
{
    //Setup navigation widget (with buttons for accessing available workspaces)
    d->navigationWidget = new QWidget ( this );

    //Setup the widget where the medInria general information are displayed
    d->infoWidget = new QWidget ( this );
    d->infoWidget->setMinimumSize(400, 400);

    //Setup the widget with about, settings, plugins and documentation buttons
    d->userWidget = new QWidget ( this );

    //Setup the about container widget (with a QTabWidget inside)
    d->aboutWidget = new QWidget ( this );
    d->aboutWidget->setMinimumSize(400, 400);
    d->aboutWidget->hide();

    //User widget content with settings, about and help buttons
    QHBoxLayout * userButtonsLayout = new QHBoxLayout(d->userWidget);
    medHomepageButton * helpButton = new medHomepageButton ( this );
    helpButton->setText ( "Help" );
    helpButton->setToolTip(tr("Open Online Documentation"));
    helpButton->setMinimumHeight ( 30 );
    helpButton->setMaximumWidth ( 150 );
    helpButton->setMinimumWidth ( 150 );
    helpButton->setFocusPolicy ( Qt::NoFocus );
    helpButton->setIcon ( QIcon ( ":icons/help.svg" ) );
    helpButton->setIconSize(QSize(16,16));
    helpButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( helpButton,SIGNAL ( clicked() ),this, SLOT ( onShowHelp() ) );

    medHomepageButton * aboutButton = new medHomepageButton ( this );
    aboutButton->setText ( "About" );
    aboutButton->setMinimumHeight ( 30 );
    aboutButton->setMaximumWidth ( 150 );
    aboutButton->setMinimumWidth ( 150 );
    aboutButton->setToolTip(QString("About ")+qApp->applicationName());
    aboutButton->setFocusPolicy ( Qt::NoFocus );
    aboutButton->setIcon ( QIcon ( ":icons/about.png" ) );
    aboutButton->setIconSize(QSize(16,16));
    aboutButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( aboutButton,SIGNAL ( clicked() ),this, SLOT ( onShowAbout() ) );

    medHomepageButton * pluginButton = new medHomepageButton ( this );
    pluginButton->setText ( "Plugins" );
    pluginButton->setMinimumHeight ( 30 );
    pluginButton->setMaximumWidth ( 150 );
    pluginButton->setMinimumWidth ( 150 );
    pluginButton->setToolTip(tr("Information on loaded plugins"));
    pluginButton->setFocusPolicy ( Qt::NoFocus );
    pluginButton->setIcon ( QIcon ( ":icons/medInriaPlugin.png" ) );
    pluginButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( pluginButton,SIGNAL ( clicked() ),this, SLOT ( onShowPlugin() ) );

    medHomepageButton * logButton = new medHomepageButton ( this );
    logButton->setText ( "Log" );
    logButton->setMinimumHeight ( 30 );
    logButton->setMaximumWidth ( 150 );
    logButton->setMinimumWidth ( 150 );
    logButton->setToolTip(QString("Open Log Directory.\nThe log file is ")
                             + QString(qApp->applicationName())
                             + QString(".log"));
    logButton->setFocusPolicy ( Qt::NoFocus );
    logButton->setIcon ( QIcon ( ":icons/widget.png" ) );
    logButton->setIconSize(QSize(16,16));
    logButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( logButton,SIGNAL ( clicked() ),this, SLOT ( openLogDirectory() ) );

    medHomepageButton * settingsButton = new medHomepageButton ( this );
    settingsButton->setText ( "Settings" );
    settingsButton->setMinimumHeight ( 30 );
    settingsButton->setMaximumWidth ( 150 );
    settingsButton->setMinimumWidth ( 150 );
    settingsButton->setToolTip(QString("Configure ")+qApp->applicationName());
    settingsButton->setFocusPolicy ( Qt::NoFocus );
    settingsButton->setIcon ( QIcon ( ":icons/settings.svg" ) );
    settingsButton->setIconSize(QSize(16,16));
    settingsButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( settingsButton,SIGNAL ( clicked() ),this, SLOT ( onShowSettings() ) );

    userButtonsLayout->insertWidget ( 0, settingsButton );
    userButtonsLayout->insertWidget ( 1, pluginButton );
    userButtonsLayout->insertWidget ( 2, aboutButton );
    userButtonsLayout->insertWidget ( 3, logButton );
    userButtonsLayout->insertWidget ( 4, helpButton );

    // Info widget : medInria logo, medInria description, etc. QtWebkit ?
    QVBoxLayout * infoLayout = new QVBoxLayout(d->infoWidget);
    QLabel * medInriaLabel = new QLabel ( this );
    QPixmap medLogo( ":pixmaps/medInria-logo-homepage.png" );
    medInriaLabel->setPixmap ( medLogo );

    QTextEdit * textEdit = new QTextEdit(this);
    textEdit->setHtml ( tr("<b>medInria</b> is a cross-platform medical image "
                           "processing and visualisation software, "
                           "and it is <b>free</b>. Through an intuitive user "
                           "interface, <b>medInria</b> offers from standard "
                           "to cutting-edge processing functionalities for "
                           "your medical images such as 2D/3D/4D image "
                           "visualisation, image registration, or diffusion "
                           "MR processing and tractography." ));
    textEdit->setReadOnly ( true );
    textEdit->setFocusPolicy ( Qt::NoFocus );
    textEdit->setMaximumHeight(300);
    textEdit->setMinimumHeight(250);
    infoLayout->insertWidget ( 0,medInriaLabel );
    infoLayout->insertWidget ( 1, textEdit );
    infoLayout->addStretch();

    d->infoWidget->setMaximumHeight ( medInriaLabel->height() + textEdit->height() );

    //About widget
    QVBoxLayout * aboutLayout = new QVBoxLayout(d->aboutWidget);
    d->aboutTabWidget = new QTabWidget(this);
    d->aboutTabWidget->setObjectName("aboutTabWidget");

    QLabel * medInriaLabel2 = new QLabel ( this );
    medInriaLabel2->setPixmap ( medLogo );

    QTextEdit * aboutTextEdit = new QTextEdit(this);

    QString aboutText = QString::fromUtf8(
                "%1 (%2) is a medical imaging platform developed at "
                "Inria.<br/>"
                "<center>Inria, Copyright 2013</center>")
            .arg(qApp->applicationName())
            .arg(qApp->applicationVersion());

    aboutTextEdit->setHtml (aboutText);
    aboutTextEdit->setFocusPolicy ( Qt::NoFocus );

    QTextBrowser * aboutAuthorTextBrowser = new QTextBrowser(this);
    aboutAuthorTextBrowser->setSource(QUrl("qrc:authors.html" ));
    aboutAuthorTextBrowser->setFocusPolicy ( Qt::NoFocus );

    QTextEdit * aboutLicenseTextEdit = new QTextEdit(this);
    QFile license ( ":LICENSE.txt" );
    license.open ( QIODevice::ReadOnly | QIODevice::Text );
    QTextStream licenseContent(&license);
    licenseContent.setCodec("UTF-8");
    aboutLicenseTextEdit->setText ( licenseContent.readAll() );
    aboutLicenseTextEdit->setFocusPolicy ( Qt::NoFocus );
    license.close();

    QTextEdit * releaseNotesTextEdit = new QTextEdit(this);
    QFile releaseNotes ( ":RELEASE_NOTES.txt" );
    releaseNotes.open ( QIODevice::ReadOnly | QIODevice::Text );
    QString releaseNotesContent = releaseNotes.readAll();
    releaseNotes.close();
    releaseNotesTextEdit->setText ( releaseNotesContent );
    releaseNotesTextEdit->setFocusPolicy ( Qt::NoFocus );

    //no parent, this layout is added to an other layout.
    QHBoxLayout * aboutButtonLayout = new QHBoxLayout();
    QPushButton * hideAboutButton = new QPushButton ( this );
    hideAboutButton->setText ( tr("Hide") );
    hideAboutButton->setFocusPolicy ( Qt::NoFocus );
    hideAboutButton->setToolTip( tr("Hide the About section") );
    QObject::connect ( hideAboutButton, SIGNAL ( clicked() ), this, SLOT ( onShowInfo() ) );

    aboutButtonLayout->addStretch();
    aboutButtonLayout->addWidget ( hideAboutButton );
    aboutButtonLayout->addStretch();

    d->aboutTabWidget->addTab ( aboutTextEdit, tr("About") );
    d->aboutTabWidget->addTab ( aboutAuthorTextBrowser, tr("Authors") );
    d->aboutTabWidget->addTab ( releaseNotesTextEdit, tr("Release Notes") );
    d->aboutTabWidget->addTab ( aboutLicenseTextEdit, tr("License") );

    aboutLayout->addWidget ( medInriaLabel2 );
    aboutLayout->addWidget ( d->aboutTabWidget );
    aboutLayout->addLayout ( aboutButtonLayout );

    //Create the plugin widget.
    d->pluginWidget = new QWidget(this);
    QVBoxLayout * pluginLayout = new QVBoxLayout(d->pluginWidget);
    QHBoxLayout * pluginHideButtonLayout = new QHBoxLayout();
    QPushButton * hidePluginButton = new QPushButton ( this );
    hidePluginButton->setText ( tr("Hide") );
    hidePluginButton->setFocusPolicy ( Qt::NoFocus );
    hidePluginButton->setToolTip( tr("Hide the Plugins section") );
    QObject::connect ( hidePluginButton, SIGNAL ( clicked() ), this, SLOT ( onShowInfo() ) );

    pluginHideButtonLayout->addStretch();
    pluginHideButtonLayout->addWidget ( hidePluginButton );
    pluginHideButtonLayout->addStretch();

    QLabel * medInriaLabel3 = new QLabel ( this );
    medInriaLabel3->setPixmap ( medLogo );

    medPluginWidget * pWid = new medPluginWidget(d->pluginWidget);

    pluginLayout->addWidget(medInriaLabel3);
    pluginLayout->addWidget(pWid);
    pluginLayout->addLayout(pluginHideButtonLayout);

    //Create the setttings widget.
    d->settingsWidget = new QWidget(this);
    d->settingsWidget->setObjectName("settingsWidget");
    QVBoxLayout * settingsLayout = new QVBoxLayout(d->settingsWidget);
    QHBoxLayout * settingsHideButtonLayout = new QHBoxLayout();
    QPushButton * hideSettingsButton = new QPushButton ( this );
    hideSettingsButton->setText ( tr("Hide") );
    hideSettingsButton->setFocusPolicy ( Qt::NoFocus );
    hideSettingsButton->setToolTip( tr("Hide the Settings section") );
    QObject::connect ( hideSettingsButton, SIGNAL ( clicked() ), this, SLOT ( onShowInfo() ) );

    settingsHideButtonLayout->addStretch();
    settingsHideButtonLayout->addWidget ( hideSettingsButton );
    settingsHideButtonLayout->addStretch();

    QLabel * medInriaLabel4 = new QLabel ( this );
    medInriaLabel4->setPixmap ( medLogo );

    d->settingsEditor = new medSettingsEditor(d->settingsWidget,true);
    settingsLayout->addWidget(medInriaLabel4);
    settingsLayout->addWidget(d->settingsEditor);
    settingsLayout->addLayout(settingsHideButtonLayout);

    //Set the position of the widgets
    d->navigationWidget->setProperty ( "pos", QPoint ( 100 ,  this->height() / 4 ) );
    d->userWidget->setProperty ( "pos", QPoint ( this->width() - 350 ,  this->height() - 90 ) );

    //Create a Stacked Widget in which to put info widget, about widget and plugin Widget
    d->stackedWidget = new QStackedWidget( this );
    d->stackedWidget->setMinimumSize ( 400,300 );

    d->stackedWidget->setProperty ( "pos", QPoint ( this->width() / 2 ,
                                                    this->height() / 5) );
    int stackedWidgetHeight = d->userWidget->pos().y() - d->stackedWidget->pos().y();
    if (d->stackedWidget->minimumHeight() > stackedWidgetHeight)
        stackedWidgetHeight = d->stackedWidget->minimumHeight();
    d->stackedWidget->setMaximumHeight(stackedWidgetHeight);
    d->stackedWidget->setMaximumWidth(this->width() / 2-50);
    d->stackedWidget->addWidget(d->infoWidget);
    d->stackedWidget->addWidget(d->aboutWidget);
    d->stackedWidget->addWidget(d->pluginWidget);
    d->stackedWidget->addWidget(d->settingsWidget);
    d->stackedWidget->setCurrentIndex(0);//d->infoWidget
    d->stackedWidget->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
}

medHomepageArea::~medHomepageArea()
{
    delete d;
    d = nullptr;
}

void medHomepageArea::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);

    //Recompute the widgets position when the window is resized
    d->navigationWidget->setProperty ( "pos", QPoint ( 20 ,  this->height() / 4 ) );

    d->userWidget->setProperty ( "pos", QPoint ( this->width() - 450,  this->height() - 50 ) );

    d->stackedWidget->setProperty ( "pos", QPoint ( this->width() / 2 ,  this->height() / 5 ) );

    int stackedWidgetHeight = d->userWidget->pos().y() - d->stackedWidget->pos().y();
    if (d->stackedWidget->minimumHeight() > stackedWidgetHeight)
    {
        stackedWidgetHeight = d->stackedWidget->minimumHeight();
    }
    d->stackedWidget->setMaximumHeight(stackedWidgetHeight);

    int stackedWidgetWidth =  this->width() / 2 - 50 ;
    d->stackedWidget->setMaximumWidth(stackedWidgetWidth);

    d->stackedWidget->resize(stackedWidgetWidth,stackedWidgetHeight);
}

void medHomepageArea::initPage()
{
    //Initialization of the navigation widget with available workspaces
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName(true);

    QVBoxLayout * workspaceButtonsLayoutBasic = new QVBoxLayout;
    workspaceButtonsLayoutBasic->setSpacing ( 10 );
    QLabel * workspaceLabelBasic = new QLabel ( "<b>Basic</b>" );
    workspaceLabelBasic->setTextFormat(Qt::RichText);
    workspaceLabelBasic->setAlignment(Qt::AlignHCenter);
    workspaceLabelBasic->setMargin(10);
    workspaceButtonsLayoutBasic->addWidget ( workspaceLabelBasic );

    medHomepageButton * browserButton = new medHomepageButton ( this );
    browserButton->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
    browserButton->setIcon ( QIcon ( ":/icons/folder.png" ) );
    browserButton->setText ( "Browser" );
    browserButton->setMinimumHeight ( 40 );
    browserButton->setMaximumWidth ( 250 );
    browserButton->setMinimumWidth ( 250 );
    browserButton->setFocusPolicy ( Qt::NoFocus );
    browserButton->setToolTip("Workspace to manage and import data.");
    workspaceButtonsLayoutBasic->addWidget ( browserButton );
    workspaceButtonsLayoutBasic->addSpacing(10);
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SLOT ( onShowBrowser() ) );

    medHomepageButton * composerButton = new medHomepageButton ( this );
    composerButton->setText ("Composer");
    composerButton->setFocusPolicy ( Qt::NoFocus );
    composerButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    composerButton->setIcon(QIcon(":/icons/composer.png"));
    composerButton->setMinimumHeight ( 40 );
    composerButton->setMaximumWidth ( 250 );
    composerButton->setMinimumWidth ( 250 );
    composerButton->setToolTip("Opens the composer workspace");
    composerButton->setIdentifier("composer");
    workspaceButtonsLayoutBasic->addWidget ( composerButton );
    workspaceButtonsLayoutBasic->addSpacing(10);
    QObject::connect ( composerButton, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowComposer() ) );

    QVBoxLayout * workspaceButtonsLayoutMethodology = new QVBoxLayout;
    workspaceButtonsLayoutMethodology->setSpacing ( 10 );
    QLabel * workspaceLabelMethodology = new QLabel ( "<b>Methodology</b>" );
    workspaceLabelMethodology->setTextFormat(Qt::RichText);
    workspaceLabelMethodology->setAlignment(Qt::AlignHCenter);
    workspaceLabelMethodology->setMargin(10);
    workspaceButtonsLayoutMethodology->addWidget ( workspaceLabelMethodology );

    QVBoxLayout * workspaceButtonsLayoutClinical = new QVBoxLayout;
    workspaceButtonsLayoutClinical->setSpacing ( 10 );
    QLabel * workspaceLabelClinical = new QLabel ( "<b>Clinical</b>" );
    workspaceLabelClinical->setTextFormat(Qt::RichText);
    workspaceLabelClinical->setAlignment(Qt::AlignHCenter);
    workspaceLabelClinical->setMargin(10);
    workspaceButtonsLayoutClinical->addWidget ( workspaceLabelClinical );

    QVBoxLayout * workspaceButtonsLayoutOther = new QVBoxLayout;
    workspaceButtonsLayoutOther->setSpacing ( 10 );
    QLabel * workspaceLabelOther = new QLabel ( "<b>Other</b>" );
    workspaceLabelOther->setTextFormat(Qt::RichText);
    workspaceLabelOther->setAlignment(Qt::AlignHCenter);
    workspaceLabelOther->setMargin(10);
    workspaceButtonsLayoutOther->addWidget ( workspaceLabelOther );

    foreach ( medWorkspaceFactory::Details* detail, workspaceDetails)
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
            workspaceButtonsLayoutBasic->addWidget ( button );
            workspaceButtonsLayoutBasic->addSpacing(10);
        }
        else if(!detail->category.compare("Methodology")) 
        {
            workspaceButtonsLayoutMethodology->addWidget ( button );
            workspaceButtonsLayoutMethodology->addSpacing(10);
        }
        else if(!detail->category.compare("Clinical")) 
        {
            workspaceButtonsLayoutClinical->addWidget ( button );
            workspaceButtonsLayoutClinical->addSpacing(10);
        }
        else 
        {
            workspaceButtonsLayoutOther->addWidget ( button );
            workspaceButtonsLayoutOther->addSpacing(10);
        }
    }
    workspaceButtonsLayoutBasic->addStretch();
    workspaceButtonsLayoutMethodology->addStretch();
    workspaceButtonsLayoutClinical->addStretch();
    workspaceButtonsLayoutOther->addStretch();

    QGridLayout* workspaceButtonsLayout = new QGridLayout;
    std::vector<QLayout*> oLayoutVect;

    if (workspaceButtonsLayoutBasic->count() > 2) oLayoutVect.push_back(workspaceButtonsLayoutBasic);
    if (workspaceButtonsLayoutMethodology->count() > 2) oLayoutVect.push_back(workspaceButtonsLayoutMethodology);
    if (workspaceButtonsLayoutClinical->count() > 2) oLayoutVect.push_back(workspaceButtonsLayoutClinical);
    if (workspaceButtonsLayoutOther->count() > 2) oLayoutVect.push_back(workspaceButtonsLayoutOther);

    for (int i = 0; i < static_cast<int>(oLayoutVect.size()); ++i)
    {
        workspaceButtonsLayout->setColumnMinimumWidth(i, 120);
        workspaceButtonsLayout->addLayout(oLayoutVect[i], 0, i);
    }

    workspaceButtonsLayout->setSpacing(40);
    d->navigationWidget->setLayout ( workspaceButtonsLayout );
}

void medHomepageArea::onShowBrowser()
{
    emit showBrowser();
}

void medHomepageArea::onShowWorkspace ( QString workspace )
{
    emit showWorkspace ( workspace );
}

void medHomepageArea::onShowAbout()
{
    d->stackedWidget->setCurrentWidget(d->aboutWidget);
    d->aboutWidget->setFocus();
}

void medHomepageArea::onShowPlugin()
{
    d->stackedWidget->setCurrentWidget(d->pluginWidget);

    d->pluginWidget->setFocus();
}

void medHomepageArea::onShowInfo()
{
    d->stackedWidget->setCurrentWidget(d->infoWidget);
    d->infoWidget->setFocus();
}

void medHomepageArea::onShowHelp()
{
    QDesktopServices::openUrl(QUrl("http://med.inria.fr/help/documentation"));
}

void medHomepageArea::onShowSettings()
{
    d->settingsEditor->setTabPosition(QTabWidget::North);
    d->settingsEditor->initialize();
    d->settingsEditor->queryWidgets();
    d->stackedWidget->setCurrentWidget(d->settingsWidget);

    d->settingsWidget->setFocus();
}

void medHomepageArea::onShowComposer()
{
    emit showComposer();
}

void medHomepageArea::openLogDirectory()
{
    QString path = QFileInfo(dtkLogPath(qApp)).path();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
