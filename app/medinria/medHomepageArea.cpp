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

class medHomepageAreaPrivate
{
public:
    QGraphicsView * view;
    QGraphicsScene * scene;
    QPropertyAnimation * animation;
    QWidget * buttonWidget;

    medViewerArea * viewerArea;
};

medHomepageArea::medHomepageArea ( QWidget * parent ) : QWidget ( parent ), d ( new medHomepageAreaPrivate )
{
    //Hack modifications from alex
    d->viewerArea = NULL;
    d->buttonWidget = new QWidget ( this );
    d->buttonWidget->setMinimumWidth ( 200 );
    d->view = new QGraphicsView ( this );
    d->scene = new QGraphicsScene ( this );
    d->view->setScene ( d->scene );
    d->view->setStyleSheet ( "background: #4b4b4b;border: 0px;padding: 0px 0px 0px 0px;" );
    d->view->setFocusPolicy ( Qt::NoFocus );
}

medHomepageArea::~medHomepageArea()
{
    delete d;

    d = NULL;
}

void medHomepageArea::initPage ( void )
{
    QList<QString> configList = medViewerConfigurationFactory::instance()->configurations();
    QGridLayout * buttonLayout = new QGridLayout ( this );

//     QPushButton * buttonBrowser = new QPushButton ( this );
    medHomepageButton * buttonBrowser = new medHomepageButton ( this );
    buttonBrowser->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    buttonBrowser->setIcon(QIcon(":/icons/folder.png"));
    buttonBrowser->setText ( "Browser" );
    buttonBrowser->setMinimumHeight ( 30 );
    buttonBrowser->setMaximumWidth ( 200 );
    buttonBrowser->setMinimumWidth ( 200 );
    buttonBrowser->setFocusPolicy ( Qt::NoFocus );
    buttonLayout->addWidget ( buttonBrowser,0,0 );
    QObject::connect ( buttonBrowser, SIGNAL ( clicked() ),this, SLOT ( onShowBrowser() ) );

    for ( int i = 0; i< configList.size(); i++ )
    {
//         QPushButton * button = new QPushButton ( this );
        medHomepageButton * button = new medHomepageButton ( this );
        button->setText ( configList.at ( i ) );
        button->setFocusPolicy ( Qt::NoFocus );
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setMinimumHeight ( 30 );
        button->setMaximumWidth ( 200 );
        button->setMinimumWidth ( 200 );;
        buttonLayout->addWidget ( button,i + 1,0 );
        QObject::connect ( button, SIGNAL ( clicked ( QString ) ),this, SLOT ( onShowConfiguration ( QString ) ) );
    }
    d->buttonWidget->setLayout ( buttonLayout );
    d->scene->addWidget ( d->buttonWidget );

    d->animation = new QPropertyAnimation ( d->buttonWidget, "pos" );
    d->animation->setDuration ( 750 );
    d->animation->setEasingCurve ( QEasingCurve::OutCubic );
    d->animation->setStartValue ( QPoint ( ( d->scene->width() / 2 ) - 250, 250 ) );
    d->animation->setEndValue ( QPoint ( ( d->scene->width() / 2 ) + 100 ,  250 ) );
}

QPropertyAnimation* medHomepageArea::getAnimation ( void )
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
    emit showConfiguration(configuration);
}



