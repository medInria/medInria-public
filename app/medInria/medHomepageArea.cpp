/*
    Copyright (c) 2011, Alexandre Abadie <Alexandre.Abadie@irisa.fr>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Alexandre <email> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Alexandre <email> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "medHomepageArea.h"

#include <medHomepageButton.h>
#include <medWorkspace.h>
#include <medToolBoxFactory.h>
#include <medWorkspaceFactory.h>
#include <medSettingsManager.h>
#include "medPluginWidget.h"
#include <medSettingsEditor.h>


class medHomepageAreaPrivate
{
public:
    QCheckBox * showOnStartupCheckBox;
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
    d->userWidget->setMinimumHeight ( 40 );

    //Setup the about container widget (with a QTabWidget inside)
    d->aboutWidget = new QWidget ( this );
    d->aboutWidget->setMaximumHeight ( 500 );

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
    helpButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( helpButton,SIGNAL ( clicked() ),this, SLOT ( onShowHelp() ) );

    medHomepageButton * aboutButton = new medHomepageButton ( this );
    aboutButton->setText ( "About" );
    aboutButton->setMinimumHeight ( 30 );
    aboutButton->setMaximumWidth ( 150 );
    aboutButton->setMinimumWidth ( 150 );
    aboutButton->setToolTip(tr("About medInria"));
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
    settingsButton->setToolTip(tr("Configure medInria"));
    settingsButton->setFocusPolicy ( Qt::NoFocus );
    settingsButton->setIcon ( QIcon ( ":icons/settings.svg" ) );
    settingsButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( settingsButton,SIGNAL ( clicked() ),this, SLOT ( onShowSettings() ) );

    userButtonsLayout->insertWidget ( 0, settingsButton );
    userButtonsLayout->insertWidget ( 1, pluginButton );
    userButtonsLayout->insertWidget ( 2, aboutButton );
    userButtonsLayout->insertWidget ( 3, helpButton );
    //no need to set the layout the userWidget is the parent of the layout already.
//    d->userWidget->setLayout ( userButtonsLayout );

    // Info widget : medInria logo, medInria description, etc. QtWebkit ?
    QVBoxLayout * infoLayout = new QVBoxLayout(d->infoWidget);
    QLabel * medInriaLabel = new QLabel ( this );
    QPixmap medLogo( ":pixmaps/medInria-logo-homepage.png" );
    medInriaLabel->setPixmap ( medLogo );
//     QLabel * textLabel = new QLabel;

    QTextEdit * textEdit = new QTextEdit(this);
    textEdit->setHtml ( tr("<b>medInria</b> is a multi-platform medical image "
                           "processing and visualization software, "
                           "and it's <b>free</b>. Through an intuitive user "
                           "interface, <b>medInria</b> offers from standard "
                           "to cutting-edge processing functionalities for "
                           "your medical images such as 2D/3D/4D image "
                           "visualization, image registration, or diffusion "
                           "MR processing and tractography." ));
    textEdit->setReadOnly ( true );
    textEdit->setFocusPolicy ( Qt::NoFocus );
    textEdit->setMaximumHeight ( 200 );
    infoLayout->insertWidget ( 0,medInriaLabel );
    infoLayout->insertWidget ( 1, textEdit );
    infoLayout->addStretch();

    //no need to set the layout, the infoWidget is the parent of the layout already.
//    d->infoWidget->setLayout ( infoLayout );
    d->infoWidget->setMaximumHeight ( medInriaLabel->height() + textEdit->height() );

    //About widget
    QVBoxLayout * aboutLayout = new QVBoxLayout(d->aboutWidget);
    d->aboutTabWidget = new QTabWidget(this);

    QLabel * medInriaLabel2 = new QLabel ( this );
    medInriaLabel2->setPixmap ( medLogo );

    QTextEdit * aboutTextEdit = new QTextEdit(this);
    QString aboutText(tr("<br/><br/>"
                      "medInria is the medical imaging platform developed at "
                      "Inria<br/><br/>"
                      "<center>Inria, Copyright 2011</center><br/><br/><br/>"
                      "Version: "));
    aboutText+= qApp->applicationVersion();
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

    d->aboutTabWidget->addTab ( aboutTextEdit, tr("About") );
    d->aboutTabWidget->addTab ( aboutAuthorTextBrowser, tr("Authors") );
    d->aboutTabWidget->addTab ( releaseNotesTextEdit, tr("Release Notes") );
    d->aboutTabWidget->addTab ( aboutLicenseTextEdit, tr("License") );

    aboutLayout->addWidget ( medInriaLabel2 );
    aboutLayout->addWidget ( d->aboutTabWidget );
    aboutLayout->addLayout ( aboutButtonLayout );
    aboutLayout->addStretch();


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

    //Setup the startup checkbox
    d->showOnStartupCheckBox = new QCheckBox ( this );
    d->showOnStartupCheckBox->setCheckState(Qt::Checked);
    d->showOnStartupCheckBox->setText ( "Start medInria on the homepage?" );
    d->showOnStartupCheckBox->setFocusPolicy ( Qt::NoFocus );
    d->showOnStartupCheckBox->setProperty ( "pos", QPoint ( this->width() - 200 ,  this->height() - 30 ) );
    if ( medSettingsManager::instance()->value ( "startup","default_starting_area" ).toInt() )
        d->showOnStartupCheckBox->setCheckState ( Qt::Unchecked );
    QObject::connect ( d->showOnStartupCheckBox, SIGNAL ( stateChanged ( int ) ), this, SLOT ( onStartWithHomepage ( int ) ) );
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
    d->showOnStartupCheckBox->setProperty ( "pos", QPoint ( this->width() - 200 ,  this->height() - 30 ) );

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

void medHomepageArea::initPage ( void )
{
    //Initialization of the navigation widget with available workspaces
    QHash<QString,medWorkspaceDetails*> workspaceDetails =
            medWorkspaceFactory::instance()->workspaceDetails();
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

    foreach ( QString id, workspaceDetails.keys())
    {
        medHomepageButton * button = new medHomepageButton ( this );
        medWorkspaceDetails* detail = workspaceDetails.value(id);
        button->setText ( detail->name );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
        button->setMinimumHeight ( 40 );
        button->setMaximumWidth ( 250 );
        button->setMinimumWidth ( 250 );
        button->setToolTip(detail->description);
        button->setIdentifier(id);
        workspaceButtonsLayout->addWidget ( button );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowWorkspace ( QString ) ) );
        if (!(medWorkspaceFactory::instance()->isUsable(id)))
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

QParallelAnimationGroup* medHomepageArea::getAnimation ( void )
{
    return d->animation;
}

void medHomepageArea::onShowBrowser ( void )
{
    emit showBrowser();
}

void medHomepageArea::onShowWorkspace ( QString workspace )
{
    emit showWorkspace ( workspace );
}

void medHomepageArea::onShowAbout ( void )
{
    d->stackedWidget->setCurrentWidget(d->aboutWidget);
    d->aboutWidget->setFocus();
}

void medHomepageArea::onShowPlugin ( void )
{
    d->stackedWidget->setCurrentWidget(d->pluginWidget);

    d->pluginWidget->setFocus();
}



void medHomepageArea::onShowInfo()
{
    d->stackedWidget->setCurrentWidget(d->infoWidget);
    d->infoWidget->setFocus();
}

void medHomepageArea::onShowHelp ( void )
{
    QDesktopServices::openUrl(QUrl("http://med.inria.fr/help/documentation"));
//     QMessageBox * msgBox = new QMessageBox ( QApplication::activeWindow() );
//     msgBox->setIcon ( QMessageBox::Information );
//     msgBox->setText ( "Help ! Help !" );
//     msgBox->exec();
//     delete msgBox;
}

void medHomepageArea::onShowSettings ( void )
{
    // emit showSettings is not deprecated here
//    emit showSettings();
    d->settingsEditor->setTabPosition(QTabWidget::North);
    d->settingsEditor->initialize();
    d->settingsEditor->queryWidgets();
    d->stackedWidget->setCurrentWidget(d->settingsWidget);

    d->settingsWidget->setFocus();
}

void medHomepageArea::onStartWithHomepage ( int state )
{
    if ( state == Qt::Checked )
    {
        medSettingsManager::instance()->setValue ( "startup","default_starting_area", 0 );
    }
    else
    {
        medSettingsManager::instance()->setValue ( "startup","default_starting_area", 1 );
    }
}


