/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRpiApplyTransformationCommand.h>

#include <medAbstractImageData.h>
#include <medSVFTransformation.h>
#include <medDisplacementFieldTransformation.h>
#include <medAbstractTransformation.h>
#include <medAbstractDataFactory.h>
#include <medDataIndexParameter.h>
#include <medDataIndex.h>
#include <medDataManager.h>

#include <itkProcessRegistration.h>
#include <rpiDisplacementFieldTransform.h>
#include <itkImageRegistrationFactory.h>
#include <itkCastImageFilter.h>



medRpiApplyTransformationCommand::medRpiApplyTransformationCommand(medAbstractTransformation *transfo,
                                                                   itk::ImageRegistrationFactory<RegImageType>::Pointer factory)
    :medAbstractApplyTransformationCommand(transfo)
{
    m_transfo = transfo;
    m_factory = factory;

    this->setText("Apply Displacement Field Transfo");
}

medRpiApplyTransformationCommand::~medRpiApplyTransformationCommand()
{

}

void medRpiApplyTransformationCommand::undo()
{
    m_factory->Undo();
    m_factory->Update();

    itk::ImageBase<3>::Pointer result = m_factory->GetOutput();
    result->DisconnectPipeline();

    m_output = medAbstractDataFactory::instance()->create("medItkFloat3ImageData");
    m_output->setData(result);

    emit commandUndone();
}

void medRpiApplyTransformationCommand::redo()
{
    medDisplacementFieldTransformation *displFieldtransfo = dynamic_cast<medDisplacementFieldTransformation *>(m_transfo);
    if(displFieldtransfo)
    {
        medAbstractImageData *transfoImage = displFieldtransfo->parameter();

        typedef float       PixelType;
        typedef double      VectorComponentType;
        const   unsigned int        Dimension = 3;

        typedef itk::Vector< VectorComponentType, Dimension  > VectorPixelType;
        typedef itk::Image< VectorPixelType, Dimension > DeformationFieldType;
        typedef rpi::DisplacementFieldTransform<VectorComponentType, Dimension> DisplacementFieldTransformType;

        DeformationFieldType::Pointer deformationField = NULL;

        if(transfoImage->PixelType() == typeid(itk::Vector< float, Dimension >))
        {
            typedef typename itk::Image< VectorPixelType, Dimension > ConvertedImageType; // We always convert to itk::Vector< double, 3 >
            typedef typename itk::Image<itk::Vector< float, Dimension >, Dimension> ImageType; // input data type
            typedef typename itk::CastImageFilter<ImageType, ConvertedImageType> CastFilterType;

            CastFilterType::Pointer caster = CastFilterType::New();

            itk::Object *itkObj = static_cast<itk::Object*>(transfoImage->data());
            ImageType *vectorField = dynamic_cast<ImageType *>(itkObj);

            caster->SetInput(vectorField);
            caster->Update();
            deformationField = caster->GetOutput();
        }
        else if(transfoImage->PixelType() == typeid(itk::Vector< double, Dimension >))
        {
            itk::Object *itkObj = static_cast<itk::Object*>(transfoImage->data());
            deformationField = dynamic_cast<DeformationFieldType *>(itkObj);
        }

        const DisplacementFieldTransformType::Pointer displacementFieldTransform = DisplacementFieldTransformType::New();
        displacementFieldTransform->SetParametersAsVectorField( deformationField );

        itk::Transform<VectorComponentType,3,3>::ConstPointer test = static_cast<itk::Transform<VectorComponentType,3,3>::ConstPointer>(displacementFieldTransform);

        m_factory->GetGeneralTransform()->InsertTransform(test);

        m_factory->Update();

        itk::ImageBase<3>::Pointer result = m_factory->GetOutput();
        result->DisconnectPipeline();

        m_output = medAbstractDataFactory::instance()->create("medItkFloat3ImageData");
        m_output->setData(result);
    }

    emit commandDone();
}

medAbstractData* medRpiApplyTransformationCommand::output()
{
    return m_output;
}
