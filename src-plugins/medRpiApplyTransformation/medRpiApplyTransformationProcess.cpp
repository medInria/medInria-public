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
#include <medAbstractTransformation.h>
#include <medAbstractDataFactory.h>
#include <medDataIndexParameter.h>
#include <medDataIndex.h>
#include <medDataManager.h>

#include <itkProcessRegistration.h>
#include <rpiDisplacementFieldTransform.h>
#include <itkImageRegistrationFactory.h>
#include <itkCastImageFilter.h>


// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class medRpiApplyTransformationProcessPrivate
{
public:
    medDataIndexParameter* SVFTransfoParam;
    medAbstractImageData *SVFImage;

    typedef itk::Image< float, 3 > RegImageType;
    itk::ImageRegistrationFactory<RegImageType>::Pointer factory;

    // dummy itkProcessRegistration to beneficiate from input conversions
    // TODO: find a better solution
    itkProcessRegistration *dummyProcess;



};

// /////////////////////////////////////////////////////////////////
// medRpiApplyTransformationProcess
// /////////////////////////////////////////////////////////////////

medRpiApplyTransformationProcess::medRpiApplyTransformationProcess() : medAbstractApplyTransformationProcess(), d(new medRpiApplyTransformationProcessPrivate)
{
    d->SVFTransfoParam = new medDataIndexParameter("SVF Transfo", this);
    connect(d->SVFTransfoParam, SIGNAL(valueChanged(const medDataIndex &)),this,SLOT(onSVFDropped(const medDataIndex &)));

    typedef itk::Image< float, 3 > RegImageType;
    d->factory = itk::ImageRegistrationFactory<RegImageType>::New();
    d->dummyProcess = new itkProcessRegistration();
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
    params << d->SVFTransfoParam;
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

void medRpiApplyTransformationProcess::onSVFDropped(const medDataIndex &index)
{
    if (!index.isValid()){
        return;
    }

    d->SVFImage = dynamic_cast <medAbstractImageData *> ( medDataManager::instance()->retrieveData(index));

    typedef float       PixelType;
    typedef double      VectorComponentType;
    const   unsigned int        Dimension = 3;

    typedef itk::Vector< VectorComponentType, Dimension  > VectorPixelType;
    typedef itk::Image< VectorPixelType, Dimension > DeformationFieldType;
    typedef rpi::DisplacementFieldTransform<VectorComponentType, Dimension> DisplacementFieldTransformType;

    DeformationFieldType::Pointer deformationField = NULL;

    if(d->SVFImage->PixelType() == typeid(itk::Vector< float, Dimension >))
    {
        typedef typename itk::Image< VectorPixelType, Dimension > ConvertedImageType; // We always convert to itk::Vector< double, 3 >
        typedef typename itk::Image<itk::Vector< float, Dimension >, Dimension> ImageType; // input data type
        typedef typename itk::CastImageFilter<ImageType, ConvertedImageType> CastFilterType;

        CastFilterType::Pointer caster = CastFilterType::New();

        itk::Object *itkObj = static_cast<itk::Object*>(d->SVFImage->data());
        ImageType *vectorField = dynamic_cast<ImageType *>(itkObj);

        caster->SetInput(vectorField);
        caster->Update();
        deformationField = caster->GetOutput();
    }
    else if(d->SVFImage->PixelType() == typeid(itk::Vector< double, Dimension >))
    {
        itk::Object *itkObj = static_cast<itk::Object*>(d->SVFImage->data());
        deformationField = dynamic_cast<DeformationFieldType *>(itkObj);
    }

    const DisplacementFieldTransformType::Pointer displacementFieldTransform = DisplacementFieldTransformType::New();
    displacementFieldTransform->SetParametersAsVectorField( deformationField );

    itk::Transform<VectorComponentType,3,3>::ConstPointer test = static_cast<itk::Transform<VectorComponentType,3,3>::ConstPointer>(displacementFieldTransform);

    d->factory->GetGeneralTransform()->InsertTransform(test);

    typedef itk::Image< PixelType, 3 > RegImageType;
    d->dummyProcess->setFixedInput(this->input<medAbstractData>(0));
    d->dummyProcess->setMovingInput(this->input<medAbstractData>(1));

    d->factory->SetFixedImage((RegImageType*)d->dummyProcess->fixedImage().GetPointer());
    d->factory->SetMovingImage((RegImageType*)d->dummyProcess->movingImages()[0].GetPointer());

    d->factory->Update();

    itk::ImageBase<3>::Pointer result = d->factory->GetOutput();
    result->DisconnectPipeline();

    medAbstractData *output = medAbstractDataFactory::instance()->create("medItkFloat3ImageData");
    output->setData(result);
    this->setOutput<medAbstractData>(output, 0);

    handleOutputs();
}


dtkAbstractProcess *createmedRpiApplyTransformation()
{
    return new medRpiApplyTransformationProcess;
}

