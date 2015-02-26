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
#include <medBoolGroupParameter.h>
#include <medDataListParameter.h>
#include <medLayerParameterGroup.h>
#include <medParameterGroupManager.h>
#include <medDataIndex.h>

#include <QDomDocument>
#include <QDomElement>

class medAbstractLayeredViewPrivate
{
public:

    // Smart pointed.
    // dtkSmartPointer should only be used in views, process and dataManager.
    QList <dtkSmartPointer<medAbstractData> > layersDataList;
    QHash<medAbstractData *,  medAbstractLayeredViewInteractor*> primaryInteractorsHash;
    QHash<medAbstractData *,  QList<medAbstractInteractor*> > extraInteractorsHash;

    medAbstractLayeredViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;

    medDataListParameter *dataListParameter;

    unsigned int currentLayer;
};

medAbstractLayeredView::medAbstractLayeredView(QObject *parent) : medAbstractView(parent), d (new medAbstractLayeredViewPrivate)
{
    d->primaryNavigator = NULL;
    d->currentLayer = 0;

    d->dataListParameter = new medDataListParameter("DataList",this);
    connect(d->dataListParameter,SIGNAL(valuesChanged(QList<medDataIndex>)),this,SLOT(setDataList(QList<medDataIndex>)));
    connect(this,SIGNAL(layerAdded(unsigned int)),this,SLOT(updateDataListParameter(unsigned int)));
    connect(this,SIGNAL(layerRemoved(unsigned int)),this,SLOT(updateDataListParameter(unsigned int)));
    connect(this,SIGNAL(layerRemoved(unsigned int)),this,SLOT(render()));
}

