/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainerSplitter.h>

#include <QDebug>
#include <QUuid>

#include <medAbstractData.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medViewContainer.h>

medViewContainerSplitter::medViewContainerSplitter(QWidget *parent)
{
    this->setOrientation(Qt::Horizontal);
    this->setHandleWidth(2);
    this->setContentsMargins(0,0,0,0);
    this->setOpaqueResize(false);
}

medViewContainerSplitter::~medViewContainerSplitter()
{

    emit aboutTobedestroyed();

    // Trick to 'inform' a parented splitter
    // "you're not my dad anymore!"
    // There is no  'takeItem()' or 'removeWidget()' or wathever methode to remove a widget from a QSplitter.
    // this is used to remove the ownership of the container, If the parent splitter end up with no child it will be deleted.
    // see medViewContainerSplitter::~medViewContainerSplitter() and medViewContainerSplitter::checkIfStillDeserveToLive()
    this->setParent(NULL);
}

medViewContainer * medViewContainerSplitter::splitHorizontally(medViewContainer *sender)
{
    return this->split(sender, Qt::AlignBottom);
}

medViewContainer * medViewContainerSplitter::splitVertically(medViewContainer *sender)
{
    return this->split(sender, Qt::AlignRight);
}

medViewContainer *medViewContainerSplitter::split(medViewContainer *sender, Qt::AlignmentFlag alignement)
{
    if(!sender)
        return NULL;

    int index = this->indexOf(sender);
    int newSize;
    if(this->count() == 1)
    {
        if(this->orientation() == Qt::Vertical)
            newSize = this->height() / 2;
        else
            newSize = this->width() / 2;
    }
    else
        newSize = this->sizes()[index] / 2;

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
            this->insertNestedSplitter(index, sender, newContainer, true);
        break;
    case Qt::AlignBottom:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index + 1, newContainer);
            this->recomputeSizes(index, index + 1, newSize);
        }
        else
            this->insertNestedSplitter(index, sender, newContainer);
        break;
    case Qt::AlignRight:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index + 1, newContainer);
            this->recomputeSizes(index, index + 1, newSize);
        }
        else
            this->insertNestedSplitter(index, sender, newContainer);
        break;
    case Qt::AlignTop:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index, newContainer);
            this->recomputeSizes(index + 1, index, newSize);
        }
        else
            this->insertNestedSplitter(index, sender, newContainer, true);
        break;
    default:
        break;
    }

    return newContainer;
}


medViewContainer * medViewContainerSplitter::splitHorizontally()
{
    return this->split(Qt::AlignBottom);
}

medViewContainer * medViewContainerSplitter::splitVertically()
{
    return this->split(Qt::AlignRight);
}

medViewContainer *medViewContainerSplitter::split(Qt::AlignmentFlag alignement)
{
    medViewContainer* sender = dynamic_cast<medViewContainer*>(this->sender());
    return this->split(sender, alignement);
}

void medViewContainerSplitter::split(medDataIndex index, Qt::AlignmentFlag alignement)
{
    medViewContainer *newContainer = this->split(alignement);
    newContainer->addData(medDataManager::instance()->retrieveData(index));
}

/**
 * @brief checkIfStillDeserveToLive
 * Check the number of child widget and call the destuctor if there is no one.
 */
void medViewContainerSplitter::checkIfStillDeserveToLive()
{
    if(this->count() == 0)
        delete this;
}

