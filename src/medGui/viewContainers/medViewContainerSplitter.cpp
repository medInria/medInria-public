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
    this->setOrientation(Qt::Horizontal);
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
    int newSize = this->sizes()[index] / 2;

    medViewSplitableContainer *newContainer = new medViewSplitableContainer;

    switch(alignement)
    {
    case Qt::AlignLeft:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index, newContainer);
            this->recomputeSizes(index + 1, index, newSize);
        }
        else
        {
            container->disconnect(this);
            medViewContainerSplitter *spliter = new medViewContainerSplitter;
            spliter->setOrientation(Qt::Horizontal);
            spliter->addViewContainer(container);
            spliter->addViewContainer(newContainer);
            this->insertWidget(index, spliter);
            this->recomputeSizes(index, index + 1, newSize * 2);
        }
        break;
    case Qt::AlignBottom:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index + 1, newContainer);
            this->recomputeSizes(index, index + 1, newSize);
        }
        else
        {
            container->disconnect(this);
            medViewContainerSplitter *spliter = new medViewContainerSplitter;
            spliter->setOrientation(Qt::Vertical);
            spliter->addViewContainer(container);
            spliter->addViewContainer(newContainer);
            this->insertWidget(index, spliter);
            this->recomputeSizes(index - 1, index, newSize * 2);
        }
        break;
    case Qt::AlignRight:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index + 1, newContainer);
            this->recomputeSizes(index, index + 1, newSize);
        }
        else
        {
            container->disconnect(this);
            medViewContainerSplitter *spliter = new medViewContainerSplitter;
            spliter->setOrientation(Qt::Horizontal);
            spliter->addViewContainer(container);
            spliter->addViewContainer(newContainer);
            this->insertWidget(index, spliter);
            this->recomputeSizes(index, index + 1, newSize * 2);
        }
        break;
    case Qt::AlignTop:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index, newContainer);
            this->recomputeSizes(index + 1, index, newSize);
        }
        else
        {
            container->disconnect(this);
            medViewContainerSplitter *spliter = new medViewContainerSplitter;
            spliter->setOrientation(Qt::Vertical);
            spliter->addViewContainer(container);
            spliter->addViewContainer(new medViewSplitableContainer);
            this->insertWidget(index, spliter);
            this->recomputeSizes(index - 1, index, newSize * 2);
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

    this->insertWidget(index, container);
    this->setCollapsible(index, false);
}

void medViewContainerSplitter::addViewContainer(medViewSplitableContainer *container)
{   
    int newSize = 0;
    if(this->count() > 0)
        newSize = this->sizes()[this->count() - 1] / 2;

    this->insertViewContainer(this->count(), container);
    if(this->count() > 1)
        this->recomputeSizes(this->count() - 2, this->count() - 1, newSize);
}

void medViewContainerSplitter::recomputeSizes(int requestIndex, int newIndex, int newSize)
{
    if(requestIndex < 0 || newIndex < 0)
        return;

    QList <int> newSizes = this->sizes();
    if(requestIndex >= newSizes.size() || newIndex >= newSizes.size())
        return;

    newSizes.replace(requestIndex, newSize);
    newSizes.replace(newIndex, newSize);
    this->setSizes(newSizes);
}
