/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include <itkObjectFactoryBase.h>
#include <itkImageIOBase.h>

#include <medImageIOExport.h>

namespace itk //TODO no namespace please ! (and no class name prifixed with other librarie pefix)
{
/** \class GISImageIOFactory
 * \brief Create instances of GISImageIO objects using an object factory.
 */
class MEDIMAGEIO_EXPORT GISImageIOFactory : public ObjectFactoryBase
{
public:  
  /** Standard class typedefs. */
  typedef GISImageIOFactory        Self;
  typedef ObjectFactoryBase        Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Class methods used to interface with the registered factories. */
  virtual const char* GetITKSourceVersion() const;
  virtual const char* GetDescription() const;
  
  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GISImageIOFactory, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory()
    {
    GISImageIOFactory::Pointer GISFactory = GISImageIOFactory::New();
    ObjectFactoryBase::RegisterFactory(GISFactory);
    }

protected:
  GISImageIOFactory();
  ~GISImageIOFactory();

private:
  GISImageIOFactory(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};
  
  
} // end namespace itk


