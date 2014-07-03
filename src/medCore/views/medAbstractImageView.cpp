/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageView.h>

#include <medAbstractData.h>
#include <medAbstractImageViewInteractor.h>
#include <medAbstractImageViewNavigator.h>
#include <medViewFactory.h>
#include <medViewContainer.h>
#include <medViewContainerSplitter.h>

#include <medTriggerParameter.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medParameterPool.h>
#include <medParameterPoolManager.h>
#include <medDataListParameter.h>
#include <medStringListParameter.h>

#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

class medAbstractImageViewPrivate
{
public:
    QHash<dtkSmartPointer<medAbstractData>,  medAbstractImageViewInteractor*> primaryInteractorsHash;
    QHash<dtkSmartPointer<medAbstractData>,  QList<medAbstractInteractor*> > extraInteractorsHash;

    medAbstractImageViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;

    medTriggerParameter *fourViewsParameter;
};

medAbstractImageView::medAbstractImageView(QObject *parent) : medAbstractLayeredView(parent),
    d(new medAbstractImageViewPrivate)
{
    d->primaryNavigator = NULL;
    d->fourViewsParameter = NULL;
}

medAbstractImageView::~medAbstractImageView()
{
    for(unsigned int i=0; i<layersCount(); i++)
        removeLayer(i);
    delete d;
}

medAbstractImageViewInteractor* medAbstractImageView::primaryInteractor(medAbstractData* data)
{
    if(d->primaryInteractorsHash.isEmpty())
        return NULL;

    return d->primaryInteractorsHash.value(data);
}

QList<medAbstractInteractor*> medAbstractImageView::extraInteractors(medAbstractData* data)
{
    return d->extraInteractorsHash.value(data);
}

medAbstractImageViewInteractor* medAbstractImageView::primaryInteractor(unsigned int layer)
{
    return d->primaryInteractorsHash.value(this->layerData(layer));
}

QList<medAbstractInteractor*> medAbstractImageView::extraInteractors(unsigned int layer)
{
    return d->extraInteractorsHash.value(this->layerData(layer));
}

medAbstractImageViewNavigator* medAbstractImageView::primaryNavigator()
{
    return d->primaryNavigator;
}

QList<medAbstractNavigator*> medAbstractImageView::extraNavigators()
{
    return d->extraNavigators;
}

void medAbstractImageView::removeInteractors(medAbstractData *data)
{
    medAbstractLayeredViewInteractor* pInteractor = d->primaryInteractorsHash.take(data);
    pInteractor->removeData();
    delete pInteractor;

    QList<medAbstractInteractor*> extraInt =  d->extraInteractorsHash.take(data);
    foreach(medAbstractInteractor* extra, extraInt)
    {
        delete extra;
    }
    extraInt.clear();
}

bool medAbstractImageView::initialiseInteractors(medAbstractData *data)
{
    // primary

    medViewFactory* factory = medViewFactory::instance();
    QStringList primaryInt = factory->interactorsAbleToHandle(this->identifier(), data->identifier());
    if(primaryInt.isEmpty())
    {
        qWarning() << "Unable to find any primary interactor for: " << this->identifier() << "and" << data->identifier();
        return false;
    }
    else
    {
        medAbstractImageViewInteractor* interactor = factory->createInteractor<medAbstractImageViewInteractor>(primaryInt.first(), this);
        connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
        connect(this, SIGNAL(currentLayerChanged()), interactor, SLOT(updateWidgets()));
        interactor->setInputData(data);
        d->primaryInteractorsHash.insert(data, interactor);
    }

    // extra
    QStringList extraInt = factory->additionalInteractorsAbleToHandle(this->identifier(), data->identifier());
    if(!extraInt.isEmpty())
    {
        QList<medAbstractInteractor*> extraIntList;
        foreach (QString i, extraInt)
        {
            medAbstractInteractor* interactor = factory->createAdditionalInteractor(i, this);
            connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
            connect(this, SIGNAL(currentLayerChanged()), interactor, SLOT(updateWidgets()));
            interactor->setInputData(data);
            extraIntList << interactor;
        }
        d->extraInteractorsHash.insert(data, extraIntList);
    }
    return true;
}

