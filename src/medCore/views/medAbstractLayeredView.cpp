/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractLayeredView.h>

#include <dtkCore/dtkSmartPointer.h>
#include <medDataManager.h>

#include <medAbstractData.h>
#include <medAbstractLayeredViewInteractor.h>
#include <medViewFactory.h>
#include <medStringListParameter.h>
#include <medParameterPoolManager.h>
#include <medBoolGroupParameter.h>
#include <medDataListParameter.h>

class medAbstractLayeredViewPrivate
{
public:
    QList <dtkSmartPointer<medAbstractData> > layersDataList;
    QHash<dtkSmartPointer<medAbstractData>,  medAbstractLayeredViewInteractor*> primaryInteractorsHash;
    QHash<dtkSmartPointer<medAbstractData>,  QList<medAbstractInteractor*> > extraInteractorsHash;

    medAbstractLayeredViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;

    medDataListParameter *dataListParameter;
    
    QList<medStringListParameter*> linkparameters;
    unsigned int currentLayer;

    QWidget* toolBarWidget;

    // toolboxes
    QWidget* navigatorWidget;
    QWidget* mouseInteractionWidget;

};

medAbstractLayeredView::medAbstractLayeredView(QObject *parent) : medAbstractView(parent), d (new medAbstractLayeredViewPrivate)
{
    d->primaryNavigator = NULL;
    d->currentLayer = 0;
    d->toolBarWidget = 0;
    d->navigatorWidget = NULL;
    d->mouseInteractionWidget = NULL;
    connect(this, SIGNAL(aboutToBuildThumbnail()), this, SLOT(setUpViewForThumbnail()));
    
    d->dataListParameter = new medDataListParameter("DataList",this);
    connect(d->dataListParameter,SIGNAL(valuesChanged(QList<medDataIndex>)),this,SLOT(setDataList(QList<medDataIndex>)));
    connect(this,SIGNAL(layerAdded(unsigned int)),this,SLOT(updateDataListParameter(unsigned int)));
    connect(this,SIGNAL(layerRemoved(unsigned int)),this,SLOT(updateDataListParameter(unsigned int)));
}

medAbstractLayeredView::~medAbstractLayeredView()
{
    delete d;
}

void medAbstractLayeredView::removeInteractors(medAbstractData *data)
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

bool medAbstractLayeredView::initialiseInteractors(medAbstractData *data)
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
        medAbstractLayeredViewInteractor* interactor = factory->createInteractor<medAbstractLayeredViewInteractor>(primaryInt.first(), this);
        interactor->setInputData(data);
        d->primaryInteractorsHash.insert(data, interactor);
        connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
    }

    // extra
    QStringList extraInt = factory->additionalInteractorsAbleToHandle(this->identifier(), data->identifier());
    if(!extraInt.isEmpty())
    {
        QList<medAbstractInteractor*> extraIntList;
        foreach (QString i, extraInt)
        {
            medAbstractInteractor* interactor = factory->createAdditionalInteractor(i, this);
            interactor->setInputData(data);
            connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
            extraIntList << interactor;
        }
        d->extraInteractorsHash.insert(data, extraIntList);
    }

    return true;
}

bool medAbstractLayeredView::initialiseNavigators()
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
        d->primaryNavigator = factory->createNavigator<medAbstractLayeredViewNavigator>(primaryNav.first(), this);
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

medAbstractLayeredViewInteractor* medAbstractLayeredView::primaryInteractor(medAbstractData* data)
{
    if(d->primaryInteractorsHash.isEmpty())
        return NULL;

    return d->primaryInteractorsHash.value(data);
}

QList<medAbstractInteractor*> medAbstractLayeredView::extraInteractors(medAbstractData* data)
{
    return d->extraInteractorsHash.value(data);
}

medAbstractLayeredViewInteractor* medAbstractLayeredView::primaryInteractor(unsigned int layer)
{
    return d->primaryInteractorsHash.value(this->layerData(layer));
}

