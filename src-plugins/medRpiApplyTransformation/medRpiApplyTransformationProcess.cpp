/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medRpiApplyTransformationProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medAbstractImageData.h>
#include <medSVFTransformation.h>
#include <medDisplacementFieldTransformation.h>
#include <medAbstractTransformation.h>
#include <medAbstractDataFactory.h>
#include <medDataIndexParameter.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medRpiApplyTransformationCommand.h>

#include <itkProcessRegistration.h>
#include <rpiDisplacementFieldTransform.h>
#include <itkImageRegistrationFactory.h>
#include <itkCastImageFilter.h>

#include <QUndoCommand>


// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class medRpiApplyTransformationProcessPrivate
{
public:
    medDataIndexParameter* DisplFieldTransfoParam;
    medAbstractImageData *DisplFieldImage;


    itk::ImageRegistrationFactory<medRpiApplyTransformationProcess::RegImageType>::Pointer factory;

};

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationProcess
// /////////////////////////////////////////////////////////////////

medRpiApplyTransformationProcess::medRpiApplyTransformationProcess() : medAbstractApplyTransformationProcess(), d(new medRpiApplyTransformationProcessPrivate)
{
    d->DisplFieldTransfoParam = new medDataIndexParameter("Displ Field Transfo", this);
    connect(d->DisplFieldTransfoParam, SIGNAL(valueChanged(const medDataIndex &)),this,SLOT(addDisplFieldTransfo(const medDataIndex &)));

    typedef itk::Image< float, 3 > RegImageType;
    d->factory = itk::ImageRegistrationFactory<RegImageType>::New();
}

medRpiApplyTransformationProcess::~medRpiApplyTransformationProcess()
{
    delete d;
    d = NULL;
}

bool medRpiApplyTransformationProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medRpiApplyTransformationProcess",
                                                                      createmedRpiApplyTransformation, "medAbstractApplyTransformationProcess");
}

QString medRpiApplyTransformationProcess::description() const
{
    return "medRpiApplyTransformationProcess";
}

QString medRpiApplyTransformationProcess::identifier() const
{
    return "medRpiApplyTransformationProcess";
}

QList<medAbstractParameter*> medRpiApplyTransformationProcess::parameters()
{
    QList<medAbstractParameter *> params;
    params << d->DisplFieldTransfoParam;
    return params;
}

int medRpiApplyTransformationProcess::update()
{
    if(!this->geometry() || !this->inputImage() || this->transformationStack().isEmpty())
    {
        qDebug() << "Wrong parameters, could not apply rpi transformation...";
        return EXIT_FAILURE;
    }

    qDebug() << "medRpiApplyTransformationProcess update !!!";
    return EXIT_SUCCESS;
}

bool medRpiApplyTransformationProcess::isInteractive() const
{
    return false;
}

 void medRpiApplyTransformationProcess::addTransformation(medAbstractTransformation *transfo)
 {
     // dummy itkProcessRegistration to beneficiate from input conversions
     // TODO: find a better solution
     itkProcessRegistration* dummyProcess = new itkProcessRegistration();
     dummyProcess->setFixedInput(this->input<medAbstractData>(0));
     dummyProcess->setMovingInput(this->input<medAbstractData>(1));

     d->factory->SetFixedImage((RegImageType*)dummyProcess->fixedImage().GetPointer());
     d->factory->SetMovingImage((RegImageType*)dummyProcess->movingImages()[0].GetPointer());

     medRpiApplyTransformationCommand* command = new medRpiApplyTransformationCommand(transfo, d->factory);
     connect(command, SIGNAL(commandUndone()), this, SLOT(handleEndOfCommand()));
     connect(command, SIGNAL(commandDone()), this, SLOT(handleEndOfCommand()));

     this->commandStack()->push(command);

     this->setOutput<medAbstractData>(command->output(), 0);
     handleOutputs();

 }

void medRpiApplyTransformationProcess::addDisplFieldTransfo(const medDataIndex &index)
{
    if (!index.isValid()){
        return;
    }

    medAbstractImageData* displFieldImage = dynamic_cast <medAbstractImageData *> ( medDataManager::instance()->retrieveData(index));
    medDisplacementFieldTransformation* displFieldImageTransfo = new medDisplacementFieldTransformation;
    displFieldImageTransfo->setParameter(displFieldImage);

    addTransformation(displFieldImageTransfo);
}

void medRpiApplyTransformationProcess::handleEndOfCommand()
{
    medRpiApplyTransformationCommand* command = dynamic_cast<medRpiApplyTransformationCommand*>(this->sender());
    this->setOutput<medAbstractData>(command->output(), 0);
    handleOutputs();
}

dtkAbstractProcess *createmedRpiApplyTransformation()
{
    return new medRpiApplyTransformationProcess;
}

