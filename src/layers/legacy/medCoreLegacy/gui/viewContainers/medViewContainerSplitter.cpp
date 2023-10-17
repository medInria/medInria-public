/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainerSplitter.h>

#include <dtkLog>
#include <QUuid>

#include <medAbstractData.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medViewContainer.h>

#include <medRootContainer.h>

medViewContainerSplitter::medViewContainerSplitter(QWidget *parent)
{
    Q_UNUSED(parent);
    this->setOrientation(Qt::Horizontal);
    this->setHandleWidth(2);
    this->setContentsMargins(0,0,0,0);
    this->setOpaqueResize(false);
}

medViewContainerSplitter::~medViewContainerSplitter()
{
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
    {
        return nullptr;
    }

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
    newContainer->addData(medDataManager::instance().retrieveData(index));
}

/**
 * @brief checkIfStillDeserveToLiveSpliter
 * Check the number of child widget and call the destructor if there is no one.
 */
void medViewContainerSplitter::checkIfStillDeserveToLiveSpliter()
{
    QObject *poTmp = nullptr;
    medViewContainerSplitter *poSplitterParent = nullptr;
    medRootContainer *poTabRootContainerParent = nullptr;
    switch (this->count())
    {
        case 0:
        {
            this->setParent(nullptr);
            close();
            deleteLater();
            break;
        }
        case 1 :
        {
            poTmp = this->widget(0);
            medViewContainerSplitter* poSplitterSon = dynamic_cast<medViewContainerSplitter*>(poTmp);
            if (poSplitterSon)
            {
                poTmp = parent();
                poSplitterSon->hide();  //Line very important to avoid a QWidget transparent glitch. Hide the son before changing his parent.
                poSplitterSon->setParent((QWidget*)poTmp); //Line very important to avoid a QWidget transparent glitch. Change parent of the son.
                poSplitterSon->adjustContainersSize();
                poSplitterParent = dynamic_cast<medViewContainerSplitter*>(poTmp);
                if (poSplitterParent) //The parent is another splitter
                {
                    int iOldIndex = poSplitterParent->indexOf(this);  //Compute good position by keeping the father's position.
                    this->setParent(nullptr); //This cuts the link with its father.
                    poSplitterParent->insertWidget(iOldIndex, poSplitterSon); //Insert at the good position.
                    poSplitterParent->adjustContainersSize();
                    close();
                    deleteLater();
                }
                else
                {
                    this->setParent(nullptr); //This cuts the link with its father.
                    poTabRootContainerParent = dynamic_cast<medRootContainer*>(poTmp);
                    if (poTabRootContainerParent) //The parent is the main medTabbedViewContainers.
                    {
                        poTabRootContainerParent->replaceSplitter(poSplitterSon);
                        close();
                        deleteLater();
                    }
                }
                poSplitterSon->show(); //Line very important to avoid a QWidget transparent glitch. Son is now displayed again.
            }
            break;
        }
    default:
        break;
    }
}

void medViewContainerSplitter::insertViewContainer(int index, medViewContainer *container)
{
    connect(container, SIGNAL(hSplitRequest()), this, SLOT(splitHorizontally()));
    connect(container, SIGNAL(vSplitRequest()), this, SLOT(splitVertically()));
    connect(container, SIGNAL(splitRequest(medDataIndex, Qt::AlignmentFlag)), this, SLOT(split(medDataIndex, Qt::AlignmentFlag)));
    connect(container, SIGNAL(destroyed()), this, SLOT(checkIfStillDeserveToLiveSpliter()));
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

void medViewContainerSplitter::insertNestedSplitter(int index, medViewContainer *oldContainer, medViewContainer *newContainer, bool inverseOrderInSplitter)
{
    Qt::Orientation ori = Qt::Vertical;
    if(this->orientation() == Qt::Vertical)
            ori = Qt::Horizontal;

    QList<int> savedSizes = this->sizes();
    oldContainer->disconnect(this);
    medViewContainerSplitter *splitter = new medViewContainerSplitter;
    splitter->setOrientation(ori);

    connect(splitter, SIGNAL(newContainer(QUuid)), this, SIGNAL(newContainer(QUuid)));
    connect(splitter, SIGNAL(destroyed()), this, SLOT(checkIfStillDeserveToLiveSpliter()));

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

    // resize nested container because QVtkWidget2 is automatically resize to fit the its view
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
    for(int s : currentSizes)
    {
        splitterSize+=s;
    }

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
