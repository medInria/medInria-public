/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include "itkProcessObject.h"
#include "itkRegistrationMethod.h"
#include "itkGeneralTransform.h"
#include "itkDataObjectDecorator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkTransformFactory.h"
#include "itkDisplacementFieldTransform.h"

/* #include "itkAffineRegistrationMethod.h" */
/* #include "itkLandmarkRegistrationMethod.h" */
/* #include "itkDiffeomorphicDemonsRegistrationMethod.h" */

#include <map>

namespace itk
{


/**
   \class CommandIterationUpdate
   \brief Just a little class to handle iteration event observations

   \ingroup Command
   \author Nicolas Toussaint, INRIA

*/
class CommandIterationUpdate : public itk::Command
{
 public:
  typedef CommandIterationUpdate  Self;
  typedef itk::Command            Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::ProcessObject      ProcessObjectType;
  typedef ProcessObjectType::Pointer  ProcessObjectPointer;

  itkNewMacro( Self );

  void SetItkObjectToWatch (ProcessObjectPointer object)
  {
    m_ProcessObject = object;
  }


 protected:
  CommandIterationUpdate() {};
  ~CommandIterationUpdate() {};

  ProcessObjectPointer m_ProcessObject;

 public:

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    if( !(itk::ProgressEvent().CheckEvent( &event )) )
     return;

    const ProcessObjectType * filter = dynamic_cast< const ProcessObjectType * >( object );
    if (!filter)
      return;


