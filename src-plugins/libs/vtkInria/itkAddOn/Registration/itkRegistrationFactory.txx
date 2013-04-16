/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkRegistrationFactory.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTranslationTransform.h"
#include "itkTransformToDeformationFieldFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkWarpImageFilter.h"
#include "itkDisplacementFieldTransform.h"

namespace itk
{

/*
 * Constructor
 */
template <typename TImage>
RegistrationFactory<TImage>
::RegistrationFactory()
{

  this->SetNumberOfRequiredOutputs( 1 );  // for the Transform

  m_GeneralTransform = GeneralTransformType::New();

  // create the output
  ImagePointer imageDecorator = static_cast<ImageType*>(this->MakeOutput(0).GetPointer());
  this->ProcessObject::SetNthOutput( 0, imageDecorator.GetPointer() );

  // register linear transform into factory for I/O purposes
  itk::TransformFactory<LinearTransformType>::RegisterTransform ();

  // settings
  m_FixedImage   = 0; // has to be provided by the user.
  m_MovingImage  = 0; // has to be provided by the user.
  m_InitialTransform = 0; // can provided by the user.
  m_Method = 0; // has to be provided by the user.
  m_UseFixedImage = 1; // it's better
  m_WarperGridSize = 16;
  m_SkipResampling = 0;
  m_UseInnerTransformation = 1;
}


/*
 * Initialize by setting the interconnects between components.
 */
template <typename TImage>
bool
RegistrationFactory<TImage>
::CheckInputs()
{
  if( !m_FixedImage || !m_MovingImage )
  {
    return 0;
  }

  return 1;

}



/*
 * Initialize by setting the interconnects between components.
 */
template <typename TImage>
void
RegistrationFactory<TImage>
::Initialize()
{


  typename MethodListType::iterator it;

  for ( it = this->MethodList.begin(); it != this->MethodList.end(); ++it)
  {
    (*it).second->Initialize ();
  }

  try
  {
    this->Reset();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e;
    itkExceptionMacro ("Error when initializing factory.");
  }


}




/*
 * Initialize by setting the interconnects between components.
 */
template <typename TImage>
void
RegistrationFactory<TImage>
::Reset()
{
  m_GeneralTransform->RemoveAllTransforms();
  this->SetInitialTransform (NULL);
  this->Modified();
  /*
  if (this->GetUseInnerTransformation() && this->CheckInputs() && !this->CheckImagesConsistency (this->GetFixedImage(), this->GetMovingImage() ) )
  {
    this->SetInitialTransform (this->GetTargetToSourceInnerTransform (this->GetFixedImage(), this->GetMovingImage()).GetPointer());
  }
  */
}

template < typename TImage >
DataObject::Pointer
RegistrationFactory<TImage>
::MakeOutput(unsigned int output)
{
  switch (output)
  {
    case 0:
      return static_cast<DataObject*>(ImageType::New().GetPointer());
      break;
    default:
      itkExceptionMacro("MakeOutput request for an output number larger than the expected number of outputs");
      return 0;
  }
}

/*
 *  Get Output
 */
template < typename TImage >
typename RegistrationFactory<TImage>::ImageType *
RegistrationFactory<TImage>
::GetOutput()
{
  return static_cast< ImageType* >( this->ProcessObject::GetOutput(0) );
}


/*
 *  Get Output
 */
template < typename TImage >
const typename RegistrationFactory<TImage>::ImageType *
RegistrationFactory<TImage>
::GetOutput() const
{
  return static_cast< const ImageType* >( this->ProcessObject::GetOutput(0) );
}

