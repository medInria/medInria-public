/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDiffeomorphicDemonsRegistrationProcess.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include <itkImageRegistrationMethod.h>

#include <itkImage.h>
#include <itkResampleImageFilter.h>

#include <time.h>

#include <DiffeomorphicDemons/rpiDiffeomorphicDemons.hxx>
#include <rpiCommonTools.hxx>
#include <registrationFactory.h>

#include <medStringParameter.h>
#include <medStringListParameter.h>
#include <medBoolParameter.h>
#include <medDoubleParameter.h>

// /////////////////////////////////////////////////////////////////
// medDiffeomorphicDemonsRegistrationDiffeomorphicDemonsPrivate
// /////////////////////////////////////////////////////////////////

class medDiffeomorphicDemonsRegistrationPrivate
{
public:
    medDiffeomorphicDemonsRegistrationProcess * proc;
    template <class PixelType>
            int update();
    template < typename TFixedImage, typename TMovingImage >
           bool write(const QString&);
    void * registrationMethod ;

    std::vector<unsigned int> iterations;

    QStringList updateRules;
    QStringList gradientTypes;

    medStringParameter *iterationParam;
    medStringListParameter *updateRuleParam;
    medStringListParameter *gradientTypeParam;
    medDoubleParameter *maximumUpdateStepLengthParam;
    medDoubleParameter *updateFieldStandardDeviationParam;
    medDoubleParameter *displacementFieldStandardDeviationParam;
    medBoolParameter *useHistogramMatchingParam;
    QList<medAbstractParameter*> parameters;
};

// /////////////////////////////////////////////////////////////////
// medDiffeomorphicDemonsRegistrationProcess
// /////////////////////////////////////////////////////////////////

medDiffeomorphicDemonsRegistrationProcess::medDiffeomorphicDemonsRegistrationProcess() : itkProcessRegistration(), d(new medDiffeomorphicDemonsRegistrationPrivate)
{
    d->proc = this;
    d->registrationMethod = NULL ;

    //set transform type for the exportation of the transformation to a file
    this->setProperty("transformType","nonRigid");

    d->iterationParam = new medStringParameter("Iterations", this);
    d->iterationParam->setValue("15x10x5");
    d->iterationParam->setToolTip(tr("Each number of iteration per level must be separated by \"x\". From coarser to finest levels"));

    d->updateRuleParam = new medStringListParameter("Update Rule", this);
    d->updateRules << "Diffeomorphic" << "Additive" << "Compositive";
    d->updateRuleParam->addItems( d->updateRules );
    d->updateRuleParam->setValue("Diffeomorphic");
    //TODO: find a way to add tooltip for each item
//    d->updateRuleBox->setItemData(0,"s <- s o exp(u)",Qt::ToolTipRole);
//    d->updateRuleBox->setItemData(1,"s <- s + u",Qt::ToolTipRole);
//    d->updateRuleBox->setItemData(2,"s <- s o (Id+u)",Qt::ToolTipRole);

    d->gradientTypeParam = new medStringListParameter("Gradient Type", this);
    d->gradientTypes << tr("Symmetrized") << tr ("Fixed Image")
                 << tr("Warped Moving Image")
                 << tr("Mapped Moving Image");
    d->gradientTypeParam->addItems( d->gradientTypes );
    d->gradientTypeParam->setValue("Symmetrized");

    d->maximumUpdateStepLengthParam = new medDoubleParameter("Max. Update Step Length", this);
    d->maximumUpdateStepLengthParam->setRange(0,100);
    d->maximumUpdateStepLengthParam->setSingleStep(0.01);
    d->maximumUpdateStepLengthParam->setValue(2.0);
    d->maximumUpdateStepLengthParam->setToolTip(tr(
                "Maximum length of an update vector (voxel units)."
                " Setting it to 0 implies no restrictions will be made"
                " on the step length."));

    d->updateFieldStandardDeviationParam = new medDoubleParameter("Update Field Std. Deviation", this);
    d->updateFieldStandardDeviationParam->setRange(0,1000);
    d->updateFieldStandardDeviationParam->setSingleStep(0.01);
    d->updateFieldStandardDeviationParam->setValue(0.0);
    d->updateFieldStandardDeviationParam->setToolTip(tr(
                "Standard deviation of the Gaussian smoothing"
                "of the update field (voxel units). Setting it below 0.1"
                "means no smoothing will be performed (default 0.0)."));

    d->displacementFieldStandardDeviationParam = new medDoubleParameter("Displ. Field Std. Deviation", this);
    d->displacementFieldStandardDeviationParam->setRange(0,1000);
    d->displacementFieldStandardDeviationParam->setSingleStep(0.01);
    d->displacementFieldStandardDeviationParam->setValue(1.5);
    d->displacementFieldStandardDeviationParam->setToolTip(tr(
                "Standard deviation of the Gaussian smoothing of "
                "the displacement field (voxel units). Setting it below 0.1 "
                "means no smoothing will be performed (default 1.5)."));

    d->useHistogramMatchingParam = new medBoolParameter("Histogram Matching", this);
    d->useHistogramMatchingParam->setValue(false);
    d->useHistogramMatchingParam->setToolTip(tr(
                "Use histogram matching before processing?"));

    d->parameters << d->iterationParam;
    d->parameters << d->updateRuleParam;
    d->parameters << d->gradientTypeParam;
    d->parameters << d->maximumUpdateStepLengthParam;
    d->parameters << d->updateFieldStandardDeviationParam;
    d->parameters << d->displacementFieldStandardDeviationParam;
    d->parameters << d->useHistogramMatchingParam;
}

