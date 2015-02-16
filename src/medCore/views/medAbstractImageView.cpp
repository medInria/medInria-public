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
#include <medDataIndexListParameter.h>
#include <medStringListParameter.h>

#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>
#include <medParameterGroupManager.h>



/**
 * @fn QPointF medAbstractImageView::mapWorldToDisplayCoordinates( const QVector3D & worldVec )
 * @brief Convert from world coordinates to scene coordinates.
 */

/**
 * @fn QVector3D medAbstractImageView::mapDisplayToWorldCoordinates( const QPointF & scenePoint )
 * @brief Convert from scene coordinates to world coordinates.
 */

/**
 * @fn QVector3D medAbstractImageView::viewCenter()
 * @brief Get the view center vector in world space, the center of the slice for 2d views.
 */

/**
 * @fn QVector3D medAbstractImageView::viewPlaneNormal()
 * @brief Get the view plane normal vector in world space.
 */

/**
 * @fn QVector3D medAbstractImageView::viewUp()
 * @brief Get the view plane up vector in world space.
 */

/**
 * @fn bool medAbstractImageView::is2D()
 * @brief Is the scene 2D (true) or 3D (false)
 */

/**
 * @fn qreal medAbstractImageView::sliceThickness()
 * @brief What is the thickness of the current slice (2D)
 */

/**
 * @fn qreal medAbstractImageView::scale()
 * @brief The scale (number of pixels on screen per mm)
 */


class medAbstractImageViewPrivate
{
public:
    QHash<medAbstractData*,  medAbstractImageViewInteractor*> primaryInteractorsHash;
    QHash<medAbstractData*,  QList<medAbstractInteractor*> > extraInteractorsHash;

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
    disconnect(this,SIGNAL(layerRemoved(unsigned int)),this,SLOT(updateDataListParameter(unsigned int)));

    int c = layersCount()-1;
    for(int i=c; i>=0; i--)
        removeLayer(i);
    delete d;
}

void medAbstractImageView::removeData(medAbstractData *data)
{
    medAbstractLayeredView::removeData(data);

    if( this->layersCount() == 0 && d->fourViewsParameter)
    {
        delete d->fourViewsParameter;
        d->fourViewsParameter = 0;
    }
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

QWidget* medAbstractImageView::toolBarWidget()
{
    QWidget* toolbar = medAbstractView::toolBarWidget();

    if(toolbar->layout())
        toolbar->layout()->addWidget(this->fourViewsParameter()->getPushButton());

    return toolbar;
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
        medAbstractData *data = medDataManager::instance()->retrieveData(index);
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

    QString linkGroupBaseName = "MPR ";
    unsigned int linkGroupNumber = 1;

    QString linkGroupName = linkGroupBaseName + QString::number(linkGroupNumber);
    while (medParameterGroupManager::instance()->viewGroup(linkGroupName))
    {
        linkGroupNumber++;
        linkGroupName = linkGroupBaseName + QString::number(linkGroupNumber);
    }

    medViewParameterGroup* viewGroup = new medViewParameterGroup(linkGroupName, this);
    viewGroup->addImpactedView(this);
    viewGroup->addImpactedView(topRightContainerView);
    viewGroup->addImpactedView(bottomLeftContainerView);
    viewGroup->addImpactedView(bottomRightContainerView);
    viewGroup->setLinkAllParameters(true);
    viewGroup->removeParameter("Orientation");

    for (unsigned int i = 0;i < this->layersCount();++i)
    {
        QString linkLayerName = linkGroupBaseName + QString::number(linkGroupNumber) + " Layer " + QString::number(i+1);
        medLayerParameterGroup* layerGroup = new medLayerParameterGroup(linkLayerName, this);
        layerGroup->setLinkAllParameters(true);
        layerGroup->addImpactedlayer(this, this->layerData(i));
        layerGroup->addImpactedlayer(topRightContainerView, topRightContainerView->layerData(i));
        layerGroup->addImpactedlayer(bottomLeftContainerView, bottomLeftContainerView->layerData(i));
        layerGroup->addImpactedlayer(bottomRightContainerView, bottomRightContainerView->layerData(i));
    }

    foreach(medAbstractParameter* param, this->linkableParameters())
        param->trigger();

    for (unsigned int i = 0;i < this->layersCount();++i)
        foreach(medAbstractParameter* param, this->linkableParameters(i))
            param->trigger();

    topLeftContainer->setSelected(true);
}

void medAbstractImageView::setOrientation(medImageView::Orientation orientation)
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return;

    nav->setOrientation(orientation);
    emit orientationChanged();
}

/**
 * Return a composite parameter made of:
 * QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale
 */
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

/**
 * Return a composite parameter made of:
 * double &window, double level
 */
medCompositeParameter *medAbstractImageView::windowLevelParameter(unsigned int layer)
{
    medAbstractImageViewInteractor* pInteractor = this->primaryInteractor(layer);
    if(!pInteractor)
    {
        return NULL;
    }

    return pInteractor->windowLevelParameter();
}

medTimeLineParameter *medAbstractImageView::timeLineParameter()
{
    medAbstractImageViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        return NULL;
    }

    return pNavigator->timeLineParameter();
}

medImageView::Orientation medAbstractImageView::orientation()
{
    medAbstractImageViewNavigator * nav = this->primaryNavigator();
    if(!nav)
        return medImageView::VIEW_ORIENTATION_AXIAL;

    return nav->orientation();
}
