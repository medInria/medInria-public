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
#include <medViewContainer.h>
#include <medAbstractData.h>
#include <QUuid>

medViewContainerSplitter::medViewContainerSplitter(QWidget *parent)
{
    this->setOrientation(Qt::Horizontal);
    this->setHandleWidth(1);
    this->setContentsMargins(0,0,0,0);
    this->setOpaqueResize(false);
}

medViewContainerSplitter::~medViewContainerSplitter()
{

}

void medViewContainerSplitter::hSplit()
{
    this->split(Qt::AlignBottom);
}

void medViewContainerSplitter::vSplit()
{
    this->split(Qt::AlignRight);
}

medViewContainer *medViewContainerSplitter::split(Qt::AlignmentFlag alignement)
{
    medViewContainer* container = dynamic_cast<medViewContainer*>(this->sender());
    if(!container)
        return NULL;
    int index = this->indexOf(container);
    int newSize = this->sizes()[index] / 2;

    medViewContainer *newContainer = new medViewContainer;

    switch(alignement)
    {
    case Qt::AlignLeft:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index, newContainer);
            this->recomputeSizes(index + 1, index, newSize);
        }
        else
            this->addNestedSplitter(index, newContainer, container);
        break;
    case Qt::AlignBottom:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index + 1, newContainer);
            this->recomputeSizes(index, index + 1, newSize);
        }
        else
            this->addNestedSplitter(index, container, newContainer);
        break;
    case Qt::AlignRight:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index + 1, newContainer);
            this->recomputeSizes(index, index + 1, newSize);
        }
        else
            this->addNestedSplitter(index, container, newContainer);
        break;
    case Qt::AlignTop:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index, newContainer);
            this->recomputeSizes(index + 1, index, newSize);
        }
        else
            this->addNestedSplitter(index, newContainer, container);
        break;
    }

    return newContainer;
}

void medViewContainerSplitter::split(medDataIndex index, Qt::AlignmentFlag alignement)
{
    medViewContainer *newContainer = this->split(alignement);
    newContainer->addData(medDataManager::instance()->data(index));
}

void medViewContainerSplitter::checkIfStillDeserveToLive()
{
    if(this->count() < 1)
        this->~medViewContainerSplitter();
}

void medViewContainerSplitter::insertViewContainer(int index, medViewContainer *container)
{
    connect(container, SIGNAL(hSplitRequest()), this, SLOT(hSplit()));
    connect(container, SIGNAL(vSplitRequest()), this, SLOT(vSplit()));
    connect(container, SIGNAL(splitRequest(medDataIndex, Qt::AlignmentFlag)),
            this, SLOT(split(medDataIndex, Qt::AlignmentFlag)));
    connect(container, SIGNAL(destroyed()), this, SLOT(checkIfStillDeserveToLive()));

    emit newContainer(container->uuid());

    this->insertWidget(index, container);
    this->setCollapsible(index, false);
}

void medViewContainerSplitter::addViewContainer(medViewContainer *container)
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

void medViewContainerSplitter::addNestedSplitter(int index,
                                                 medViewContainer *oldContainer,
                                                 medViewContainer *newContainer)
{
    Qt::Orientation ori = Qt::Vertical;
    if(this->orientation() == Qt::Vertical)
            ori = Qt::Horizontal;

    QList<int> savedSizes = this->sizes();
    oldContainer->disconnect(this);
    medViewContainerSplitter *splitter = new medViewContainerSplitter;
    splitter->setOrientation(ori);
    connect(splitter, SIGNAL(newContainer(QUuid&)), this, SIGNAL(newContainer(QUuid&)));
    splitter->addViewContainer(oldContainer);
    splitter->addViewContainer(newContainer);
    this->insertWidget(index, splitter);
    this->setSizes(savedSizes);
}
