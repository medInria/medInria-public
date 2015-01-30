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
#include <medLinearTransformation.h>
#include <medAbstractTransformation.h>
#include <medAbstractDataFactory.h>
#include <medDataIndexParameter.h>
#include <medPathParameter.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medRpiApplyTransformationCommand.h>

#include <itkProcessRegistration.h>
#include <rpiDisplacementFieldTransform.h>
#include <itkImageRegistrationFactory.h>
#include <itkCastImageFilter.h>
#include <itkAffineTransform.h>

#include <QUndoCommand>


// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationPrivate
// /////////////////////////////////////////////////////////////////

class medRpiApplyTransformationProcessPrivate
{
public:
    medDataIndexParameter* displFieldTransfoParam;
    medPathParameter* linearTransfoParam;
    medDataIndexParameter* SVFTransfoParam;

    medAbstractTransformation* currentTransfo;

    itk::ImageRegistrationFactory<medRpiApplyTransformationProcess::RegImageType>::Pointer factory;
};

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationProcess
// /////////////////////////////////////////////////////////////////

medRpiApplyTransformationProcess::medRpiApplyTransformationProcess() : medAbstractApplyTransformationProcess(), d(new medRpiApplyTransformationProcessPrivate)
{
    d->displFieldTransfoParam = new medDataIndexParameter("Displ Field Transfo", this);
    connect(d->displFieldTransfoParam, SIGNAL(valueChanged(const medDataIndex &)),this,SLOT(addDisplFieldTransfo(const medDataIndex &)));

    d->linearTransfoParam = new medPathParameter("Linear Transfo", this);
    connect(d->linearTransfoParam, SIGNAL(valueChanged(QString)),this,SLOT(addLinearTransfo(QString)));

    d->SVFTransfoParam = new medDataIndexParameter("SVF Transfo", this);
    connect(d->SVFTransfoParam, SIGNAL(valueChanged(const medDataIndex &)),this,SLOT(addSVFTransfo(const medDataIndex &)));

    typedef itk::Image< float, 3 > RegImageType;
    d->factory = itk::ImageRegistrationFactory<RegImageType>::New();

    d->currentTransfo = NULL;
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
    params << d->displFieldTransfoParam;
    params << d->linearTransfoParam;
    params << d->SVFTransfoParam;
    return params;
}

int medRpiApplyTransformationProcess::update()
{
    if(d->currentTransfo)
    {
        addTransformation(d->currentTransfo);

        //TODO: delete current transfo after application?
    }

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
 }

void medRpiApplyTransformationProcess::addDisplFieldTransfo(const medDataIndex &index)
{
    if (!index.isValid()){
        return;
    }

    medAbstractImageData* displFieldImage = dynamic_cast <medAbstractImageData *> ( medDataManager::instance()->retrieveData(index));
    medDisplacementFieldTransformation* displFieldImageTransfo = new medDisplacementFieldTransformation;
    displFieldImageTransfo->setParameter(displFieldImage);

    d->currentTransfo = displFieldImageTransfo;
}

void medRpiApplyTransformationProcess::addSVFTransfo(const medDataIndex &index)
{
    if (!index.isValid()){
        return;
    }

    medAbstractImageData* SVFImage = dynamic_cast <medAbstractImageData *> ( medDataManager::instance()->retrieveData(index));
    medSVFTransformation* SVFTransfo = new medSVFTransformation;
    SVFTransfo->setParameter(SVFImage);

    d->currentTransfo = SVFTransfo;

}

void medRpiApplyTransformationProcess::addLinearTransfo(QString transfoFile)
{
    if (transfoFile != "")
    {
        itk::TransformFileReader::Pointer tmpTrRead = itk::TransformFileReader::New();
        tmpTrRead->SetFileName (transfoFile.toStdString());

        itk::AffineTransform<> * affineTransfo = NULL;
        medLinearTransformation *transfo = NULL;

        try
        {
            tmpTrRead->Update();

            itk::TransformFileReader::TransformListType trsfList = * (tmpTrRead->GetTransformList());
            itk::TransformFileReader::TransformListType::iterator tr_it = trsfList.begin();

            transfo = new medLinearTransformation;
            affineTransfo = dynamic_cast <itk::AffineTransform<> *> ((*tr_it).GetPointer());
        }
        catch (itk::ExceptionObject &e)
        {
            qWarning() << "Unable to read linear transform...";
        }

        if(affineTransfo)
        {
            QMatrix4x4 qmatrix;
            for(int i=0; i<3; i++)
                for(int j=0; j<3; j++)
                    qmatrix(i,j) =  affineTransfo->GetMatrix()[i][j];

            for(int i=0; i<3; i++)
                qmatrix(i,3) = affineTransfo->GetOffset()[i];

            qmatrix(3,3) = 1;

            transfo->setMatrix(qmatrix);

            d->currentTransfo = transfo;
        }
    }
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

