/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: CodeCopyright.txt 1182 2009-06-30 09:19:05Z acanale $
Language:  C++
Author:    $Author: $
Date:      $Date: 2009-06-30 11:19:05 +0200 (mar, 30 jun 2009) $
Version:   $Revision: 1182 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#pragma once

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include "itkObjectFactoryBase.h"
#include "itkImageIOBase.h"

namespace itk
{
/** \class GISImageIOFactory
 * \brief Create instances of GISImageIO objects using an object factory.
 */
class ITK_EXPORT GISImageIOFactory : public ObjectFactoryBase
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