  /*
 *  Get Output
 */
template < typename TImage >
void
RegistrationFactory<TImage>
::SetInitialTransform(TransformConstPointerType transform)
{
  if (!transform)
    return;

  m_InitialTransform = transform;
  m_GeneralTransform->InsertTransform (m_InitialTransform);
}


template <typename TImage>
typename RegistrationFactory<TImage>::MethodPointerType
RegistrationFactory<TImage>
::FindMethodById (unsigned int id)
{

  typename MethodListType::iterator iterator;

  iterator = this->MethodList.find (id);

  if (iterator != this->MethodList.end())
    return this->MethodList[id];
  return NULL;
}



template <typename TImage>
typename RegistrationFactory<TImage>::MethodPointerType
RegistrationFactory<TImage>
::FindMethodByName (const char* name)
{
  typename MethodListType::iterator iterator;

  for (iterator = this->MethodList.begin(); iterator != this->MethodList.end(); iterator++)
  {
    MethodPointerType method = (*iterator).second;
    if( strcmp( name, method->GetName() ) == 0 )
    {
        return method;
    }
  }
  return NULL;
}


/*
 * Generate Data
 */
template <typename TImage>
void
RegistrationFactory<TImage>
::GenerateData()
{
  itkExceptionMacro ("this method should not be called from here but in sub-classes instead !");
}




template <typename TImage>
void
RegistrationFactory<TImage>
::SetFixedImage( ImageType * fixedImage )
{
  itkDebugMacro("setting Fixed Image to " << fixedImage );

  if (this->m_FixedImage.GetPointer() != fixedImage )
  {
    this->m_FixedImage = fixedImage;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(0,
				     const_cast< ImageType*>( fixedImage ) );

    typename MethodListType::iterator it;

    for ( it = this->MethodList.begin(); it != this->MethodList.end(); ++it)
    {
      (*it).second->SetFixedImage(fixedImage);
    }

    this->Modified();
    /*
    if (this->GetUseInnerTransformation() && this->CheckInputs() && !this->CheckImagesConsistency (this->GetFixedImage(), this->GetMovingImage() ) )
    {
      this->SetInitialTransform (this->GetTargetToSourceInnerTransform (this->GetFixedImage(), this->GetMovingImage()).GetPointer());
    }
    */
  }

}



template <typename TImage>
void
RegistrationFactory<TImage>
::SetMovingImage( ImageType * movingImage )
{
  itkDebugMacro("setting Moving Image to " << movingImage );

  if (this->m_MovingImage.GetPointer() != movingImage )
  {
    this->m_MovingImage = movingImage;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(1,
				     const_cast< ImageType*>( movingImage ) );

    typename MethodListType::iterator it;

    for ( it = this->MethodList.begin(); it != this->MethodList.end(); ++it)
    {
      (*it).second->SetMovingImage(movingImage);
    }

    this->Modified();

    /*
    if (this->GetUseInnerTransformation() && this->CheckInputs() && !this->CheckImagesConsistency (this->GetFixedImage(), this->GetMovingImage() ) )
    {
      this->SetInitialTransform (this->GetTargetToSourceInnerTransform (this->GetFixedImage(), this->GetMovingImage()).GetPointer());
    }
    */
  }
}


template <typename TImage>
void
RegistrationFactory<TImage>
::SetGeneralTransform(GeneralTransformType* genTrans)
{
  m_GeneralTransform = genTrans;
  if (m_InitialTransform)
    m_GeneralTransform->InsertTransform (m_InitialTransform);
};


template <typename TImage>
void
RegistrationFactory<TImage>
::AddRegistrationMethod (unsigned int method_id, MethodPointerType method)
{
  MethodPointerType found = this->FindMethodById (method_id);
  if (!found.IsNull())
  {
    itkExceptionMacro(<<"method id already exist !");
  }

  typename CallbackType::Pointer callback = CallbackType::New();
  method->AddObserver (itk::ProgressEvent(), callback);
  callback->SetItkObjectToWatch (this);

  if(!m_MovingImage.IsNull())
      method->SetMovingImage(m_MovingImage);

  if(!m_FixedImage.IsNull())
      method->SetFixedImage(m_FixedImage);

  this->MethodList[method_id] = method;
}


template <typename TImage>
void
RegistrationFactory<TImage>
::RemoveRegistrationMethod (unsigned int method_id)
{
  MethodPointerType found = this->FindMethodById (method_id);
  if (found.IsNull())
  {
    itkExceptionMacro(<<"method id doesn't exist !");
  }

  typename MethodListType::iterator it = this->MethodList.find(method_id);
  this->MethodList.erase(it);
}



template <typename TImage>
void
RegistrationFactory<TImage>
::Undo()
{
  if (m_GeneralTransform->Undo())
    this->Modified();
}



template <typename TImage>
void
RegistrationFactory<TImage>
::Redo()
{
  if (m_GeneralTransform->Redo())
    this->Modified();
}


template <typename TImage>
void
RegistrationFactory<TImage>
::ApplyRegistrationMethod (unsigned int method_id)
{

  if (!this->CheckInputs())
    itkExceptionMacro(<<"inputs not set correctly !");

  MethodPointerType method = this->FindMethodById (method_id);
  if (method.IsNull())
  {
    itkExceptionMacro(<<"could not find requested method !");
    return;
  }

  if (this->GetUseInnerTransformation() && this->CheckInputs() && (m_GeneralTransform->GetNumberOfTransformsInStack() == 0)
      && !this->CheckImagesConsistency (this->GetFixedImage(), this->GetMovingImage() ) )
  {
    this->SetInitialTransform (this->GetTargetToSourceInnerTransform (this->GetFixedImage(), this->GetMovingImage()).GetPointer());
  }

  try
  {
//     if (this->CheckForceResampling())
//     {
//       this->SkipResamplingOff();
//       this->UpdateLargestPossibleRegion();
//     }

    this->UpdateProgress (0);

    method->SetFixedImage(this->GetFixedImage());
    method->SetMovingImage(this->GetOutput());

    method->Update();

    m_GeneralTransform->InsertTransform (method->GetTransform());
  }
  catch (itk::ExceptionObject& err )
  {
    this->UpdateProgress (1);
    this->SetProgress (0);
    std::cerr << err << std::endl;
    itkExceptionMacro ("Error in RegistrationFactory<TImage>::ApplyRegistrationMethod ()");
  }

  this->UpdateProgress (1);
  this->SetProgress (0);

  this->Modified();

}

template <typename TImage>
void
RegistrationFactory<TImage>
::ReadFixedImage (const char* filename)
{
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName (filename);
  try
  {
    reader->Update();
    this->SetFixedImage (reader->GetOutput ());
  }
  catch (itk::ExceptionObject& err )
  {

    std::cerr << err << std::endl;
    itkExceptionMacro ("Error in RegistrationFactory<TImage>::ReadFixedImage ()");
  }

}


template <typename TImage>
void
RegistrationFactory<TImage>
::ReadMovingImage (const char* filename)
{
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName (filename);
  try
  {
    reader->Update();
    this->SetMovingImage (reader->GetOutput ());
  }
  catch (itk::ExceptionObject& err )
  {
    std::cerr << err << std::endl;
    itkExceptionMacro ("Error in RegistrationFactory<TImage>::ReadMovingImage ()");
  }

}



template <typename TImage>
void
RegistrationFactory<TImage>
::WriteOutput (const char* filename)
{
  if (!this->CheckInputs())
    itkExceptionMacro(<<"inputs not set correctly !");

  typename ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName (filename);
  writer->SetInput (this->GetOutput());
  try
  {
    writer->Update();
  }
  catch (itk::ExceptionObject& err )
  {
    std::cerr << err << std::endl;
    itkExceptionMacro ("Error in RegistrationFactory<TImage>::WriteOutput ()");
  }
}

template <typename TImage>
void
RegistrationFactory<TImage>
::WriteGlobalDisplacementField (const char* filename)
{
  if (!this->CheckInputs())
    itkExceptionMacro(<<"inputs not set correctly !");

  typename DisplacementFieldType::Pointer displacement = NULL;

  try
  {
    displacement = this->ExportGlobalDisplacementField();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    itkExceptionMacro(<<"could not export warped image !");
  }

  typename ImageFileWriter<DisplacementFieldType>::Pointer writer = ImageFileWriter<DisplacementFieldType>::New();
  writer->SetFileName (filename);
  writer->SetInput (displacement);
  writer->Update();
}



template <typename TImage>
void
RegistrationFactory<TImage>
::WriteWarpedImage (const char* filename)
{
  if (!this->CheckInputs())
    itkExceptionMacro(<<"inputs not set correctly !");

  typename WarpedImageType::Pointer warpedimage = NULL;

  try
  {
    warpedimage = this->ExportWarpedImage();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    itkExceptionMacro(<<"could not write warped image !");
  }

  typename ImageFileWriter<WarpedImageType>::Pointer writer = ImageFileWriter<WarpedImageType>::New();
  writer->SetFileName (filename);
  writer->SetInput (warpedimage);
  writer->Update();
}




template <typename TImage>
typename RegistrationFactory<TImage>::WarpedImagePointerType
RegistrationFactory<TImage>
::ExportWarpedImage()
{

//   if (!this->CheckInputs())
//     itkExceptionMacro(<<"inputs not set correctly !");

  typename DisplacementFieldType::Pointer displacement = NULL;

  try
  {
    displacement = this->ExportGlobalDisplacementField();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    itkExceptionMacro(<<"could not export warped image !");
  }

  this->UpdateProgress (0);

  typename WarpedImageType::Pointer gridimage = WarpedImageType::New();
  /*
    gridimage->SetRegions( displacement->GetRequestedRegion() );
    gridimage->SetOrigin(  displacement->GetOrigin() );
    gridimage->SetSpacing( displacement->GetSpacing() );
    gridimage->SetDirection( displacement->GetDirection() );
  */
  gridimage->SetRegions( this->GetMovingImage()->GetRequestedRegion() );
  gridimage->SetOrigin(  this->GetMovingImage()->GetOrigin() );
  gridimage->SetSpacing( this->GetMovingImage()->GetSpacing() );
  gridimage->SetDirection( this->GetMovingImage()->GetDirection() );
  gridimage->Allocate();
  gridimage->FillBuffer(0);

  typedef itk::ImageRegionIteratorWithIndex<WarpedImageType> GridImageIteratorWithIndex;
  GridImageIteratorWithIndex itergrid = GridImageIteratorWithIndex( gridimage, gridimage->GetRequestedRegion() );

  typename WarpedImageType::SizeType size = this->GetMovingImage()->GetLargestPossibleRegion().GetSize();

  unsigned int gridspacing[2];
  for (unsigned int i=0; i<2; i++)
  {
    gridspacing[i]= (unsigned int)((double)size[i]/(double)m_WarperGridSize);
  }

  itk::Index<WarpedImageType::ImageDimension> index;

  for (itergrid.GoToBegin(); !itergrid.IsAtEnd(); ++itergrid)
  {
    index = itergrid.GetIndex();
    if ( (index[0]%gridspacing[0])==0 ||
	 (index[1]%gridspacing[1])==0 )
    {
      itergrid.Set( itk::NumericTraits<unsigned char>::max() );
    }
  }

  typedef itk::WarpImageFilter < WarpedImageType, WarpedImageType, DisplacementFieldType >  WarperType;
  typename WarperType::Pointer warper = WarperType::New();
  warper->SetInput( gridimage );
  warper->SetOutputSpacing( displacement->GetSpacing() );
  warper->SetOutputOrigin( displacement->GetOrigin() );
  warper->SetOutputDirection( displacement->GetDirection() );
  warper->SetDeformationField( displacement );

  // instanciate callback to follow the warper
  typename CallbackType::Pointer callback = CallbackType::New();
  warper->AddObserver (itk::ProgressEvent(), callback);
  callback->SetItkObjectToWatch (this);

  this->UpdateProgress (0);

  // Update the warper
  try
  {
    warper->Update();
  }
  catch( itk::ExceptionObject& err )
  {
    this->UpdateProgress (1);
    this->SetProgress (0);
    std::cerr << err << std::endl;
    return NULL;
  }

  this->UpdateProgress (1);
  this->SetProgress (0);

  return warper->GetOutput();

}



template <typename TImage>
typename RegistrationFactory<TImage>::DisplacementFieldPointerType
RegistrationFactory<TImage>
::ExportGlobalDisplacementField()
{

  // Set up the TransformToDeformationFieldFilter
  typedef itk::TransformToDeformationFieldFilter <DisplacementFieldType, ParametersValueType> FieldGeneratorType;
  typename FieldGeneratorType::Pointer fieldGenerator = FieldGeneratorType::New();

  fieldGenerator->SetTransform( m_GeneralTransform );

  if ( this->CheckInputs() )
  {
    fieldGenerator->SetOutputRegion(m_FixedImage->GetRequestedRegion());
    fieldGenerator->SetOutputSpacing(m_FixedImage->GetSpacing());
    fieldGenerator->SetOutputOrigin(m_FixedImage->GetOrigin());
    fieldGenerator->SetOutputDirection(m_FixedImage->GetDirection());
  }
  else
  {
    if (m_GeneralTransform->GetNumberOfTransformsInStack() == 1)
    {
      //typedef typename itk::DisplacementFieldTransform<ParametersValueType, DisplacementFieldType::ImageDimension> DisplacementFieldTransformType;
      //const DisplacementFieldTransformType* transform = dynamic_cast<const DisplacementFieldTransformType*>(m_GeneralTransform->GetInput().GetPointer());

      const DisplacementFieldTransformType* displacementfieldtransform = dynamic_cast< const DisplacementFieldTransformType* >(m_GeneralTransform->GetTransform(0).GetPointer());

      if (!displacementfieldtransform || !(displacementfieldtransform->GetDisplacementField()))
      {
	itkExceptionMacro(<<"cannot export displacement field, wrong input type !");
      }
      else
      {
	fieldGenerator->SetOutputRegion(displacementfieldtransform->GetDisplacementField()->GetRequestedRegion());
	fieldGenerator->SetOutputSpacing(displacementfieldtransform->GetDisplacementField()->GetSpacing());
	fieldGenerator->SetOutputOrigin(displacementfieldtransform->GetDisplacementField()->GetOrigin());
	fieldGenerator->SetOutputDirection(displacementfieldtransform->GetDisplacementField()->GetDirection());
      }
    }

    else
      itkExceptionMacro(<<"cannot export displacement field, wrong input type !");
  }





  // instanciate callback to follow the field generator
  typename CallbackType::Pointer callback = CallbackType::New();
  fieldGenerator->AddObserver (itk::ProgressEvent(), callback);
  callback->SetItkObjectToWatch (this);

  this->UpdateProgress (0);

  // Update the fieldGenerator
  try
  {
    fieldGenerator->Update();
  }
  catch( itk::ExceptionObject& err )
  {
    this->UpdateProgress (1);
    this->SetProgress (0);
    std::cerr << err << std::endl;
    itkExceptionMacro("Error during displacement field export");
  }

  this->UpdateProgress (1);
  this->SetProgress (0);

  return fieldGenerator->GetOutput();

}

template <typename TImage>
void
RegistrationFactory<TImage>
::WriteGlobalLinearTransform (const char* filename)
{
  if (!this->CheckInputs())
    itkExceptionMacro(<<"inputs not set correctly !");

  typename LinearTransformType::Pointer transform = NULL;

  try
  {
    transform = this->ExportGlobalLinearTransform();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    itkExceptionMacro(<<"could not export linear transform !");
  }

  typename TransformFileWriter::Pointer writer = TransformFileWriter::New();
  writer->SetFileName (filename);
  writer->SetInput (transform);
  writer->Update();
}



template <typename TImage>
typename RegistrationFactory<TImage>::LinearTransformPointerType
RegistrationFactory<TImage>
::ExportGlobalLinearTransform()
{
  return m_GeneralTransform->GetGlobalLinearTransform();
}


template <typename TImage>
void
RegistrationFactory<TImage>
::DeepCopyImage ( ImageConstPointer input, ImagePointer output)
{
  ///\todo make sure the input image region is not corrupted
  if (input.IsNull() || output.IsNull())
  {
    throw itk::ExceptionObject(__FILE__,__LINE__,"Error in RegistrationFactory::DeepCopyImage(), images not set");
  }

  output->Initialize();
  output->SetRegions (input->GetLargestPossibleRegion());
  output->SetOrigin  (input->GetOrigin());
  output->SetSpacing (input->GetSpacing());
  output->SetDirection (input->GetDirection());

  output->Allocate();

  ImageRegionConstIterator<ImageType>   itIn  (input,  input->GetLargestPossibleRegion());
  ImageRegionIterator<ImageType>        itOut (output, input->GetLargestPossibleRegion());

  while ( ! itOut.IsAtEnd() )
  {
    itOut.Set(itIn.Get());

    ++itOut;
    ++itIn;
  }
}

