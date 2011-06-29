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

#include <medViewerArea.h>
#include <medHomepageButton.h>
#include <medGui/medViewerConfiguration.h>
#include <medGui/medViewerConfigurationFactory.h>
#include <medCore/medSettingsManager.h>

class medHomepageAreaPrivate
{
public:
    QWidget * navigationWidget;
    QPropertyAnimation * navigationAnimation;

    QWidget * userWidget;
    QPropertyAnimation * userAnimation;

    QWidget * infoWidget;
    QPropertyAnimation * infoAnimation;

    QWidget * aboutWidget;

    QParallelAnimationGroup * animation;

    medViewerArea * viewerArea;
};

medHomepageArea::medHomepageArea ( QWidget * parent ) : QWidget ( parent ), d ( new medHomepageAreaPrivate )
{
    d->viewerArea = NULL;
    setupUi ( this );

    d->navigationWidget = new QWidget ( this );
    d->navigationWidget->setMinimumWidth ( 250 );
    d->navigationWidget->setMinimumHeight ( 400 );

    d->userWidget = new QWidget ( this );
    d->userWidget->setMinimumWidth ( 250 );
    d->userWidget->setMinimumHeight ( 40 );

    d->infoWidget = new QWidget ( this );
    d->infoWidget->setMinimumWidth ( 400 );
    d->infoWidget->setMinimumHeight ( 500 );

    QHBoxLayout * userButtonsLayout = new QHBoxLayout;
    medHomepageButton * helpButton = new medHomepageButton ( this );
    helpButton->setText ( "Help" );
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
    aboutButton->setFocusPolicy ( Qt::NoFocus );
    aboutButton->setIcon ( QIcon ( ":icons/about.png" ) );
    aboutButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( aboutButton,SIGNAL ( clicked() ),this, SLOT ( onShowAbout() ) );

    medHomepageButton * settingsButton = new medHomepageButton ( this );
    settingsButton->setText ( "Settings" );
    settingsButton->setMinimumHeight ( 30 );
    settingsButton->setMaximumWidth ( 150 );
    settingsButton->setMinimumWidth ( 150 );
    settingsButton->setFocusPolicy ( Qt::NoFocus );
    settingsButton->setIcon ( QIcon ( ":icons/settings.svg" ) );
    settingsButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
    QObject::connect ( settingsButton,SIGNAL ( clicked() ),this, SLOT ( onShowSettings() ) );

    userButtonsLayout->insertWidget ( 0, settingsButton );
    userButtonsLayout->insertWidget ( 1, aboutButton );
    userButtonsLayout->insertWidget ( 2, helpButton );

    d->userWidget->setLayout ( userButtonsLayout );

    //Special widget : image, text, etc. QtWebkit ?
    QVBoxLayout * infoLayout = new QVBoxLayout;
    QLabel * medinriaLabel = new QLabel ( this );
    medinriaLabel->setPixmap ( QPixmap ( ":pixmaps/medinria-logo-homepage.png" ) );
    QLabel * textLabel = new QLabel;

    QTextEdit * textEdit = new QTextEdit;
    textEdit->setHtml ( "<b>medINRIA</b> is a multi-platform medical image processing and visualization software,\
                      and it's <b>free</b>. Through an intuitive user interface, <b>medINRIA</b> offers from standard \
                      to cutting-edge processing functionalities for your medical images such as 2D/3D/4D image visualization, \
                      image registration, or diffusion MR processing and tractography." );
    textEdit->setReadOnly ( true );
    textEdit->setStyleSheet ( "background: #313131;border: 0px;padding: 0px 0px 0px 0px;" );
    textEdit->setFocusPolicy ( Qt::NoFocus );
    textEdit->setMaximumHeight ( 200 );
    infoLayout->insertWidget ( 0,medinriaLabel );
    infoLayout->insertWidget ( 1, textEdit );
    infoLayout->addStretch();

    d->infoWidget->setLayout ( infoLayout );
    d->infoWidget->setMaximumHeight ( medinriaLabel->height() + textEdit->height() );

    //Set the position of the widgets
    d->navigationWidget->setProperty ( "pos", QPoint ( 100 ,  this->height() / 3 ) );
    d->userWidget->setProperty ( "pos", QPoint ( this->width() - 350 ,  this->height() - 100 ) );
    d->infoWidget->setProperty ( "pos", QPoint ( this->width() / 2 ,  this->height() / 3 ) );

    d->animation = new QParallelAnimationGroup ( this );
    d->navigationAnimation = new QPropertyAnimation ( d->navigationWidget, "pos" );
    d->navigationAnimation->setDuration ( 750 );
    d->navigationAnimation->setEasingCurve ( QEasingCurve::OutCubic );
    d->navigationAnimation->setStartValue ( QPoint ( - 300,  this->height() / 4 ) );
    d->navigationAnimation->setEndValue ( QPoint ( 100 ,  this->height() / 4 ) );

    d->userAnimation = new QPropertyAnimation ( d->userWidget, "pos" );
    d->userAnimation->setDuration ( 750 );
    d->userAnimation->setEasingCurve ( QEasingCurve::OutCubic );
    d->userAnimation->setStartValue ( QPoint ( this->width() + 250,  this->height() - 100 ) );
    d->userAnimation->setEndValue ( QPoint ( this->width() * 0.8 ,  this->height() - 100 ) );

    d->infoAnimation = new QPropertyAnimation ( d->infoWidget, "pos" );
    d->infoAnimation->setDuration ( 900 );
    d->infoAnimation->setEasingCurve ( QEasingCurve::OutCubic );
    d->infoAnimation->setStartValue ( QPoint ( this->width() + 100 , this->height() / 4 ) );
    d->infoAnimation->setEndValue ( QPoint ( this->width() / 2 ,  this->height() / 4 ) );

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
    d->navigationWidget->setProperty ( "pos", QPoint ( 100 ,  this->height() / 4 ) );
    d->userWidget->setProperty ( "pos", QPoint ( this->width() - 350 ,  this->height() - 100 ) );
    d->infoWidget->setProperty ( "pos", QPoint ( this->width() / 2 ,  this->height() / 4 ) );

    d->navigationAnimation->setStartValue ( QPoint ( - 300,  this->height() / 4 ) );
    d->navigationAnimation->setEndValue ( QPoint ( 100 ,  this->height() / 4 ) );

    d->userAnimation->setStartValue ( QPoint ( this->width() + 50,  this->height() - 100 ) );
    d->userAnimation->setEndValue ( QPoint ( this->width() - 350 ,  this->height() - 100 ) );

    d->infoAnimation->setStartValue ( QPoint ( this->width() , this->height() / 4 ) );
    d->infoAnimation->setEndValue ( QPoint ( this->width() / 2 ,  this->height() / 4 ) );
}

