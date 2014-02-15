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
    if(this->orientation() == Qt::Vertical)
    {
        this->addViewContainer(new medViewContainer2);
    }
    else
    {
        medViewContainer2* container = dynamic_cast<medViewContainer2*>(this->sender());
        if(!container)
            return;

        container->hide();
        container->disconnect(this);
        int index = this->indexOf(container);
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
    if(this->orientation() == Qt::Horizontal)
    {
        this->addViewContainer(new medViewContainer2);
    }
    else
    {
        medViewContainer2* container = dynamic_cast<medViewContainer2*>(this->sender());
        if(!container)
            return;

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


void medViewContainerSplitter::rmSplit()
{
    qDebug() << "rmSplit !!!";
    if(this->count() > 1)
        return;

    qDebug() << "about to actaually remove it !!!";
    this->~medViewContainerSplitter();
}

void medViewContainerSplitter::addViewContainer(medViewContainer2 *container)
{
    connect(container, SIGNAL(hSplitRequest()), this, SLOT(hSplit()));
    connect(container, SIGNAL(vSplitRequest()), this, SLOT(vSplit()));
    connect(container, SIGNAL(destroyed()), this, SLOT(rmSplit()));

    this->addWidget(container);
}