  /**
   * Inform pipeline of required output region
   */
template <typename TImage>
void
RegistrationFactory<TImage>
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // get pointers to the input and output
  ImagePointer outputPtr = const_cast<ImageType*>(this->GetOutput());
  if ( !outputPtr )
  {
    return;
  }

  ImageConstPointer fixedImage = this->GetFixedImage();
  ImageConstPointer movingImage = this->GetMovingImage();

  if (!fixedImage && !movingImage)
    return;

  // Set the size of the output region
  if( m_UseFixedImage && fixedImage )
  {
    outputPtr->SetLargestPossibleRegion( fixedImage->GetLargestPossibleRegion() );
  }
  else
  {
    outputPtr->SetLargestPossibleRegion( movingImage->GetLargestPossibleRegion() );
  }

  outputPtr->SetRequestedRegionToLargestPossibleRegion();

  // Set spacing and origin
  if (m_UseFixedImage && fixedImage)
  {
    outputPtr->SetSpacing( fixedImage->GetSpacing() );
    outputPtr->SetOrigin( fixedImage->GetOrigin() );
    outputPtr->SetDirection( fixedImage->GetDirection() );
  }
  else
  {
    outputPtr->SetSpacing( movingImage->GetSpacing() );
    outputPtr->SetOrigin( movingImage->GetOrigin() );
    outputPtr->SetDirection( movingImage->GetDirection() );
  }
}

