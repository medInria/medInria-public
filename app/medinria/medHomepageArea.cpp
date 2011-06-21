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

#include <medGui/medViewerConfiguration.h>
#include <medGui/medViewerConfigurationFactory.h>

class medHomepageAreaPrivate
{
public:
    QGraphicsView * view;
    QGraphicsScene * scene;
};

medHomepageArea::medHomepageArea(QWidget * parent) : QWidget(parent), d(new medHomepageAreaPrivate)
{
    //Hack modifications from alex
    d->view = new QGraphicsView(this);
    d->scene = new QGraphicsScene(this);
    d->view->setScene(d->scene);
    d->view->setStyleSheet("background: #4b4b4b;");
}

medHomepageArea::~medHomepageArea()
{

}

void medHomepageArea::generateConfigurationButtons ( void )
{
    QWidget * buttonWidget = new QWidget;
    buttonWidget->setStyleSheet("background: #4b4b4b;");
    QList<QString> configList = medViewerConfigurationFactory::instance()->configurations();
    QGridLayout * buttonLayout = new QGridLayout(this);

    QPushButton * buttonBrowser = new QPushButton(this);
    buttonBrowser->setText("Browser");
    buttonLayout->addWidget(buttonBrowser,0,0);

    for (int i = 0; i< configList.size(); i++)
    {
        QPushButton * button = new QPushButton(this);
        button->setText(configList.at(i));
        buttonLayout->addWidget(button,i + 1,0);
    }
    buttonWidget->setLayout(buttonLayout);
    d->scene->addWidget(buttonWidget);

}