void medViewContainerSplitter::insertViewContainer(int index, medViewContainer *container)
{
    connect(container, SIGNAL(hSplitRequest()), this, SLOT(splitHorizontally()));
    connect(container, SIGNAL(vSplitRequest()), this, SLOT(splitVertically()));
    connect(container, SIGNAL(splitRequest(medDataIndex, Qt::AlignmentFlag)),
            this, SLOT(split(medDataIndex, Qt::AlignmentFlag)));
    connect(container, SIGNAL(destroyed()), this, SLOT(checkIfStillDeserveToLive()));
    connect(container, SIGNAL(destroyed()), this, SIGNAL(containerRemoved()));
    container->setContainerParent(this);

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

void medViewContainerSplitter::insertNestedSplitter(int index,
                                                 medViewContainer *oldContainer,
                                                 medViewContainer *newContainer,
                                                 bool inverseOrderInSplitter)
{
    Qt::Orientation ori = Qt::Vertical;
    if(this->orientation() == Qt::Vertical)
            ori = Qt::Horizontal;

    QList<int> savedSizes = this->sizes();
    oldContainer->disconnect(this);
    medViewContainerSplitter *splitter = new medViewContainerSplitter;
    splitter->setOrientation(ori);

    connect(splitter, SIGNAL(newContainer(QUuid)), this, SIGNAL(newContainer(QUuid)));
    connect(splitter, SIGNAL(containerRemoved()), this, SIGNAL(containerRemoved()));
    connect(splitter, SIGNAL(destroyed()), this, SLOT(checkIfStillDeserveToLive()));

    if(inverseOrderInSplitter)
    {
       splitter->addViewContainer(newContainer);
       splitter->blockSignals(true);
       splitter->addViewContainer(oldContainer);
       splitter->blockSignals(false);
    }
    else
    {
       splitter->blockSignals(true);
       splitter->addViewContainer(oldContainer);
       splitter->blockSignals(false);
       splitter->addViewContainer(newContainer);
    }

    this->insertWidget(index, splitter);
    this->setCollapsible(index, false);
    this->setSizes(savedSizes);

    // resize nested container because QVtkWidget2 is automaticlly resize to fit the its view
    // (given the fixed width/height of the splitter) when it is added to the splitter.
    int newSize = 0;
    if(splitter->orientation() == Qt::Vertical)
        newSize = splitter->width() / 2;
    else
        newSize = splitter->height() / 2;
    splitter->recomputeSizes(0, 1, newSize);

}

QList<medViewContainer*> medViewContainerSplitter::containers()
{
    QList<medViewContainer*> containers;
    for(int i=0; i<this->count(); i++)
    {
        QWidget *widget = this->widget(i);

        if(medViewContainer *container = dynamic_cast<medViewContainer *>(widget))
        {
            containers.append( container );
        }
        else if(medViewContainerSplitter *nestedSplitter = dynamic_cast<medViewContainerSplitter *>(widget))
        {
            containers.append( nestedSplitter->containers() );
        }
    }
    return containers;
}

void medViewContainerSplitter::adjustContainersSize()
{
    QList<int> currentSizes = this->sizes();
    int splitterSize = 0;

    //compute splitter size
    foreach(int s, currentSizes)
        splitterSize+=s;

    int newSize = splitterSize / this->count();
    QList<int> newSizes;

    for(int i=0; i<currentSizes.size(); i++)
        newSizes.append(newSize);

    this->setSizes(newSizes);

    for(int i=0; i<this->count(); i++)
    {
        if(medViewContainerSplitter *nestedSplitter = dynamic_cast<medViewContainerSplitter *>(this->widget(i)))
        {
            nestedSplitter->adjustContainersSize();
        }
    }
}

medViewContainer *medViewContainerSplitter::split(medViewContainer *sender, Qt::AlignmentFlag alignement,medViewContainer *newContainer)
{
    if(!sender)
        return NULL;

    int index = this->indexOf(sender);
    int newSize;
    if(this->count() == 1)
    {
        if(this->orientation() == Qt::Vertical)
            newSize = this->height() / 2;
        else
            newSize = this->width() / 2;
    }
    else
        newSize = this->sizes()[index] / 2;

    switch(alignement)
    {
    case Qt::AlignLeft:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index, newContainer);
            this->recomputeSizes(index + 1, index, newSize);
        }
        else
            this->insertNestedSplitter(index, sender, newContainer, true);
        break;
    case Qt::AlignBottom:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index + 1, newContainer);
            this->recomputeSizes(index, index + 1, newSize);
        }
        else
            this->insertNestedSplitter(index, sender, newContainer);
        break;
    case Qt::AlignRight:
        if(this->orientation() == Qt::Horizontal)
        {
            this->insertViewContainer(index + 1, newContainer);
            this->recomputeSizes(index, index + 1, newSize);
        }
        else
            this->insertNestedSplitter(index, sender, newContainer);
        break;
    case Qt::AlignTop:
        if(this->orientation() == Qt::Vertical)
        {
            this->insertViewContainer(index, newContainer);
            this->recomputeSizes(index + 1, index, newSize);
        }
        else
            this->insertNestedSplitter(index, sender, newContainer, true);
        break;
    default:
        break;
    }

    return newContainer;
}