template <typename TImage>
void
RegistrationFactory<TImage>
::EnlargeOutputRequestedRegion(DataObject * ptr )
  {
    // call the superclass's implementation
    Superclass::EnlargeOutputRequestedRegion( ptr );

//     // set the output requested region to largest possible.
//     ImageType * outputPtr;
//     outputPtr = dynamic_cast<ImageType*>( ptr );

//     if( outputPtr )
//     {
//       outputPtr->SetRequestedRegionToLargestPossibleRegion();
//     }

  }

/*
 * PrintSelf
 */
template <typename TImage>
void
RegistrationFactory<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "General Transform: " << m_GeneralTransform.GetPointer() << std::endl;
  os << indent << "Fixed Image: " << m_FixedImage.GetPointer() << std::endl;
  os << indent << "Moving Image: " << m_MovingImage.GetPointer() << std::endl;
}


template <typename TImage>
typename RegistrationFactory<TImage>::LinearTransformPointerType
RegistrationFactory<TImage>
::GetTargetToSourceInnerTransform (ImageConstPointer target, ImageConstPointer source)
{
  typename LinearTransformType::Pointer ret = LinearTransformType::New();
  ret->SetIdentity();

  typename LinearTransformType::Pointer targettransfo = LinearTransformType::New();
  typename LinearTransformType::Pointer inversetargettransfo = LinearTransformType::New();
  targettransfo->SetIdentity();

//   typedef typename ImageType::ImageDimension Dimension;

  unsigned int dim = ImageType::ImageDimension;
//   for (unsigned int i=0; i<ImageDimension; i++)
//     dim++;

  typename LinearTransformType::ParametersType targetparam = typename LinearTransformType::ParametersType (dim*(dim+1));
  typename LinearTransformType::ParametersType sourceparam = typename LinearTransformType::ParametersType (dim*(dim+1));

  for (unsigned int i=0; i<dim; i++)
  {
    for (unsigned int j=0; j<dim; j++)
    {
      targetparam[dim*i+j] = target->GetDirection()[j][i];
      sourceparam[dim*i+j] = source->GetDirection()[j][i];
    }
  }
  for (unsigned int i=0; i<3; i++)
  {
    targetparam[dim*dim+i] = target->GetOrigin()[i];
    sourceparam[dim*dim+i] = source->GetOrigin()[i];
  }

  targettransfo->SetParameters (targetparam);
  targettransfo->GetInverse (inversetargettransfo);

  ret->SetParameters (sourceparam);
  ret->Compose (inversetargettransfo);

  return ret;
}

template <typename TImage>
bool
RegistrationFactory<TImage>
::CheckImagesConsistency (const ImageType* image1, const ImageType* image2, double epsilon)
{
  typename ImageType::PointType origin1 = image1->GetOrigin();
  typename ImageType::PointType origin2 = image2->GetOrigin();
  typename ImageType::DirectionType direction1 = image1->GetDirection();
  typename ImageType::DirectionType direction2 = image2->GetDirection();

  unsigned int dim = ImageType::ImageDimension;
  for (unsigned int i=0; i<dim; i++)
    for (unsigned int j=0; j<dim; j++)
      if (fabs (direction2[i][j] - direction1[i][j]) > epsilon)
	return false;
  for (unsigned int i=0; i<dim; i++)
    if (fabs (origin2[i] - origin1[i]) > epsilon)
      return false;
  return true;
}




template <typename TImage>
bool
RegistrationFactory<TImage>
::CheckForceResampling()
{
  bool ret = false;
  ret = ret || this->GetSkipResampling();
  ret = ret && !this->CheckImagesConsistency (this->GetFixedImage(), this->GetMovingImage());

  return ret;
}


} // end namespace itk



