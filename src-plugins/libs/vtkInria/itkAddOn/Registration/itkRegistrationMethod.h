/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkProcessObject.h"
#include "itkOrientedImage.h"

#include "itkTransform.h"
#include "itkCommand.h"

namespace itk
{
  
/**
   \class RegistrationMethod
   \brief Base class for Image Registration Methods
   
   This abstract class define the generic interface for a registration method.
   It takes as inputs two images with SetFixedImage() and SetMovingImage().

   The FixedImage is the target space whereas the MovingImage is the source space.

   The output of this ProcessObject is not usable, it does not correspond to any
   image... (to be fixed).
   The real goal of a registration method is to export a transform that fit in a
   specific way the MovingImage to the FixedImage.
   Hence the user should use GetTransform() after calling Update().
   
   Use Initialize() for internal consistency check (Usually check the presence
   of the inputs)

   As the real output of this ProcessObject is a Transform, neither InitialTransform,
   nor InitialTransformParameters (nor LastTransformParameters) should be used !! (to be removed)

   Subclasses should overwrite the GenerateData() method to apply a specific method.
   at the end of the GenerateData() method a transform should be set via SetTransform() method

   WriteTransform() method is a convinient method that attempt to write the internal
   transform in file. The transform should be supported by I/O factory in order use it.
   
   
   \ingroup RegistrationFilters
   \author Nicolas Toussaint, INRIA
   
*/
template <typename TImage>
class ITK_EXPORT RegistrationMethod : public ProcessObject 
{
public:
  /** Standard class typedefs. */
  typedef RegistrationMethod  Self;
  typedef ProcessObject  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(RegistrationMethod, ProcessObject);

  /**  Type of the Fixed image. */
  typedef          TImage                     ImageType;
  typedef typename ImageType::ConstPointer    ImageConstPointer;

  typedef double ParametersValueType;

  /**  Type of the Transform . */
  typedef  Transform<ParametersValueType, ImageType::ImageDimension, ImageType::ImageDimension> TransformType;
  typedef  typename TransformType::Pointer                  TransformPointerType;
  typedef  typename TransformType::ConstPointer             TransformConstPointerType;
  

  /** Type of the Transformation parameters This is the same type used to
   *  represent the search space of the optimization algorithm */
  typedef  typename TransformType::ParametersType    ParametersType;

  typedef Array2D<unsigned int> ScheduleType;
  
  /**Returns a raw pointer to this object. This is useful in cases where we use Self in functions that return 
  objects as covariant return types */
  Self* GetSelf()
  {  return this;  }

  /** Set/Get the name (description name) of this method */
  void SetName (const char* name)
  { m_Name = name; }
  const char* GetName() const
  { return m_Name; }
  
  /** Set/Get the Fixed image. */
  virtual void SetFixedImage( ImageType * fixedImage );
  itkGetObjectMacro( FixedImage, ImageType ); 
  
  /** Set/Get the Moving image. */
  virtual void SetMovingImage( ImageType * movingImage );
  itkGetObjectMacro( MovingImage, ImageType );
  
  /** Set/Get the Transfrom. */
  itkSetObjectMacro( Transform, TransformType );
  itkGetObjectMacro( Transform, TransformType );

  /** Set/Get the initial transformation parameters. */
  virtual void SetInitialTransformParameters( const ParametersType & param );
  itkGetConstReferenceMacro( InitialTransformParameters, ParametersType );

  /** Get the last transformation parameters visited by 
   * the optimizer. */
  itkGetConstReferenceMacro( LastTransformParameters, ParametersType );

  /** Initialize by setting the interconnects between the components. */
  virtual void Initialize();

  void SetInitialTransform (TransformConstPointerType transform);
  itkGetConstObjectMacro( InitialTransform, TransformType );
  
  /** Returns the transform resulting from the registration process  */
  virtual TransformPointerType GetOutput() const
  {
    return m_Transform;
  }

  virtual void WriteTransform (const char* filename);

  static int roundint(double a)
  {  
#ifdef WIN32
    double test = fabs ( a - double(int(a)) );
    int res = 0;
    if(a>0)
      (test>0.5)?(res=a+1):res=a;  
    else
      (test>0.5)?(res=a-1):res=a;
    
    return res;   
#else
    return (int)rint(a);
#endif // WIN32
  }

  
  virtual ScheduleType GetOptimumSchedule (unsigned int MinimumDimensionSize = 8, unsigned int DesiredNumberOfLevels = 3);

  
protected:
  RegistrationMethod();
  virtual ~RegistrationMethod() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Method invoked by the pipeline in order to trigger the computation of 
   * the registration. */
  virtual void  GenerateData ()
  {
    itkExceptionMacro("subclass should overwrite this method !");
  }

  /** Provides derived classes with the ability to set this private var */
  itkSetMacro( LastTransformParameters, ParametersType );

  typename ImageType::Pointer m_MovingImage;
  typename ImageType::Pointer m_FixedImage;
  
  TransformPointerType m_Transform;
  TransformConstPointerType m_InitialTransform;

  ParametersType m_InitialTransformParameters;
  ParametersType m_LastTransformParameters;

  const char* m_Name;
  
  
private:
  RegistrationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  

};


} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegistrationMethod.txx"
#endif
  




