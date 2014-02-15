/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainerSplitter.h>

#include <QDebug>

#include <medViewContainer2.h>

class medViewContainerSplitterPrivate
{
    QList <medViewContainer2*> containers;
};


medViewContainerSplitter::medViewContainerSplitter(QWidget *parent):
    d(new medViewContainerSplitterPrivate)
{
    this->setOrientation(Qt::Vertical);
}

medViewContainerSplitter::~medViewContainerSplitter()
{
    qDebug() << "deleting container splitter";
    delete d;
}


void medViewContainerSplitter::hSplit()
{
    qDebug()<< "hsplit!!!";
    medViewContainer2* container = dynamic_cast<medViewContainer2*>(this->sender());
    if(!container)
        return;
    int index = this->indexOf(container);

    if(this->orientation() == Qt::Vertical)
    {
        this->insertViewContainer(index + 1, new medViewContainer2);
    }
    else
    {
        container->hide();
        container->disconnect(this);
        medViewContainerSplitter *spliter = new medViewContainerSplitter;
        spliter->setOrientation(Qt::Vertical);
        spliter->addViewContainer(container);
        container->show();
        spliter->addViewContainer(new medViewContainer2);
        this->insertWidget(index, spliter);
    }
}

void medViewContainerSplitter::vSplit()
{
    qDebug()<< "vsplit!!!";
    medViewContainer2* container = dynamic_cast<medViewContainer2*>(this->sender());
    if(!container)
        return;
    int index = this->indexOf(container);

    if(this->orientation() == Qt::Horizontal)
    {
        this->insertViewContainer(index + 1, new medViewContainer2);
    }
    else
    {
        container->hide();
        container->disconnect(this);
        int index = this->indexOf(container);
        medViewContainerSplitter *spliter = new medViewContainerSplitter;
        spliter->setOrientation(Qt::Horizontal);
        spliter->addViewContainer(container);
        container->show();
        spliter->addViewContainer(new medViewContainer2);
        this->insertWidget(index, spliter);
    }
}


void medViewContainerSplitter::checkIfStillDeserveToLive()
{
    //Deserve to live if there is more than one widget in it
    if(this->count() < 1)
        this->~medViewContainerSplitter();
}

void medViewContainerSplitter::insertViewContainer(int index, medViewContainer2 *container)
{
    connect(container, SIGNAL(hSplitRequest()), this, SLOT(hSplit()));
    connect(container, SIGNAL(vSplitRequest()), this, SLOT(vSplit()));
    connect(container, SIGNAL(destroyed()), this, SLOT(checkIfStillDeserveToLive()));

    this->insertWidget(index, container);
}

void medViewContainerSplitter::addViewContainer(medViewContainer2 *container)
{
    this->insertViewContainer(this->count(), container);
}
