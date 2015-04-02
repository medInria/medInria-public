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
#include <medLinearTransformation.h>
#include <medAbstractTransformation.h>
#include <medAbstractDataFactory.h>
#include <medDataIndexParameter.h>
#include <medDataIndex.h>
#include <medDataManager.h>

#include <itkProcessRegistration.h>
#include <rpiDisplacementFieldTransform.h>
#include <itkStationaryVelocityFieldTransform.h>
#include <itkImageRegistrationFactory.h>

#include <itkCastImageFilter.h>
#include <itkAffineTransform.h>



medRpiApplyTransformationCommand::medRpiApplyTransformationCommand(medAbstractTransformation *transfo,
                                                                   itk::ImageRegistrationFactory<RegImageType>::Pointer factory)
    :medAbstractApplyTransformationCommand(transfo)
{
    m_transfo = transfo;
    m_factory = factory;
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
        applyDisplFieldTransfo(displFieldtransfo);
        return;
    }

    medLinearTransformation *linearTransfo = dynamic_cast<medLinearTransformation *>(m_transfo);
    if(linearTransfo)
    {
        applyLinearTransfo(linearTransfo);
        return;
    }

    medSVFTransformation *svfTransfo = dynamic_cast<medSVFTransformation *>(m_transfo);
    if(svfTransfo)
    {
        applySVFTransfo(svfTransfo);
        return;
    }
}

void medRpiApplyTransformationCommand::applyDisplFieldTransfo(medDisplacementFieldTransformation *displFieldtransfo)
{
    this->setText("Displacement Field Transfo");

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
        typedef itk::Image< VectorPixelType, Dimension > ConvertedImageType; // We always convert to itk::Vector< double, 3 >
        typedef itk::Image<itk::Vector< float, Dimension >, Dimension> ImageType; // input data type
        typedef itk::CastImageFilter<ImageType, ConvertedImageType> CastFilterType;

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

    emit commandDone();

}

void medRpiApplyTransformationCommand::applySVFTransfo(medSVFTransformation* svfTransfo)
{
    this->setText("SVF Transfo");

    medAbstractImageData *transfoImage = svfTransfo->parameter();

    typedef float       PixelType;
    typedef double      VectorComponentType;
    const   unsigned int        Dimension = 3;

    typedef itk::Vector< VectorComponentType, Dimension  > VectorPixelType;
    typedef itk::Image< VectorPixelType, Dimension > DeformationFieldType;
    typedef itk::StationaryVelocityFieldTransform<VectorComponentType, Dimension> SVFTransformType;
    typedef rpi::DisplacementFieldTransform<VectorComponentType, Dimension> DisplacementFieldTransformType;

    DeformationFieldType::Pointer deformationField = NULL;

    if(transfoImage->PixelType() == typeid(itk::Vector< float, Dimension >))
    {
        typedef itk::Image< VectorPixelType, Dimension > ConvertedImageType; // We always convert to itk::Vector< double, 3 >
        typedef itk::Image<itk::Vector< float, Dimension >, Dimension> ImageType; // input data type
        typedef itk::CastImageFilter<ImageType, ConvertedImageType> CastFilterType;

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

    const SVFTransformType::Pointer svfTransform = SVFTransformType::New();
    svfTransform->SetParametersAsVectorField( deformationField );

    const DisplacementFieldTransformType::Pointer displacementFieldTransform = DisplacementFieldTransformType::New();
    displacementFieldTransform->SetParametersAsVectorField( svfTransform->GetDisplacementFieldAsVectorField() );

    itk::Transform<VectorComponentType,3,3>::ConstPointer itkTransform = static_cast<itk::Transform<VectorComponentType,3,3>::ConstPointer>(displacementFieldTransform);

    m_factory->GetGeneralTransform()->InsertTransform(itkTransform);

    m_factory->Update();

    itk::ImageBase<3>::Pointer result = m_factory->GetOutput();
    result->DisconnectPipeline();

    m_output = medAbstractDataFactory::instance()->create("medItkFloat3ImageData");
    m_output->setData(result);

    emit commandDone();
}

void medRpiApplyTransformationCommand::applyLinearTransfo(medLinearTransformation* linearTransfo)
{
    this->setText("Linear Transfo");

    typedef double      VectorComponentType;

    QMatrix4x4 qmatrix = linearTransfo->matrix();

    const itk::AffineTransform<>::Pointer itkTransfo = itk::AffineTransform<>::New();
    itk::AffineTransform<>::MatrixType matrix;
    itk::AffineTransform<>::OutputVectorType offset;

    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            matrix[i][j] = qmatrix(i,j);

    for(int i=0; i<3; i++)
        offset[i] = qmatrix(i,3);

    itkTransfo->SetMatrix(matrix);
    itkTransfo->SetOffset(offset);

    itk::Transform<VectorComponentType,3,3>::ConstPointer test = static_cast<itk::Transform<VectorComponentType,3,3>::ConstPointer>(itkTransfo);
    m_factory->GetGeneralTransform()->InsertTransform(test);

    m_factory->Update();

    itk::ImageBase<3>::Pointer result = m_factory->GetOutput();
    result->DisconnectPipeline();

    m_output = medAbstractDataFactory::instance()->create("medItkFloat3ImageData");
    m_output->setData(result);

    emit commandDone();
}

medAbstractData* medRpiApplyTransformationCommand::output()
{
    return m_output;
}
