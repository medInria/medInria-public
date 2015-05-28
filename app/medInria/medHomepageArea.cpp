/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medHomepageArea.h>
#ifdef MEDINRIA_HAS_REVISIONS
    #include <medRevisions.h>
#endif

#include <medHomepageButton.h>
#include <medAbstractWorkspace.h>
#include <medToolBoxFactory.h>
#include <medWorkspaceFactory.h>
#include <medSettingsManager.h>
#include <medPluginWidget.h>
#include <medSettingsEditor.h>


class medHomepageAreaPrivate
{
public:
    QStackedWidget* stackedWidget;
    QWidget * navigationWidget;
    QPropertyAnimation * navigationAnimation;

    QWidget * userWidget;
    QPropertyAnimation * userAnimation;

    QWidget * infoWidget;
    QPropertyAnimation * infoAnimation;

    QWidget * aboutWidget;
    QTabWidget * aboutTabWidget;
    QWidget * pluginWidget;
    QWidget * settingsWidget;
    medSettingsEditor* settingsEditor;

    QParallelAnimationGroup * animation;
};

medHomepageArea::medHomepageArea ( QWidget * parent ) : QWidget ( parent ), d ( new medHomepageAreaPrivate )
{
    //Setup navigation widget (with buttons for accessing available workspaces)
    d->navigationWidget = new QWidget ( this );
    d->navigationWidget->setMinimumWidth ( 250 );

    //Setup the widget where the medInria general information are displayed
    d->infoWidget = new QWidget ( this );
    d->infoWidget->setMinimumSize(400,300);

    //Setup the widget with about, settings, plugins and documentation buttons
    d->userWidget = new QWidget ( this );
    d->userWidget->setMinimumWidth ( 250 );
    d->userWidget->setMaximumWidth ( 350 ); //TODO: find the right solution
    d->userWidget->setMinimumHeight ( 40 );

    //Setup the about container widget (with a QTabWidget inside)
    d->aboutWidget = new QWidget ( this );
    d->aboutWidget->setMinimumSize(400,300);

    d->aboutWidget->hide();

//#if 0
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
    helpButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( helpButton,SIGNAL ( clicked() ),this, SLOT ( onShowHelp() ) );

    medHomepageButton * aboutButton = new medHomepageButton ( this );
    aboutButton->setText ( "About" );
    aboutButton->setMinimumHeight ( 30 );
    aboutButton->setMaximumWidth ( 150 );
    aboutButton->setMinimumWidth ( 150 );
    aboutButton->setToolTip(tr("About MUSIC"));
    aboutButton->setFocusPolicy ( Qt::NoFocus );
    aboutButton->setIcon ( QIcon ( ":icons/about.png" ) );
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

    medHomepageButton * settingsButton = new medHomepageButton ( this );
    settingsButton->setText ( "Settings" );
    settingsButton->setMinimumHeight ( 30 );
    settingsButton->setMaximumWidth ( 150 );
    settingsButton->setMinimumWidth ( 150 );
    settingsButton->setToolTip(tr("Configure MUSIC"));
    settingsButton->setFocusPolicy ( Qt::NoFocus );
    settingsButton->setIcon ( QIcon ( ":icons/settings.svg" ) );
    settingsButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( settingsButton,SIGNAL ( clicked() ),this, SLOT ( onShowSettings() ) );

    userButtonsLayout->insertWidget ( 0, settingsButton );
    userButtonsLayout->insertWidget ( 1, pluginButton );
    userButtonsLayout->insertWidget ( 2, aboutButton );
    userButtonsLayout->insertWidget ( 3, helpButton );
//#endif
    //no need to set the layout the userWidget is the parent of the layout already.
//    d->userWidget->setLayout ( userButtonsLayout );

    // Info widget : medInria logo, medInria description, etc. QtWebkit ?
    QVBoxLayout * infoLayout = new QVBoxLayout(d->infoWidget);
    QLabel * medInriaLabel = new QLabel ( this );
    QPixmap medLogo( ":music_logo.png" );
    medLogo = medLogo.scaled(350, 150, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    medInriaLabel->setPixmap ( medLogo );
//     QLabel * textLabel = new QLabel;

    QTextEdit * textEdit = new QTextEdit(this);
    textEdit->setHtml ( tr("<b>MUSIC</b> is a software developed in collaboration with "
                           "the IHU LIRYC in order to propose functionalities "
                           "dedicated to cardiac interventional planning and "
                           "guidance, based on the medInria software platform."
                           "<br/><br/>"
                           "<b>MUSIC</b> is proprietary software, copyright Inria - IHU Liryc 2014." ));
    textEdit->setReadOnly ( true );
    textEdit->setFocusPolicy ( Qt::NoFocus );
    textEdit->setMaximumHeight(300);
    textEdit->setMinimumHeight(250);
    infoLayout->insertWidget ( 0,medInriaLabel );
    infoLayout->insertWidget ( 1, textEdit );
    infoLayout->addStretch();

    //no need to set the layout, the infoWidget is the parent of the layout already.
//    d->infoWidget->setLayout ( infoLayout );
    d->infoWidget->setMaximumHeight ( medInriaLabel->height() + textEdit->height() );

    //About widget
    QVBoxLayout * aboutLayout = new QVBoxLayout(d->aboutWidget);
    d->aboutTabWidget = new QTabWidget(this);
    d->aboutTabWidget->setObjectName("aboutTabWidget");

    QLabel * medInriaLabel2 = new QLabel ( this );
    medInriaLabel2->setPixmap ( medLogo );

    QTextEdit * aboutTextEdit = new QTextEdit(this);
    
    QString aboutText = QString(tr("<br/><br/>"
                      "MUSIC %1 is the cardiac imaging platform based on medInria developed at "
                      "Inria and IHU LIRYC.<br/><br/>"
                      "<center>Inria & IHU LIRYC, Copyright 2013</center>"))
                      .arg(qApp->applicationVersion());
    
#ifdef MEDINRIA_HAS_REVISIONS
    aboutText += QString::fromLocal8Bit(REVISIONS);
#endif

    
    aboutTextEdit->setHtml (aboutText);
    aboutTextEdit->setFocusPolicy ( Qt::NoFocus );

    QTextBrowser * aboutAuthorTextBrowser = new QTextBrowser(this);
    aboutAuthorTextBrowser->setSource(QUrl("qrc:authors.html" ));
    aboutAuthorTextBrowser->setFocusPolicy ( Qt::NoFocus );

    QTextEdit * aboutLicenseTextEdit = new QTextEdit(this);
    QFile license ( ":LICENSE.txt" );
    license.open ( QIODevice::ReadOnly | QIODevice::Text );
    QString licenseContent = license.readAll();
    license.close();
    aboutLicenseTextEdit->setText ( licenseContent );
    aboutLicenseTextEdit->setFocusPolicy ( Qt::NoFocus );

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

    d->aboutTabWidget->addTab ( aboutTextEdit,          tr("About") );
    d->aboutTabWidget->addTab ( aboutAuthorTextBrowser, tr("Authors") );
    d->aboutTabWidget->addTab ( releaseNotesTextEdit,   tr("Release Notes") );
    d->aboutTabWidget->addTab ( aboutLicenseTextEdit,   tr("License") );

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


    //Create the settings widget.
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

    //Setup homepage animations
    d->animation = new QParallelAnimationGroup ( this );
    d->navigationAnimation = new QPropertyAnimation ( d->navigationWidget, "pos" );
    d->navigationAnimation->setDuration ( 750 );
    d->navigationAnimation->setEasingCurve ( QEasingCurve::OutCubic );
    d->navigationAnimation->setStartValue ( QPoint ( - 300,  this->height() / 4 ) );
    d->navigationAnimation->setEndValue ( QPoint ( 100 ,  this->height() / 4 ) );

    d->userAnimation = new QPropertyAnimation ( d->userWidget, "pos" );
    d->userAnimation->setDuration ( 750 );
    d->userAnimation->setEasingCurve ( QEasingCurve::OutCubic );
    d->userAnimation->setStartValue ( QPoint ( this->width() + 250,  this->height() - 90 ) );
    d->userAnimation->setEndValue ( QPoint ( this->width() * 0.8 ,  this->height() - 90 ) );

    d->infoAnimation = new QPropertyAnimation ( d->stackedWidget, "pos" );
    d->infoAnimation->setDuration ( 750 );
    d->infoAnimation->setEasingCurve ( QEasingCurve::OutCubic );
    d->infoAnimation->setStartValue ( QPoint ( this->width() + 100 , this->height() / 5 ) );
    d->infoAnimation->setEndValue ( QPoint ( this->width() / 2 ,  this->height() / 5 ) );

    d->animation->addAnimation ( d->navigationAnimation );
    d->animation->addAnimation ( d->userAnimation );
    d->animation->addAnimation ( d->infoAnimation );

}

medHomepageArea::~medHomepageArea()
{
    delete d;

    d = NULL;
}

void medHomepageArea::resizeEvent ( QResizeEvent * event )
{
    //Recompute the widgets position when the window is resized
    d->navigationWidget->setProperty ( "pos", QPoint ( 100 ,  this->height() / 4 ) );
    d->userWidget->setProperty ( "pos", QPoint ( this->width() - 350 ,  this->height() - 90 ) );
    d->stackedWidget->setProperty ( "pos", QPoint ( this->width() / 2 ,  this->height() / 5 ) );

    int stackedWidgetHeight = d->userWidget->pos().y() - d->stackedWidget->pos().y();
    if (d->stackedWidget->minimumHeight() > stackedWidgetHeight)
        stackedWidgetHeight = d->stackedWidget->minimumHeight();
    d->stackedWidget->setMaximumHeight(stackedWidgetHeight);

    int stackedWidgetWidth =  this->width() / 2 - 50 ;

    d->stackedWidget->setMaximumWidth(stackedWidgetWidth);
    //TODO: this is ugly, find a way to use the right policy here...
    d->stackedWidget->resize(stackedWidgetWidth,stackedWidgetHeight);
//    qDebug()<< d->stackedWidget->maximumSize();

    //Update the animations as well
    d->navigationAnimation->setStartValue ( QPoint ( - 300,  this->height() / 4 ) );
    d->navigationAnimation->setEndValue ( QPoint ( 100 ,  this->height() / 4 ) );

    d->userAnimation->setStartValue ( QPoint ( this->width() + 50,  this->height() - 90 ) );
    d->userAnimation->setEndValue ( QPoint ( this->width() - 350 ,  this->height() - 90 ) );

    d->infoAnimation->setStartValue ( QPoint ( this->width() , this->height() / 5 ) );
    d->infoAnimation->setEndValue ( QPoint ( this->width() / 2 ,  this->height() / 5 ) );
}

void medHomepageArea::initPage()
{
    //Initialization of the navigation widget with available workspaces
    QList<medWorkspaceFactory::Details*> workspaceDetails = medWorkspaceFactory::instance()->workspaceDetailsSortedByName();
    QVBoxLayout * workspaceButtonsLayout = new QVBoxLayout;
    workspaceButtonsLayout->setSpacing ( 10 );
    QLabel * workspaceLabel = new QLabel ( "<b>Available workspaces</b>" );
    workspaceLabel->setTextFormat(Qt::RichText);
    workspaceLabel->setAlignment(Qt::AlignHCenter);
    workspaceButtonsLayout->addWidget ( workspaceLabel );

    medHomepageButton * browserButton = new medHomepageButton ( this );
    browserButton->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
    browserButton->setIcon ( QIcon ( ":/icons/folder.png" ) );
    browserButton->setText ( "Browser" );
    browserButton->setMinimumHeight ( 40 );
    browserButton->setMaximumWidth ( 250 );
    browserButton->setMinimumWidth ( 250 );
    browserButton->setFocusPolicy ( Qt::NoFocus );
    workspaceButtonsLayout->addWidget ( browserButton );
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SLOT ( onShowBrowser() ) );

    foreach ( medWorkspaceFactory::Details* detail, workspaceDetails)
    {
        medHomepageButton * button = new medHomepageButton ( this );
        button->setText ( detail->name );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
        button->setMinimumHeight ( 40 );
        button->setMaximumWidth ( 250 );
        button->setMinimumWidth ( 250 );
        button->setToolTip(detail->description);
        button->setIdentifier(detail->identifier);
        workspaceButtonsLayout->addWidget ( button );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowWorkspace ( QString ) ) );
        if (!(medWorkspaceFactory::instance()->isUsable(detail->identifier)))
        {
            button->setDisabled(true);
            button->setToolTip("No useful plugin has been found for this workspace.");
        }
    }
    workspaceButtonsLayout->addStretch();
    d->navigationWidget->setLayout ( workspaceButtonsLayout );
    d->navigationWidget->setProperty ( "pos", QPoint ( 100,  100 ) );
    d->navigationWidget->setMinimumHeight ( 55 * ( 1 + workspaceDetails.size() ) );
}

QParallelAnimationGroup* medHomepageArea::getAnimation()
{
    return d->animation;
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