medDiffeomorphicDemonsRegistrationProcess::~medDiffeomorphicDemonsRegistrationProcess()
{
    d->proc = NULL;
    switch(fixedImageType()){
    //only float will be used here, diffeoMorphic demons only work on float and double.

    default:
    {
        typedef itk::Image< float, 3 >  RegImageType;
        delete static_cast<rpi::DiffeomorphicDemons< RegImageType, RegImageType,
                float > *>(d->registrationMethod);
    }
        break;
    }
    d->registrationMethod = NULL;
    delete d;
    d = NULL;
}

bool medDiffeomorphicDemonsRegistrationProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medDiffeomorphicDemonsRegistrationProcess",
                                                                      createmedDiffeomorphicDemonsRegistration, "medAbstractRegistrationProcess");
}

QString medDiffeomorphicDemonsRegistrationProcess::description() const
{
    return "medDiffeomorphicDemonsRegistrationProcess";
}

QString medDiffeomorphicDemonsRegistrationProcess::identifier() const
{
    return "medDiffeomorphicDemonsRegistrationProcess";
}

QList<medAbstractParameter*> medDiffeomorphicDemonsRegistrationProcess::parameters()
{
    return d->parameters;
}

// /////////////////////////////////////////////////////////////////
// Templated Version of update
// /////////////////////////////////////////////////////////////////