    if (m_ProcessObject)
    {
      m_ProcessObject->UpdateProgress (filter->GetProgress());
      //std::cout<<"factory at : "<<filter->GetProgress()<<std::endl;
    }

  }
};









  /** \class RegistrationFactory (itk)
   *   \brief Factory for any type of Registration.
   * This class was contributed by Nicolas Toussaint, INRIA.
   * The aim of this class is to provide a versatile API that can
   * hold various registration methods. The user is able to
   * switch between registration methods, accumulate them, undo and redo
   * registration processes.
   *
   * The class is templated over the image type, but only scalar images
   * of dimension 2 or 3 are supported by the currently implemented registration methods
   *
   * This class is a process object that takes 2 input images. The output of the process object
   * is an image of the same type as the inputs.
   * Inputs images can be set with SetFixedImage() and SetMovinImages() methods.
   * Output image can be recovered by GetOutput().
   *
   * The different available methods are listed in RegistrationMethodTypeIds enum. The user
   * can apply (process) a specific registration method by calling ApplyRegistrationMethod().
   * Use Initialize() after the inputs are set and before applying any registration method.
   * Use Update() after a registration method has been applied to be able to recover the resampled
   * output image.
   *
   * Registration methods can be applied iteratively. Meaning that you can call ApplyRegistrationMethod()
   * several times (and with different registration methods). The global transformation will
   * pipeline each iterative transformation.
   *
   * Use ExportGlobalDisplacementField() to recover a displacement field describing the
   * global transformation. Currently this class cannot export Parameters of the global transform. But the user
   * can attempt to export the global transformation as a linear transform (e.g. affine, similarity or rigid)
   * assuming all the registration steps are linear based. For that use ExportGlobalLinearTransform() method
   *
   * Use Undo() and Redo() calls to recover former transformations or re-apply them.
   *
   * Use SetInitialTransform() to plug an initial transformation (of any type) at the beginning
   * of the global transformation pipeline.
   * Use Reset() to erase all the previously applyed transformations. Be carefull : transformations
   * are truely deleted, use Undo() to recover previous registration steps without deleting transformations
   *
   * WriteOutput() and WriteDisplacementField() do what they say !
   *
   * =============================================================================
   *
   * Currently supported methods :
   *
   * 1. RegistrationFactory::REGISTRATION_RIGID
   *
   * the method used for rigid registration is LandmarkRegistrationMethod .
   * Using a set of pair of landmarks, i.e. correspondence points between the Fixed
   * and the Moving image. This method will find the best fitting rigid transformation
   * that match this set of points in a least square meaning.
   *
   * 2. RegistrationFactory::REGISTRATION_AFFINE
   *
   * the method used for affine registration is AffineRegistrationMethod .
   * Using a mutual information criterion, and a multi level registration algorithm,
   * this method will find the best fitting affine transformation between input images.
   *
   * 3. RegistrationFactory::REGISTRATION_DIFFEOMORPHIC_DEMONS
   *
   * the method used for non-linear registration is DiffeomorphicDemonsRegistrationMethod .
   * The diffeomorphic Demons algorithm has been contributed by Tom Vercauteren, INRIA & Mauna Kea Technologies.
   * see \class DiffeomorphicDemonsRegistrationFilter for more details
   *
   *
   * \ingroup   ProcessObject
   *
   * \sa LandmarkRegistrationMethod
   * \sa AffineRegistrationMethod
   * \sa DiffeomorphicDemonsRegistrationMethod
   * \sa GeneralTransform
   * \sa RegistrationMethod
   *
   * \author Nicolas Toussaint, Florence Dru, INRIA
   */

  template <typename TImage>
  class ITK_EXPORT RegistrationFactory : public ProcessObject
  {

  public:


    typedef RegistrationFactory  Self;
    typedef ProcessObject Superclass;
    typedef SmartPointer<Self>   Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(RegistrationFactory, ProcessObject);

    /**  Type of the Fixed image. */
    typedef          TImage                     ImageType;
    typedef typename ImageType::Pointer         ImagePointer;
    typedef typename ImageType::ConstPointer    ImageConstPointer;

    typedef ImageFileReader<ImageType> ImageReaderType;
    typedef ImageFileWriter<ImageType> ImageWriterType;

    typedef          RegistrationMethod<ImageType> MethodType;
    typedef typename MethodType::Pointer           MethodPointerType;

    typedef typename MethodType::ParametersValueType ParametersValueType;

    typedef          GeneralTransform<ParametersValueType, ImageType::ImageDimension> GeneralTransformType;
    typedef typename GeneralTransformType::Pointer GeneralTransformPointerType;

    typedef typename GeneralTransformType::TransformType TransformType;
    typedef typename TransformType::Pointer TransformPointerType;
    typedef typename TransformType::ConstPointer TransformConstPointerType;

    typedef typename GeneralTransformType::MatrixOffsetTransformType LinearTransformType;
    typedef typename LinearTransformType::Pointer LinearTransformPointerType;

/*     typedef itk::Vector< ParametersValueType, ImageType::ImageDimension >    VectorPixelType; */
/*     typedef itk::Image<  VectorPixelType, ImageType::ImageDimension > DisplacementFieldType; */
/*     typedef typename DisplacementFieldType::Pointer DisplacementFieldPointerType; */
    typedef itk::DisplacementFieldTransform<ParametersValueType, ImageType::ImageDimension> DisplacementFieldTransformType;
    typedef typename DisplacementFieldTransformType::Pointer DisplacementFieldTransformPointerType;
    typedef typename DisplacementFieldTransformType::DisplacementFieldType DisplacementFieldType;
    typedef typename DisplacementFieldType::Pointer DisplacementFieldPointerType;
    typedef itk::ImageFileReader<DisplacementFieldType> DisplacementFieldReaderType;

    typedef itk::Image<unsigned char, ImageType::ImageDimension> WarpedImageType;
    typedef typename WarpedImageType::Pointer WarpedImagePointerType;


    typedef itk::CommandIterationUpdate CallbackType;
    typedef typename CallbackType::Pointer CallbackPointerType;

    typedef std::map<unsigned int, MethodPointerType> MethodListType;
    
    /** Set/Get the Fixed image. */
    void SetFixedImage( ImageType * fixedImage );
    itkGetObjectMacro( FixedImage, ImageType );

    /** Set/Get the Moving image. */
    void SetMovingImage( ImageType * movingImage );
    itkGetObjectMacro( MovingImage, ImageType );

    /** Set/Get the Transform. */
    void SetGeneralTransform(GeneralTransformType*);
    itkGetObjectMacro( GeneralTransform, GeneralTransformType );


    /** Initialize by setting the interconnects between the components. */
    virtual void Initialize();

    /** Check if inputs are present or not. */
    virtual bool CheckInputs();

    /** Returns the transform resulting from the registration process  */
    virtual ImageType * GetOutput();

    /** Returns the transform resulting from the registration process  */
    virtual const ImageType * GetOutput() const;

    /** Make a DataObject of the correct type to be used as the specified
     * output. Internal use only*/
    virtual DataObjectPointer MakeOutput(unsigned int idx);

    /** Set/Get the initial transform. This transform will be plugged into
	the "general transform" as first transform of the pipeline.
	this transform is removed when calling Reset() */
    void SetInitialTransform(TransformConstPointerType transform);
    itkGetConstObjectMacro (InitialTransform, TransformType);

    /** By default the registration factory does not contain any registration method,
	so the user can basically do nothing with it. Use this method to insert the different
	methods you will need for registration. Perform these methods by calling ApplyRegistrationMethod() */
    virtual void AddRegistrationMethod (unsigned int method_id, MethodPointerType method);
    /** Removes a registration method by its id. */
    virtual void RemoveRegistrationMethod (unsigned int method_id);
    /** Undo the last registration step. The method only un-plugs the last transformation from the
	"transform pipeline" and calls a Modified(). The user might need to call Update()
	to have an up to date output image. */
    virtual void Undo();
    /** Redo the last registration step. The method only re-plug the last transformation to the
	"transform pipeline" and calls a Modified(). The user might need to call Update()
	to have an up to date output image. */
    virtual void Redo();
    /** Applies a specific registration method. After the method is performed, output
	transform is plugged into the "transformation pipeline", and a call of Modified() is done.
	The user might need to call Update() to have an up to date output image */
    virtual void ApplyRegistrationMethod (unsigned int method_id);
    /** Finds a specific method by its id */
    virtual MethodPointerType FindMethodById (unsigned int id);
    /** Finds a specific method by its name */
    virtual MethodPointerType FindMethodByName (const char* name);

    /** Set/Get the warp grid size. This parameter is used during the
	exportation of the warp image (ExportWarpedImage()) and defines
	the size of the grid that is used in warping process. By default
	this size is set to 16. */
    itkGetMacro (WarperGridSize, unsigned int);
    itkSetMacro (WarperGridSize, unsigned int);
    /** Set/Get the boolean value that says if we should use the
	reorientation transformation camong from the input orientation matrices
	and origins. This transformation will be applied as initial transform */
    itkGetMacro (UseInnerTransformation, bool);
    itkSetMacro (UseInnerTransformation, bool);
    itkBooleanMacro (UseInnerTransformation);
    /** Set/Get the boolean value that says if we should resample the output
	image weither using FixedImage dimensions *OR* using MovingImage dimensions.
	We have to do that choice before setting the input images. Note that if set to true
	(default) the moving image is resampled through the fixed image dimensions at first update.
	this means that the output image is of FixedImage dimensions and spacing. Note also that
	if set to false, the direct comparison of the FixedImage and Output image is no longer
	reliable. You will need to resample the Output image at your convenience outside the factory. */
    itkGetMacro (UseFixedImage, bool);
    itkSetMacro (UseFixedImage, bool);
    itkBooleanMacro (UseFixedImage);
    /** Resets the transformation pipeline, removes all transformations from pipeline,
	actually destroy them. A call of Modified() is done. Call Update() to get an
	up to date output image */
    virtual void Reset();
    /** Writes the output image into file. The output image is resampled and transformed. */
    virtual void ReadFixedImage (const char* filename);
    /** Writes the output image into file. The output image is resampled and transformed. */
    virtual void ReadMovingImage (const char* filename);
    /** Writes the output image into file. The output image is resampled and transformed. */
    virtual void WriteOutput (const char* filename);
    /** Attempts to concatenate all transforms in linear forms and write the linear matrix
	into file. If one or more transforms of the pipeline are not linear (such as a deformation field),
	then an exception is thrown. */
    virtual void WriteGlobalLinearTransform (const char* filename);
    /** This method concatenates all transforms into a global deformation field, and
	writes it into filename */
    virtual void WriteGlobalDisplacementField (const char* filename);
    /** Warps a regular grid (its size can be set using SetWarperGridSize()) with the
	current global deformation field, and writes the output warped image into filename. */
    virtual void WriteWarpedImage (const char* filename);
    /** Attempts to concatenate all transforms in linear forms and write the linear matrix
	into file. If one or more transforms of the pipeline are not linear (such as a deformation field),
	then an exception is thrown. */
    virtual LinearTransformPointerType ExportGlobalLinearTransform();
    /** This method concatenates all transforms into a global deformation field. It does not
	care if transforms are linear or not */
    virtual DisplacementFieldPointerType ExportGlobalDisplacementField();
    /** Warps a regular grid (its size can be set using SetWarperGridSize()) with the
	current global deformation field */
    virtual WarpedImagePointerType ExportWarpedImage();

    /** Convenient method to deep copy an image into another image. output image has to be instanciated
	outside the method */
    static void DeepCopyImage ( ImageConstPointer input, ImagePointer output);

    itkGetMacro (SkipResampling, bool);
    itkSetMacro (SkipResampling, bool);
    itkBooleanMacro (SkipResampling);

    /** Returns the linear transform that goes from source to target image
	according to the images origins and orientation instances given by GetOrigin()
	and GetDirection() methods */
    static LinearTransformPointerType GetTargetToSourceInnerTransform (ImageConstPointer target, ImageConstPointer source);
    /** This method returns true the origins and directions of image1 and image2
	do NOT differ, false otherwise. Precision is given by epsilon */ 
    static bool CheckImagesConsistency (const ImageType* image1, const ImageType* image2, double epsilon=0.001);
    
  protected:
    RegistrationFactory();
    virtual ~RegistrationFactory() {};
    void PrintSelf(std::ostream& os, Indent indent) const;

    /** Method invoked by the pipeline in order to trigger the computation of
     * the registration. */
    virtual void  GenerateData ();

    /**
       ResampleImageFilter produces an image which is a different size
       * than its input.  As such, it needs to provide an implementation
       * for GenerateOutputInformation() in order to inform the pipeline
       * execution model.  The original documentation of this method is
       * below. \sa ProcessObject::GenerateOutputInformaton()
       */
    virtual void GenerateOutputInformation();
    /** The current implementation of this class does not supprot
     * streaming. As such it produces the output for the largest
     * possible region. */
    virtual void EnlargeOutputRequestedRegion( DataObject *ptr );

    virtual bool CheckForceResampling();
    
    ImagePointer m_MovingImage;
    ImagePointer m_FixedImage;

    GeneralTransformPointerType m_GeneralTransform;
    TransformConstPointerType m_InitialTransform;

    MethodPointerType m_Method;

    unsigned int m_WarperGridSize;
    bool m_UseFixedImage;
    bool m_SkipResampling;
    bool m_UseInnerTransformation;
    


  private:
    RegistrationFactory(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    MethodListType MethodList;


  };


} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegistrationFactory.txx"
#endif