void medHomepageArea::initPage ( void )
{
    QList<QString> configList = medViewerConfigurationFactory::instance()->configurations();
    QVBoxLayout * configurationButtonsLayout = new QVBoxLayout;
    configurationButtonsLayout->setSpacing ( 10 );
    QLabel * configurationLabel = new QLabel ( "Available workspaces:" );
    configurationButtonsLayout->addWidget ( configurationLabel );

    medHomepageButton * browserButton = new medHomepageButton ( this );
    browserButton->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
    browserButton->setIcon ( QIcon ( ":/icons/folder.png" ) );
    browserButton->setText ( "Browser" );
    browserButton->setMinimumHeight ( 40 );
    browserButton->setMaximumWidth ( 250 );
    browserButton->setMinimumWidth ( 250 );
    browserButton->setFocusPolicy ( Qt::NoFocus );
    configurationButtonsLayout->addWidget ( browserButton );
    QObject::connect ( browserButton, SIGNAL ( clicked() ),this, SLOT ( onShowBrowser() ) );

    for ( int i = 0; i< configList.size(); i++ )
    {
        medHomepageButton * button = new medHomepageButton ( this );
        button->setText ( configList.at ( i ) );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
        button->setMinimumHeight ( 40 );
        button->setMaximumWidth ( 250 );
        button->setMinimumWidth ( 250 );;
        configurationButtonsLayout->addWidget ( button );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowConfiguration ( QString ) ) );
    }
    configurationButtonsLayout->addStretch();
    d->navigationWidget->setLayout ( configurationButtonsLayout );
    d->navigationWidget->setProperty ( "pos", QPoint ( 100,  100 ) );

    //Setup the startup checkbox
    if ( medSettingsManager::instance()->value ( "startup","default_starting_area" ).toInt() )
        showOnStartupCheckBox->setCheckState ( Qt::Unchecked );
    QObject::connect ( showOnStartupCheckBox, SIGNAL ( stateChanged ( int ) ), this, SLOT ( onStartWithHomepage ( int ) ) );
}

QParallelAnimationGroup* medHomepageArea::getAnimation ( void )
{
    return d->animation;
}

void medHomepageArea::setViewerArea ( medViewerArea* viewer )
{
    d->viewerArea = viewer;
}


void medHomepageArea::onShowBrowser ( void )
{
    emit showBrowser();
}

void medHomepageArea::onShowConfiguration ( QString configuration )
{
    emit showViewer();
    emit showConfiguration ( configuration );
}

void medHomepageArea::onShowAbout ( void )
{
    QString aboutMessage ( "This is an ultimate medical imaging platform" );
    QMessageBox::about ( this,"About Medinria", aboutMessage );
}

void medHomepageArea::onHideAbout()
{

}

void medHomepageArea::onShowHelp ( void )
{
    QMessageBox * msgBox = new QMessageBox ( QApplication::activeWindow() );
    msgBox->setIcon ( QMessageBox::Information );
    msgBox->setText ( "Help ! Help !" );
    msgBox->exec();
    delete msgBox;
}

void medHomepageArea::onShowSettings ( void )
{
    emit showSettings();
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


