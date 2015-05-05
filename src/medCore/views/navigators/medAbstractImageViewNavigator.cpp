/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageViewNavigator.h>

#include <medAbstractImageView.h>

#include <medCompositeParameter.h>
#include <medVector3DParameter.h>
#include <medTimeLineParameter.h>
#include <medDataIndex.h>
#include <medDataManager.h>


class medAbstractImageViewNavigatorPrivate
{
public:
    medAbstractImageView *view;
    medCompositeParameter *cameraParameter;
    medVector3DParameter *positionBeingViewedParameter;
    medTimeLineParameter *timeLineParameter;

};

medAbstractImageViewNavigator::medAbstractImageViewNavigator(medAbstractView *parent):
    medAbstractLayeredViewNavigator(parent), d(new medAbstractImageViewNavigatorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView *>(parent);
    d->positionBeingViewedParameter = NULL;
    d->cameraParameter = NULL;
    d->timeLineParameter = NULL;

    d->view = dynamic_cast<medAbstractImageView *>(parent);
    if(!d->view)
    {
        qWarning() << "Derived type of medAbstractImageViewNavigator should always be parented \
                      with derived type of medAbstractImageView.";
        return;
    }

    connect(this, SIGNAL(currentTimeChanged(double)), d->view, SIGNAL(currentTimeChanged(double)));
    connect(d->view, SIGNAL(layerAdded(uint)), this, SLOT(updateTimeLineParameter()));
    connect(d->view, SIGNAL(layerRemoved(uint)), this, SLOT(updateTimeLineParameter()));
}

medAbstractImageViewNavigator::~medAbstractImageViewNavigator()
{
    delete d;
}

medCompositeParameter* medAbstractImageViewNavigator::cameraParameter()
{
    if(!d->cameraParameter)
    {
        d->cameraParameter = new medCompositeParameter("Camera", this);
        d->cameraParameter->addVariant("Camera Position", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Camera Up", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Camera Focal", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Parallel Scale", QVariant((double)0.0), 0, HUGE_VAL);
        connect(d->cameraParameter, SIGNAL(valuesChanged(QHash<QString,QVariant>)), this, SLOT(setCamera(QHash<QString,QVariant>)));
    }
    return d->cameraParameter;
}

medAbstractVector3DParameter* medAbstractImageViewNavigator::positionBeingViewedParameter()
{
    if(!d->positionBeingViewedParameter)
    {
        d->positionBeingViewedParameter = new medVector3DParameter("Position", this);
        connect(d->positionBeingViewedParameter, SIGNAL(valueChanged(QVector3D)), this, SLOT(moveToPosition(QVector3D)));
    }
    return d->positionBeingViewedParameter;
}

medTimeLineParameter* medAbstractImageViewNavigator::timeLineParameter()
{
    if(!d->timeLineParameter)
    {
        d->timeLineParameter = new medTimeLineParameter("TimeLine", this);
        connect(d->timeLineParameter, SIGNAL(timeChanged(double)), this, SLOT(setCurrentTime(double)));
    }
    return d->timeLineParameter;
}

void medAbstractImageViewNavigator::setCurrentTime (const double &time)
{
    emit currentTimeChanged(time);
    d->view->render();
}

void medAbstractImageViewNavigator::updateTimeLineParameter()
{
    bool viewHasTemporalData = false;

    double sequenceDuration = 0;
    double sequenceFrameRate = 0;
    foreach(medDataIndex index, d->view->dataList())
    {
        medAbstractData *data = medDataManager::instance()->retrieveData(index);
        if (!data)
            continue;

        if(data->hasMetaData("SequenceDuration") && data->hasMetaData("SequenceFrameRate"))
        {
            double sd = data->metadata("SequenceDuration").toDouble();
            sequenceDuration = (sequenceDuration < sd) ? sd : sequenceDuration;

            double sf = data->metadata("SequenceFrameRate").toDouble();
            sequenceFrameRate = (sequenceFrameRate < sf) ? sf : sequenceFrameRate;

            viewHasTemporalData = true;
        }
    }
    if(viewHasTemporalData)
    {
        unsigned int numFrames = (unsigned int)floor(sequenceDuration * sequenceFrameRate + 0.5);
        d->timeLineParameter->setNumberOfFrame(numFrames);
        d->timeLineParameter->setDuration(sequenceDuration);
        d->timeLineParameter->show();
    }
    else
        d->timeLineParameter->hide();
}

void medAbstractImageViewNavigator::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    qDebug()<<"in medAbstractImageViewNavigator::toXMLNode(QDomDocument* doc,QDomElement* currentNode)";
    currentNode->setAttribute("name",name());
    currentNode->setAttribute("version",version());
    QDomElement elmt=doc->createElement("parameters");
    elmt.setAttribute("number",this->linkableParameters().size());
    for(int i=0;i<linkableParameters().size();i++)
    {
        if(linkableParameters()[i]->name()=="Camera")   //composite parameters cannot save themselves
            continue;
        QDomElement paramNode=doc->createElement("parameter");
        linkableParameters()[i]->toXMLNode(doc,&paramNode);
        elmt.appendChild(paramNode);
    }

    //handle Camera parameter we skipped previously:
    QHash<QString,QString> paramsToBeSaved;
    paramsToBeSaved["Camera Position"]="QVector3D";
    paramsToBeSaved["Camera Up"]="QVector3D";
    paramsToBeSaved["Camera Focal"]="QVector3D";
    paramsToBeSaved["Parallel Scale"]="double";

    QHashIterator<QString, QString> it(paramsToBeSaved);
    while (it.hasNext())
    {
        it.next();

        qDebug()<<it.key()<<" "<<it.value();

        QDomElement paramNode=doc->createElement("parameter");
        QDomElement nameNode=doc->createElement("name");
        nameNode.appendChild(doc->createTextNode(it.key()));
        QDomElement typeNode=doc->createElement("type");
        typeNode.appendChild(doc->createTextNode(it.value()));
        QDomElement valueNode=doc->createElement("value");
        QString textValue;
        QVariant var=d->cameraParameter->value(it.key());
        if(var==QVariant())
            continue;

        if(it.value()=="QVector3D")
        {
            QVector3D obj=var.value<QVector3D>();
            textValue=QString::number(obj.x())+" "+QString::number(obj.y())+" "+QString::number(obj.z());

        }
        else if(it.value()=="double")
        {
            double obj=var.toDouble();
            textValue=QString::number(obj);
        }
        valueNode.appendChild(doc->createTextNode(textValue));

        paramNode.appendChild(nameNode);
        paramNode.appendChild(typeNode);
        paramNode.appendChild(valueNode);

        elmt.appendChild(paramNode);
    }

    currentNode->appendChild(elmt);
}