medAbstractLayeredView::~medAbstractLayeredView()
{
    disconnect(this,SIGNAL(layerRemoved(unsigned int)),this,SLOT(updateDataListParameter(unsigned int)));

    int c = layersCount()-1;
    for(int i=c; i>=0; i--)
        removeLayer(i);
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

medAbstractLayeredViewInteractor * medAbstractLayeredView::primaryInteractor()
{
    return this->primaryInteractor(d->currentLayer);
}

QList<medAbstractInteractor *> medAbstractLayeredView::extraInteractors()
{
    return this->extraInteractors(d->currentLayer);
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
    int res = d->layersDataList.removeAll(data);

    if(d->layersDataList.count() != 0  && layer == d->layersDataList.count())
    {
        this->setCurrentLayer(layer -1);
    }
    else if(d->layersDataList.count() != 0)
    {
        this->setCurrentLayer(layer);
    }

    if( res > 0 )
    {
        emit layerRemoved(layer);
        emit layerRemoved(data);
    }
}

void medAbstractLayeredView::setDataList(QList<medDataIndex> dataList)
{
    d->dataListParameter->blockSignals(true);

    foreach(medDataIndex index, this->dataList())
    {
        if (!dataList.contains(index)) {
            medAbstractData *data = medDataManager::instance()->retrieveData(index);
            if (data)
                this->removeLayer(this->layer(data));
        }
    }

    foreach(medDataIndex index, dataList)
    {
        medAbstractData *data = medDataManager::instance()->retrieveData(index);
        if (!data)
            continue;

        this->addLayer(data);

        unsigned int layerNumber = this->layer(data);

        QList<medLayerParameterGroup*> groupsLayer0 = medParameterGroupManager::instance()->layerGroups(this, this->layerData(0));
        QList<medLayerParameterGroup*> groupsLayeri = medParameterGroupManager::instance()->layerGroups(this, data);
        if(!groupsLayer0.isEmpty() && layerNumber > 0 && groupsLayeri.isEmpty() )
        {
            QString newGroup = groupsLayer0[0]->name() + " Layer " + QString::number(layerNumber+1);
            medLayerParameterGroup* layerGroup = medParameterGroupManager::instance()->layerGroup(newGroup);
            if(!layerGroup)
              layerGroup = new medLayerParameterGroup(newGroup, this);
            layerGroup->setLinkAllParameters(true);
            layerGroup->addImpactedlayer(this, data);
        }
        else if(!groupsLayer0.isEmpty() && layerNumber == 0)
        {
            groupsLayer0[0]->addImpactedlayer(this, data);
        }
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

    this->setCurrentLayer(layer);
    if(this->layersCount() == 1)
        this->reset();

    emit layerAdded(layer);
}

medAbstractData * medAbstractLayeredView::layerData(unsigned int layer) const
{
    if (layer >= (unsigned int)d->layersDataList.size())
    {
        qWarning() << "Unable to retrieve data at layer:" <<layer << "from: "<< this->description();
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

unsigned int medAbstractLayeredView::currentLayer() const
{
    return d->currentLayer;
}

void medAbstractLayeredView::setCurrentLayer(unsigned int layer)
{
    d->currentLayer = layer;
    emit currentLayerChanged();
}

QList <medAbstractInteractor*>  medAbstractLayeredView::layerInteractors(unsigned int layer)
{
    QList <medAbstractInteractor*> interactors;

    if(this->primaryInteractor(layer))
        interactors << this->primaryInteractor(layer);

    if(!this->extraInteractors(layer).isEmpty())
        interactors << this->extraInteractors(layer);

    return interactors;
}

QList <medAbstractInteractor*> medAbstractLayeredView::interactors()
{
    return layerInteractors(this->currentLayer());
}

QList<medAbstractNavigator*> medAbstractLayeredView::navigators()
{
    QList<medAbstractNavigator*> navigatorsList;

    if(this->primaryNavigator())
        navigatorsList << this->primaryNavigator();

    if(!this->extraNavigators().isEmpty())
        navigatorsList << this->extraNavigators();

    return navigatorsList;
}

QList<medAbstractParameter*> medAbstractLayeredView::interactorsParameters(unsigned int layer)
{
    QList<medAbstractParameter*>  params;
    foreach (medAbstractInteractor* interactor, this->layerInteractors(layer))
    {
        foreach(medAbstractParameter *param, interactor->linkableParameters())
        {
            params.append(param);
        }
    }

    return params;
}

QList<medAbstractParameter*> medAbstractLayeredView::linkableParameters()
{
    QList<medAbstractParameter*> params = medAbstractView::linkableParameters();
    params << dataListParameter();

    return params;
}

QList<medAbstractParameter*> medAbstractLayeredView::linkableParameters(unsigned int layer)
{
    return interactorsParameters(layer);
}


void medAbstractLayeredView::write(QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
		return;
	QFileInfo fileInfo(file);

    QTextStream out(&file);
    QDomDocument doc(fileInfo.fileName());
	QDomElement root = doc.createElement("xml");
	doc.appendChild(root);
	QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
	for(unsigned int i=0;i<this->layersCount();i++)
	{
    
    QList<dtkAbstractDataWriter*> possibleWriters;

    foreach(QString writerType, allWriters) 
    {
        dtkAbstractDataWriter * writer = medAbstractDataFactory::instance()->writer(writerType);
        if (writer->handled().contains(layerData(i)->identifier()))
            possibleWriters.append(writer);
        else
            delete writer;
    }

    if (possibleWriters.isEmpty()) {
        medMessageController::instance()->showError("Sorry, we have no exporter for this format.");
        return;
    }
		
		
		QDomElement layerDescription = doc.createElement("layer");
		layerDescription.setAttribute("id",i);
		//generating filename 
        QString currentFile=layerData(i)->metadata("SeriesDescription")+possibleWriters[0]->supportedFileExtensions()[0];
		if(medDataReaderWriter::write(currentFile,layerData(i)))
			layerDescription.setAttribute("filename",currentFile);
		else
			layerDescription.setAttribute("filename","failed to save data");
		
		//saving navigators
		QDomElement navigatorsNode=doc.createElement("navigators");
		for(int j=0;j<navigators().size();j++)
		{
			QDomElement currentNavigatorNode = doc.createElement("navigator");
			navigators()[j]->toXMLNode(&doc,&currentNavigatorNode);
			navigatorsNode.appendChild(currentNavigatorNode);

		}
		layerDescription.appendChild(navigatorsNode);
		
		//saving interactors
		QDomElement interactorsNode=doc.createElement("interactors");
		for(int j=0;j<interactors().size();j++)
		{
			QDomElement currentInteractorNode = doc.createElement("interactor");
			interactors()[j]->toXMLNode(&doc,&currentInteractorNode);
			interactorsNode.appendChild(currentInteractorNode);

		}
		layerDescription.appendChild(interactorsNode);
		root.appendChild(layerDescription);
	}
	out << doc.toString();
}

void medAbstractLayeredView::restoreState(QDomElement* element)
{
	QDomNodeList nodeList=element->elementsByTagName("navigator");
	QList<medAbstractNavigator*> navigatorList=navigators();
	if(navigatorList.size()!=nodeList.size())
	{
		qWarning()<< "inconsistent data, failed to reload navigator parameters";
	}
	for(int i=0;i<navigatorList.size();i++)
	{
		QDomElement currentNavigator=nodeList.at(i).toElement();
        //check interactor name and version
        if(navigatorList[i]->name()==currentNavigator.attributeNode("name").value() && navigatorList[i]->version()==currentNavigator.attributeNode("version").value())
            navigatorList[i]->fromXMLNode(&currentNavigator);
        else
            qWarning()<<"failed to a navigator: attempted to load navigator "<<
                      currentNavigator.attributeNode("name").value()<<"v"<<currentNavigator.attributeNode("version").value()         <<"in navigator "<<
                      navigatorList[i]->name()                      <<"v"<<navigatorList[i]->version();
	}
	
	nodeList=element->elementsByTagName("interactor");
	QList<medAbstractInteractor*> interactorList=interactors();
	if(interactorList.size()!=nodeList.size())
	{
		qWarning()<< "inconsistent data, failed to reload interactor parameters";
	}
	for(int i=0;i<interactorList.size();i++)
	{
		QDomElement currentInteractor=nodeList.at(i).toElement();
        if(interactorList[i]->name()==currentInteractor.attributeNode("name").value() && interactorList[i]->version()==currentInteractor.attributeNode("version").value())
		interactorList[i]->fromXMLNode(&currentInteractor);
        else
            qDebug()<<"failed to a interactor: attempted to load interactor "<<
                      currentInteractor.attributeNode("name").value()<<"v"<<currentInteractor.attributeNode("version").value()         <<"in interactor "<<
                      interactorList[i]->name()                      <<"v"<<interactorList[i]->version();
	}
}
