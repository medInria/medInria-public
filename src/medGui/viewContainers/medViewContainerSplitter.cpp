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

#include <medDataManager.h>
#include <medViewSplitableContainer.h>
#include <medAbstractData.h>

class medViewContainerSplitterPrivate
{
    QList <medViewSplitableContainer*> containers;
};


medViewContainerSplitter::medViewContainerSplitter(QWidget *parent):
    d(new medViewContainerSplitterPrivate)
{
    this->setOrientation(Qt::Vertical);
    this->setHandleWidth(1);
    this->setContentsMargins(0,0,0,0);
    this->setOpaqueResize(false);
}

medViewContainerSplitter::~medViewContainerSplitter()
{
    delete d;
}


void medViewContainerSplitter::hSplit()
{
    this->split(Qt::AlignBottom);
}

void medViewContainerSplitter::vSplit()
{
    this->split(Qt::AlignRight);
}

medViewSplitableContainer *medViewContainerSplitter::split(Qt::AlignmentFlag alignement)
{
    medViewSplitableContainer* container = dynamic_cast<medViewSplitableContainer*>(this->sender());
    if(!container)
        return NULL;
    int index = this->indexOf(container);

    medViewSplitableContainer *newContainer = new medViewSplitableContainer;

    switch(alignement)
    {
    case Qt::AlignLeft:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index, newContainer);
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
            spliter->addViewContainer(newContainer);
            this->insertWidget(index, spliter);
        }
        break;
    case Qt::AlignBottom:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index + 1, newContainer);
        }
        else
        {
            container->hide();
            container->disconnect(this);
            medViewContainerSplitter *spliter = new medViewContainerSplitter;
            spliter->setOrientation(Qt::Vertical);
            spliter->addViewContainer(container);
            container->show();
            spliter->addViewContainer(newContainer);
            this->insertWidget(index, spliter);
        }
        break;
    case Qt::AlignRight:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index + 1, newContainer);
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
            spliter->addViewContainer(newContainer);
            this->insertWidget(index, spliter);
        }
        break;
    case Qt::AlignTop:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index, newContainer);
        }
        else
        {
            container->hide();
            container->disconnect(this);
            medViewContainerSplitter *spliter = new medViewContainerSplitter;
            spliter->setOrientation(Qt::Vertical);
            spliter->addViewContainer(container);
            container->show();
            spliter->addViewContainer(new medViewSplitableContainer);
            this->insertWidget(index, spliter);
        }
        break;
    }

    return newContainer;
}

void medViewContainerSplitter::split(medDataIndex index, Qt::AlignmentFlag alignement)
{
    medViewSplitableContainer *newContainer = this->split(alignement);
    newContainer->addData(medDataManager::instance()->data(index));
}

void medViewContainerSplitter::checkIfStillDeserveToLive()
{
    if(this->count() < 1)
        this->~medViewContainerSplitter();
}

void medViewContainerSplitter::insertViewContainer(int index, medViewSplitableContainer *container)
{
    connect(container, SIGNAL(hSplitRequest()), this, SLOT(hSplit()));
    connect(container, SIGNAL(vSplitRequest()), this, SLOT(vSplit()));
    connect(container, SIGNAL(splitRequest(medDataIndex, Qt::AlignmentFlag)),
            this, SLOT(split(medDataIndex, Qt::AlignmentFlag)));
    connect(container, SIGNAL(destroyed()), this, SLOT(checkIfStillDeserveToLive()));

    QList <int> newSizes = this->sizes();
    this->insertWidget(index, container);
    if(!newSizes.isEmpty())
    {
        qDebug() << "1index" << index;
        qDebug() << "2newSizes" << newSizes;
        int newSize = newSizes.at(index) / 2;
        qDebug() << "3newSize" << newSize;
        newSizes.insert(index, newSize);
        qDebug() << "4newSizes" << newSizes;
        newSizes.replace(index - 1, newSize);
    }
    this->setSizes(newSizes);
    this->setCollapsible(index, false);


}

void medViewContainerSplitter::addViewContainer(medViewSplitableContainer *container)
{
    this->insertViewContainer(this->count(), container);
}