bool medAbstractImageView::initialiseNavigators()
{
    // primary
    medViewFactory* factory = medViewFactory::instance();
    QStringList primaryNav = factory->navigatorsAbleToHandle(this->identifier());
    if(primaryNav.isEmpty())
    {
        qWarning() << "Unable to find any primary navigator for: " << this->identifier();
        return false;
    }
    else
    {
        d->primaryNavigator = factory->createNavigator<medAbstractImageViewNavigator>(primaryNav.first(), this);
        connect(this, SIGNAL(orientationChanged()), d->primaryNavigator, SLOT(updateWidgets()));
        connect(this, SIGNAL(currentLayerChanged()), d->primaryNavigator, SLOT(updateWidgets()));
    }

    // extra
    QStringList extraNav = factory->additionalNavigatorsAbleToHandle(this->identifier());
    if(!extraNav.isEmpty())
    {
        foreach (QString n, extraNav)
        {
            medAbstractNavigator* nav = factory->createAdditionalNavigator(n, this);
            connect(this, SIGNAL(orientationChanged()), nav, SLOT(updateWidgets()));
            connect(this, SIGNAL(currentLayerChanged()), nav, SLOT(updateWidgets()));
            d->extraNavigators << nav;
        }
    }
    return true;
}

QWidget* medAbstractImageView::buildToolBarWidget()
{
    QWidget *toolBarWidget = this->fourViewsParameter()->getPushButton();

    return toolBarWidget;
}

void medAbstractImageView::switchToFourViews()
{
    medViewContainer *topLeftContainer = dynamic_cast <medViewContainer *> (this->parent());
    medViewContainerSplitter *topLeftContainerSplitter = dynamic_cast <medViewContainerSplitter *> (topLeftContainer->parent());

    medViewContainer *bottomLeftContainer = topLeftContainer->splitVertically();
    medViewContainer *topRightContainer = topLeftContainer->splitHorizontally();
    medViewContainer *bottomRightContainer = bottomLeftContainer->splitHorizontally();

    topLeftContainerSplitter->adjustContainersSize();

    foreach(medDataIndex index, this->dataList())
    {
        medAbstractData *data = medDataManager::instance()->data(index);
        if (!data)
            continue;

        topRightContainer->addData(data);
        bottomLeftContainer->addData(data);
        bottomRightContainer->addData(data);
    }

    this->setOrientation(medImageView::VIEW_ORIENTATION_3D);
    medAbstractImageView *bottomLeftContainerView = dynamic_cast <medAbstractImageView *> (bottomLeftContainer->view());
    medAbstractImageView *topRightContainerView = dynamic_cast <medAbstractImageView *> (topRightContainer->view());
    medAbstractImageView *bottomRightContainerView = dynamic_cast <medAbstractImageView *> (bottomRightContainer->view());

    bottomLeftContainerView->setOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
    topRightContainerView->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
    bottomRightContainerView->setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);

    medViewParameterGroup* viewGroup = new medViewParameterGroup("MPR", this);
    viewGroup->addImpactedView(this);
    viewGroup->addImpactedView(topRightContainerView);
    viewGroup->addImpactedView(bottomLeftContainerView);
    viewGroup->addImpactedView(bottomRightContainerView);
    viewGroup->setLinkAllParameters(true);
    viewGroup->removeParameter("Orientation");
    viewGroup->update();

    for (unsigned int i = 0;i < this->layersCount();++i)
    {
        medLayerParameterGroup* layerGroup = new medLayerParameterGroup("MPR " + QString::number(i+1), this);
        layerGroup->setLinkAllParameters(true);
        layerGroup->addImpactedlayer(this, i);
        layerGroup->addImpactedlayer(topRightContainerView, i);
        layerGroup->addImpactedlayer(bottomLeftContainerView, i);
        layerGroup->addImpactedlayer(bottomRightContainerView, i);
        layerGroup->update();
    }

//    QString linkGroupBaseName = "MPR ";
//    unsigned int linkGroupNumber = 1;

//    QString linkGroupName = linkGroupBaseName + QString::number(linkGroupNumber);
//    while (medParameterPoolManager::instance()->pool(linkGroupName))
//    {
//        linkGroupNumber++;
//        linkGroupName = linkGroupBaseName + QString::number(linkGroupNumber);
//    }

//    QColor linkGroupColor;
//    double hueValue = 2.0 * linkGroupNumber / (1.0 + sqrt(5.0));
//    hueValue -= floor(hueValue);
//    linkGroupColor.setHsvF(hueValue,1.0,1.0);

//    QPixmap linkGroupPixmap(32,32);
//    linkGroupPixmap.fill(linkGroupColor);
//    QIcon linkGroupIcon(linkGroupPixmap);

