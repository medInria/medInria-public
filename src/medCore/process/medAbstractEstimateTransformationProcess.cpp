/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractEstimateTransformationProcess.h>

#include <medAbstractImageData.h>
#include <medAbstractTransformation.h>
#include <medTriggerParameter.h>
#include <medToolBox.h>
#include <medMessageController.h>


class medAbstractEstimateTransformationProcessPrivate
{
public:
    medTriggerParameter* saveTransfoParameter;
    QPointer<medToolBox> toolbox;
};



medAbstractEstimateTransformationProcess::medAbstractEstimateTransformationProcess(medAbstractProcess *parent):
    medAbstractProcess(parent), d(new medAbstractEstimateTransformationProcessPrivate)
{
    medProcessInput<medAbstractImageData> *fixed = new medProcessInput<medAbstractImageData>("Fixed Image", false);
    this->appendDataInput( fixed );

    medProcessInput<medAbstractImageData> *moving = new medProcessInput<medAbstractImageData>("Moving Image", false);
    this->appendDataInput( moving );

    this->appendOutput( new medProcessOutput<medAbstractTransformation>("Output"));

    d->saveTransfoParameter = new medTriggerParameter("Save Transfo", this);
    d->saveTransfoParameter->setButtonText("Save Transfo");

    d->saveTransfoParameter->getPushButton()->setEnabled(false);

    connect(d->saveTransfoParameter, SIGNAL(triggered()), this, SLOT(saveTransform()));
    connect(this, SIGNAL(success()), this, SLOT(enableSaveTransform()));
}

medAbstractEstimateTransformationProcess::~medAbstractEstimateTransformationProcess()
{

}

bool medAbstractEstimateTransformationProcess::isInteractive() const
{
    return false;
}

medToolBox* medAbstractEstimateTransformationProcess::toolbox()
{
    if(d->toolbox.isNull())
    {
        d->toolbox = new medToolBox;
        d->toolbox->addWidget(this->parameterWidget());
        d->toolbox->addWidget(d->saveTransfoParameter->getPushButton());
        d->toolbox->addWidget(this->runParameter()->getPushButton());
    }
    return d->toolbox;
}

bool medAbstractEstimateTransformationProcess::saveTransform()
{
    QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save transform file"),QDir::homePath());

    bool res = false;
    if(!fileName.isEmpty())
        res = writeTransform(fileName);
    else return false;

    if(res)
        medMessageController::instance()->showInfo("File successfully saved", 2000);
    else medMessageController::instance()->showError("Error saving file", 2000);

    return res;
}

void medAbstractEstimateTransformationProcess::enableSaveTransform()
{
    d->saveTransfoParameter->getPushButton()->setEnabled(true);
}