QList<medAbstractInteractor*> medAbstractLayeredView::extraInteractors(unsigned int layer)
{
    return d->extraInteractorsHash.value(this->layerData(layer));
}

medAbstractLayeredViewNavigator* medAbstractLayeredView::primaryNavigator()
{
    return d->primaryNavigator;
}

QList<medAbstractNavigator*> medAbstractLayeredView::extraNavigators()
{
    return d->extraNavigators;
}

void medAbstractLayeredView::addLayer(medAbstractData *data)
{
    this->insertLayer(d->layersDataList.count(), data);
}

QList<medDataIndex> medAbstractLayeredView::dataList() const
{
    QList <medDataIndex> outputList;
    
    foreach(medAbstractData *data, d->layersDataList)
        outputList << data->dataIndex();
    
    return outputList;
}

unsigned int medAbstractLayeredView::layer(medAbstractData * data)
{
    return d->layersDataList.indexOf(data);
}

void medAbstractLayeredView::removeData(medAbstractData *data)
{
    if(!d->layersDataList.contains(data))
        return;

    this->removeInteractors(data);
    int layer = this->layer(data);
    d->layersDataList.removeAll(data);

    // remove the link parameter of this layer
    d->linkparameters.removeAt(layer);

    if(d->layersDataList.count() != 0  && layer == d->layersDataList.count())
    {
        this->setCurrentLayer(layer -1);
        emit layerRemoved(layer);
    }
    else if(d->layersDataList.count() != 0)
    {
        this->setCurrentLayer(layer);
        emit layerRemoved(layer);
    }
    else
        this->~medAbstractLayeredView();
}

void medAbstractLayeredView::setDataList(QList<medDataIndex> dataList)
{
    d->dataListParameter->blockSignals(true);
    
    foreach(medDataIndex index, dataList)
    {
        medAbstractData *data = medDataManager::instance()->data(index);
        if (!data)
            continue;
        
        this->addLayer(data);
        
        if ((this->layerLinkParameter(0)->value() != "")&&(this->layerLinkParameter(0)->value() != "None"))
        {
            unsigned int layerNumber = this->layer(data);
            QString groupName = this->linkParameter()->value() + " Layer " + QString::number(layerNumber+1);
            QColor layerColor;
            double hueValue = 2.0 * layerNumber / (1.0 + sqrt(5.0));
            hueValue -= floor(hueValue);
            layerColor.setHsvF(hueValue,1.0,1.0);
            
            QPixmap layerPixmap(32,32);
            layerPixmap.fill(layerColor);
            QIcon layerIcon(layerPixmap);
            
            if (!this->layerLinkParameter(layerNumber)->items().contains(groupName))
                this->layerLinkParameter(layerNumber)->addItem(groupName,layerIcon);

            this->layerLinkParameter(layerNumber)->setValue(groupName);
        }
    }
    
    foreach(medDataIndex index, this->dataList())
    {
        medAbstractData *data = medDataManager::instance()->data(index);
        if (!data)
            continue;

        if (!dataList.contains(data->dataIndex()))
            this->removeLayer(this->layer(data));
    }
    
    d->dataListParameter->blockSignals(false);
}

void medAbstractLayeredView::updateDataListParameter(unsigned int layer)
{
    d->dataListParameter->setValues(this->dataList());
}

void medAbstractLayeredView::removeLayer(unsigned int layer)
{
    this->removeData(this->layerData(layer));
}

void medAbstractLayeredView::removeLayer()
{
    this->removeData(this->layerData(d->currentLayer));
}

void medAbstractLayeredView::insertLayer(unsigned int layer, medAbstractData *data)
{
    if(!data)
    {
        qWarning() << "Attempt to add a NULL data to the view: " << this;
        return;
    }

    if ( this->contains(data))
    {
        qDebug() << "Attempt to add twice the same data to the view: " << this;
        return;
    }

    d->layersDataList.insert(layer, data);

    bool initSuccess = this->initialiseInteractors(data);
    if(!initSuccess)
    {
        d->layersDataList.removeOne(data);
        return;
    }

    // create a link parameter for this layer and add it to the list
    medStringListParameter *linkParameter = this->createLinkParameter();
    QString tooltip = QString(tr("Link layer properties ("));
    foreach(medAbstractParameter *param, this->interactorsParameters(layer))
        tooltip += param->name() + ", ";
    tooltip += ")";
    linkParameter->setToolTip(tooltip);
    d->linkparameters.insert(layer, linkParameter);

    this->setCurrentLayer(layer);
    if(this->layersCount() == 1)
        this->reset();

    emit layerAdded(layer);
}