//    this->linkParameter()->addItem(linkGroupName, linkGroupIcon);
//    this->linkParameter()->setValue(linkGroupName);
//    topRightContainerView->linkParameter()->addItem(linkGroupName, linkGroupIcon);
//    topRightContainerView->linkParameter()->setValue(linkGroupName);
//    bottomLeftContainerView->linkParameter()->addItem(linkGroupName, linkGroupIcon);
//    bottomLeftContainerView->linkParameter()->setValue(linkGroupName);
//    bottomRightContainerView->linkParameter()->addItem(linkGroupName, linkGroupIcon);
//    bottomRightContainerView->linkParameter()->setValue(linkGroupName);

//    for (unsigned int i = 0;i < this->layersCount();++i)
//    {
//        QString linkLayerName = linkGroupBaseName + QString::number(linkGroupNumber) + " Layer " + QString::number(i+1);
//        QColor linkLayerColor;
//        hueValue = 2.0 * i / (1.0 + sqrt(5.0));
//        hueValue -= floor(hueValue);
//        linkLayerColor.setHsvF(hueValue,1.0,1.0);

//        QPixmap linkLayerPixmap(32,32);
//        linkLayerPixmap.fill(linkLayerColor);
//        QIcon linkLayerIcon(linkLayerPixmap);

//        this->layerLinkParameter(i)->addItem(linkLayerName, linkLayerIcon);
//        this->layerLinkParameter(i)->setValue(linkLayerName);
//        topRightContainerView->layerLinkParameter(i)->addItem(linkLayerName, linkLayerIcon);
//        topRightContainerView->layerLinkParameter(i)->setValue(linkLayerName);
//        bottomLeftContainerView->layerLinkParameter(i)->addItem(linkLayerName, linkLayerIcon);
//        bottomLeftContainerView->layerLinkParameter(i)->setValue(linkLayerName);
//        bottomRightContainerView->layerLinkParameter(i)->addItem(linkLayerName, linkLayerIcon);
//        bottomRightContainerView->layerLinkParameter(i)->setValue(linkLayerName);
//    }

//    medParameterPool *linkPool = medParameterPoolManager::instance()->pool(linkGroupName);
//    linkPool->removeAll("axial");
//    linkPool->removeAll("coronal");
//    linkPool->removeAll("sagittal");
//    linkPool->removeAll("3d");

//    linkPool->append(this->dataListParameter());
//    linkPool->append(topRightContainerView->dataListParameter());
//    linkPool->append(bottomLeftContainerView->dataListParameter());
//    linkPool->append(bottomRightContainerView->dataListParameter());
}

void medAbstractImageView::setOrientation(medImageView::Orientation orientation)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->setOrientation(orientation);
    emit orientationChanged();
}


medCompositeParameter *medAbstractImageView::cameraParameter()
{
    medAbstractImageViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        return NULL;
    }

    return pNavigator->cameraParameter();
}

medAbstractVector3DParameter *medAbstractImageView::positionBeingViewedParameter()
{
    medAbstractImageViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        return NULL;
    }

    return pNavigator->positionBeingViewedParameter();
}

medDoubleParameter *medAbstractImageView::opacityParameter(unsigned int layer)
{
    medAbstractImageViewInteractor* pInteractor = this->primaryInteractor(layer);
    if(!pInteractor)
    {
        return NULL;
    }

    return pInteractor->opacityParameter();
}

medTriggerParameter *medAbstractImageView::fourViewsParameter()
{
    if (!d->fourViewsParameter)
    {
        d->fourViewsParameter = new medTriggerParameter("Four views", this);
        QIcon fourViewsIcon (":/icons/fourViews.png");
        d->fourViewsParameter->setButtonIcon(fourViewsIcon);

        connect(d->fourViewsParameter,SIGNAL(triggered()),this,SLOT(switchToFourViews()));
    }

    return d->fourViewsParameter;
}

medCompositeParameter *medAbstractImageView::windowLevelParameter(unsigned int layer)
{
    medAbstractImageViewInteractor* pInteractor = this->primaryInteractor(layer);
    if(!pInteractor)
    {
        return NULL;
    }

    return pInteractor->windowLevelParameter();
}

medImageView::Orientation medAbstractImageView::orientation()
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return medImageView::VIEW_ORIENTATION_AXIAL;

    return nav->orientation();
}