template <typename PixelType>
        int medDiffeomorphicDemonsRegistrationPrivate::update()
{
    typedef itk::Image< PixelType, 3 >  FixedImageType;
    typedef itk::Image< PixelType, 3 >  MovingImageType;

    //unfortunately diffeomorphic demons only work with double or float types...
    // so we need to use a cast filter.
    typedef itk::Image< float, 3 > RegImageType;
    typedef double TransformScalarType;
    typedef rpi::DiffeomorphicDemons< RegImageType, RegImageType,
                    TransformScalarType > RegistrationType;
    RegistrationType * registration = new RegistrationType;

    registrationMethod = registration;

    registration->SetFixedImage((FixedImageType*)proc->fixedImage().GetPointer());
    registration->SetMovingImage((MovingImageType*)proc->movingImages()[0].GetPointer());

    registration->SetNumberOfIterations(rpi::StringToVector<unsigned int>(
                                            iterationParam->value().toStdString()));
    registration->SetMaximumUpdateStepLength(maximumUpdateStepLengthParam->value());
    registration->SetUpdateFieldStandardDeviation(updateFieldStandardDeviationParam->value());
    registration->SetDisplacementFieldStandardDeviation(displacementFieldStandardDeviationParam->value());
    registration->SetUseHistogramMatching(useHistogramMatchingParam->value());

    // Set update rule
    switch( updateRules.indexOf(updateRuleParam->value()) )
    {
    case 0:
        registration->SetUpdateRule( RegistrationType::UPDATE_DIFFEOMORPHIC ); break;
    case 1:
        registration->SetUpdateRule( RegistrationType::UPDATE_ADDITIVE );      break;
    case 2:
        registration->SetUpdateRule( RegistrationType::UPDATE_COMPOSITIVE );   break;
    default:
        throw std::runtime_error( "Update rule must fit in the range [0,2]." );
    }

    // Set gradient type
    switch( gradientTypes.indexOf(gradientTypeParam->value()) )
    {
    case 0:
        registration->SetGradientType( RegistrationType::GRADIENT_SYMMETRIZED );
        break;
    case 1:
        registration->SetGradientType( RegistrationType::GRADIENT_FIXED_IMAGE );
        break;
    case 2:
        registration->SetGradientType( RegistrationType::GRADIENT_WARPED_MOVING_IMAGE );
        break;
    case 3:
        registration->SetGradientType( RegistrationType::GRADIENT_MAPPED_MOVING_IMAGE );
        break;
    default:
        throw std::runtime_error( "Gradient type must fit in the range [0,3]." );
    }

    // Print method parameters
    QString methodParameters = proc->getTitleAndParameters();

    qDebug() << "METHOD PARAMETERS";
    qDebug() << methodParameters;

    // Run the registration
    time_t t1 = clock();
    try {
        registration->StartRegistration();
    }
    catch( std::exception & err )
    {
        qDebug() << "ExceptionObject caught ! (startRegistration)" << err.what();
        return 1;
    }
    time_t t2 = clock();

    qDebug() << "Elasped time: " << (double)(t2-t1)/(double)CLOCKS_PER_SEC;

    emit proc->progressed(80);

    typedef itk::ResampleImageFilter< MovingImageType,MovingImageType,TransformScalarType >    ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform(registration->GetTransformation());
    resampler->SetInput((const MovingImageType*)proc->movingImages()[0].GetPointer());
    resampler->SetSize( proc->fixedImage()->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin( proc->fixedImage()->GetOrigin() );
    resampler->SetOutputSpacing( proc->fixedImage()->GetSpacing() );
    resampler->SetOutputDirection( proc->fixedImage()->GetDirection() );
    resampler->SetDefaultPixelValue( 0 );

    try {
        resampler->Update();
    }
    catch (itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return 1;
    }

    itk::ImageBase<3>::Pointer result = resampler->GetOutput();
    qDebug() << "Resampled? ";
    result->DisconnectPipeline();

    if (proc->output())
        proc->output()->setData (result);

    return 0;
}


/**
 * @brief Runs the process.
 *
 * @param ImageType the fixed image image type.
 * @return int successful or not.
*/
int medDiffeomorphicDemonsRegistrationProcess::update(itkProcessRegistration::ImageType imgType)
{
    if(fixedImage().IsNull() || movingImages().isEmpty()
            || movingImages()[0].IsNull())
    {
        qWarning() << "Either the fixed image or the moving image is Null";
        return 1;
    }

    if (imgType != itkProcessRegistration::FLOAT)
    {
        qWarning() << "the imageType should be float, and it's :"<<imgType;
        return 1;
    }

    return d->update<float>();
}

itk::Transform<double,3,3>::Pointer medDiffeomorphicDemonsRegistrationProcess::getTransform(){
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    //normaly should use long switch cases, but here we know we work with float3 data.
    if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    {
        return registration->GetTransformation();
    }
    else
        return NULL;
}

QString medDiffeomorphicDemonsRegistrationProcess::getTitleAndParameters(){
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    //normaly should use long switch cases, but here we know we work with float3 data.
    typedef rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> RegistrationType;
    RegistrationType * registration = static_cast<RegistrationType *>(d->registrationMethod);

    QString titleAndParameters;
    titleAndParameters += "DiffeomorphicDemons\n";
    titleAndParameters += "  Max number of iterations   : " + QString::fromStdString(rpi::VectorToString(registration->GetNumberOfIterations())) + "\n";
    switch (registration->GetUpdateRule())
    {
        case 0:
            titleAndParameters += "  Update rule   : DIFFEOMORPHIC\n";
            break;
        case 1:
            titleAndParameters += "  Update rule   : ADDITIVE\n";
            break;
        case 2:
            titleAndParameters += "  Update rule   : COMPOSITIVE\n";
            break;
        default:
            titleAndParameters += "  Update rule   : Unknown\n";
    }

    switch( registration->GetGradientType() )
    {
        case 0:
            titleAndParameters += "  Gradient type   : SYMMETRIZED\n";
            break;
        case 1:
            titleAndParameters += "  Gradient type   : FIXED_IMAGE\n";
            break;
        case 2:
            titleAndParameters += "  Gradient type   : WARPED_MOVING_IMAGE\n";
            break;
        case 3:
            titleAndParameters += "  Gradient type   : MAPPED_MOVING_IMAGE\n";
            break;
        default:
            titleAndParameters += "  Gradient type   : Unknown\n";
    }

    titleAndParameters += "  Maximum step length   : " + QString::number(registration->GetMaximumUpdateStepLength()) + " (voxel unit)\n";
    titleAndParameters += "  Update field standard deviation   : " + QString::number(registration->GetUpdateFieldStandardDeviation()) + " (voxel unit)\n";
    titleAndParameters += "  Displacement field standard deviation   : " + QString::number(registration->GetDisplacementFieldStandardDeviation()) + " (voxel unit)\n";
    titleAndParameters += "  Use histogram matching   : " + QString::fromStdString(rpi::BooleanToString(registration->GetUseHistogramMatching())) + "\n";

    return titleAndParameters;
}

/**
 * @brief Writes the transformation, in this case the displacement field,
 * in a file.
 *
 * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
 * @return bool successful or not.
*/
bool medDiffeomorphicDemonsRegistrationProcess::writeTransform(const QString& file)
{
    typedef float PixelType;
    typedef double TransformScalarType;
    typedef itk::Image< PixelType, 3 > RegImageType;
    //normaly should use long switch cases, but here we know we work with float3 data.
    if (rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> * registration =
            static_cast<rpi::DiffeomorphicDemons<RegImageType,RegImageType,TransformScalarType> *>(d->registrationMethod))
    {
        try{
            rpi::writeDisplacementFieldTransformation<TransformScalarType, RegImageType::ImageDimension>(
                        registration->GetTransformation(),
                        file.toStdString());
        }
        catch (std::exception)
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////
/**
 * @brief Function to instantiate the process from the factory.
 * @see registered()
 *
 * @param void
*/
dtkAbstractProcess *createmedDiffeomorphicDemonsRegistration()
{
    return new medDiffeomorphicDemonsRegistrationProcess;
}