medAbstractData * medAbstractLayeredView::layerData(unsigned int layer) const
{
    if (layer >= (unsigned int)d->layersDataList.size())
    {
        qWarning() << "Unable to retreive data at layer:" <<layer << "from: "<< this->description();
        return NULL;
    }
    return d->layersDataList[layer];
}

bool medAbstractLayeredView::contains(medAbstractData * data) const
{
    return d->layersDataList.contains(data);
}

bool medAbstractLayeredView::contains(QString identifier) const
{
    foreach(medAbstractData *data, d->layersDataList)
    {
        if(data->identifier() == identifier)
            return true;
    }

    return false;
}

unsigned int medAbstractLayeredView::layersCount() const
{
    return d->layersDataList.count();
}

medDataListParameter *medAbstractLayeredView::dataListParameter() const
{
    return d->dataListParameter;
}

medAbstractBoolParameter* medAbstractLayeredView::visibilityParameter(unsigned int layer)
{
    medAbstractLayeredViewInteractor* pInteractor = this->primaryInteractor(layer);
    if(!pInteractor)
    {
        return NULL;
    }

    return pInteractor->visibilityParameter();
}

medStringListParameter* medAbstractLayeredView::layerLinkParameter(unsigned int layer)
{
    medStringListParameter *linkParameter = d->linkparameters.at(layer);

    //this should not happened
    if(!linkParameter)
    {
        // create a link parameter for this layer and add it to the list
        medStringListParameter *linkParameter = this->createLinkParameter();
        QString tooltip = QString(tr("Link layer properties ("));
        foreach(medAbstractParameter *param, this->interactorsParameters(layer))
            tooltip += param->name() + ", ";
        tooltip += ")";
        linkParameter->setToolTip(tooltip);
        d->linkparameters.insert(layer, linkParameter);
    }

    return linkParameter;
}

medStringListParameter* medAbstractLayeredView::createLinkParameter()
{
    medStringListParameter *linkParameter =  new medStringListParameter("Link layer", this);

    linkParameter->addItem("None", medStringListParameter::createIconFromColor("transparent"));
    linkParameter->addItem("Layer group 1", medStringListParameter::createIconFromColor("orange"));
    linkParameter->addItem("Layer group 2", medStringListParameter::createIconFromColor("lightGreen"));
    linkParameter->addItem("Layer group 3", medStringListParameter::createIconFromColor("#0080C0"));

    connect(linkParameter, SIGNAL(valueChanged(QString)), this, SLOT(linkLayer(QString)));
    linkParameter->setValue("None");

    return linkParameter;
}

unsigned int medAbstractLayeredView::currentLayer() const
{
    return d->currentLayer;
}

void medAbstractLayeredView::setCurrentLayer(unsigned int layer)
{
    d->currentLayer = layer;
    emit currentLayerChanged();
}

void medAbstractLayeredView::setUpViewForThumbnail()
{
    medAbstractLayeredViewInteractor *primaryInteractor = this->primaryInteractor(this->layerData(d->currentLayer));
    if(!primaryInteractor)
    {
        QString msg = "Unable to find any primary interactor for view "  + this->identifier();
        if(this->layerData(d->currentLayer))
            msg += "and data" + this->layerData(d->currentLayer)->identifier();
        qWarning() << msg;
    }

    else
        this->primaryInteractor(this->layerData(d->currentLayer))->setUpViewForThumbnail();
}

