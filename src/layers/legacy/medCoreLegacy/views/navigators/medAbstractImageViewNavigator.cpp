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

#include <medCompositeParameterL.h>
#include <medVector3DParameterL.h>
#include <medTimeLineParameterL.h>
#include <medDataIndex.h>
#include <medDataManager.h>


class medAbstractImageViewNavigatorPrivate
{
public:
    medAbstractImageView *view;
    medCompositeParameterL *cameraParameter;
    medVector3DParameterL *positionBeingViewedParameter;
    medTimeLineParameterL *timeLineParameter;

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
        dtkWarn() << "Derived type of medAbstractImageViewNavigator should always be parented \
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

medCompositeParameterL* medAbstractImageViewNavigator::cameraParameter()
{
    if(!d->cameraParameter)
    {
        d->cameraParameter = new medCompositeParameterL("Camera", this);
        d->cameraParameter->addVariant("Camera Position", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Camera Up", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Camera Focal", QVariant(QVector3D()));
        d->cameraParameter->addVariant("Parallel Scale", QVariant((double)0.0), 0, HUGE_VAL);
        connect(d->cameraParameter, SIGNAL(valuesChanged(QHash<QString,QVariant>)), this, SLOT(setCamera(QHash<QString,QVariant>)));
    }
    return d->cameraParameter;
}

medAbstractVector3DParameterL* medAbstractImageViewNavigator::positionBeingViewedParameter()
{
    if(!d->positionBeingViewedParameter)
    {
        d->positionBeingViewedParameter = new medVector3DParameterL("Position", this);
        connect(d->positionBeingViewedParameter, SIGNAL(valueChanged(QVector3D)), this, SLOT(moveToPosition(QVector3D)));
    }
    return d->positionBeingViewedParameter;
}

medTimeLineParameterL* medAbstractImageViewNavigator::timeLineParameter()
{
    if(!d->timeLineParameter)
    {
        d->timeLineParameter = new medTimeLineParameterL("TimeLine", this);
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
        unsigned int numFrames = (unsigned int)round(sequenceDuration * sequenceFrameRate + 1.0);
        d->timeLineParameter->setNumberOfFrame(numFrames);
        d->timeLineParameter->setDuration(sequenceDuration);
        d->timeLineParameter->show();
    }
    else
        d->timeLineParameter->hide();
}