QList <medAbstractInteractor*>  medAbstractLayeredView::interactors(unsigned int layer)
{
    QList <medAbstractInteractor*> interactors;
    interactors << this->primaryInteractor(layer) << this->extraInteractors(layer);
    return interactors;
}

QList<medAbstractNavigator*> medAbstractLayeredView::navigators()
{
    QList<medAbstractNavigator*> navigatorsList;
    navigatorsList << this->primaryNavigator() << this->extraNavigators();
    
    return navigatorsList;
}

QWidget* medAbstractLayeredView::navigatorWidget()
{
    if(!d->navigatorWidget)
    {
        d->navigatorWidget = new QWidget;
        connect(d->navigatorWidget, SIGNAL(destroyed()), this, SLOT(removeInternNavigatorWidget()));
        QVBoxLayout* navigatorLayout = new QVBoxLayout(d->navigatorWidget);

        navigatorLayout->addWidget(primaryNavigator()->toolBoxWidget());
        foreach (medAbstractNavigator* navigator, this->extraNavigators())
            navigatorLayout->addWidget(navigator->toolBoxWidget());
    }

    return d->navigatorWidget;
}

QWidget* medAbstractLayeredView::toolBarWidget()
{
    if(!d->toolBarWidget)
    {
        d->toolBarWidget = this->buildToolBarWidget();
        connect(d->toolBarWidget, SIGNAL(destroyed()), this, SLOT(removeInternToolBarWidget()));
    }

    return d->toolBarWidget;
}

QWidget* medAbstractLayeredView::mouseInteractionWidget()
{
    if(!d->mouseInteractionWidget)
    {
        d->mouseInteractionWidget = new QWidget;
        connect(d->mouseInteractionWidget, SIGNAL(destroyed()), this, SLOT(removeInternMouseInteractionWidget()));

        QList<medBoolParameter*> params;

        params.append(primaryInteractor(this->currentLayer())->mouseInteractionParameters());
        foreach (medAbstractInteractor* interactor, this->extraInteractors(this->currentLayer()))
            params.append(interactor->mouseInteractionParameters());

        params.append(primaryNavigator()->mouseInteractionParameters());
        foreach (medAbstractNavigator* navigator, this->extraNavigators())
            params.append(navigator->mouseInteractionParameters());

        medBoolGroupParameter *groupParam = new medBoolGroupParameter("Mouse Interaction", this);
        groupParam->setPushButtonDirection(QBoxLayout::LeftToRight);
        foreach (medBoolParameter* param, params)
            groupParam->addParameter(param);

        d->mouseInteractionWidget = groupParam->getPushButtonGroup();
    }

    return d->mouseInteractionWidget;
}

QList<medAbstractParameter*> medAbstractLayeredView::interactorsParameters(unsigned int layer)
{
    QList<medAbstractParameter*>  params;
    foreach (medAbstractInteractor* interactor, this->interactors(layer))
    {
        foreach(medAbstractParameter *param, interactor->linkableParameters())
        {
            params.append(param);
        }
    }

    return params;
}

void medAbstractLayeredView::linkLayer(QString pool)
{
    medStringListParameter *linkParam = dynamic_cast<medStringListParameter *>(this->sender());
    if(!linkParam)
        return;

    int layer = d->linkparameters.indexOf(linkParam);
    if(layer == -1)
        return;

    unlinkLayer(layer);

    if(pool!="None")
    {
        foreach(medAbstractParameter *param, this->interactorsParameters(layer))
            medParameterPoolManager::instance()->linkParameter(param, pool);
    }
}

void medAbstractLayeredView::unlinkLayer(unsigned int layer)
{
    foreach(medAbstractParameter *param, this->interactorsParameters(layer))
        medParameterPoolManager::instance()->unlinkParameter(param);
}

void medAbstractLayeredView::removeInternToolBarWidget()
{
    d->toolBarWidget = NULL;
}

void medAbstractLayeredView::removeInternNavigatorWidget()
{
    d->navigatorWidget = NULL;
}

void medAbstractLayeredView::removeInternMouseInteractionWidget()
{
    d->mouseInteractionWidget = NULL;
}
