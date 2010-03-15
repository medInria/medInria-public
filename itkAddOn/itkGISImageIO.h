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

#ifndef _itk_GISImageIO_h_
#define _itk_GISImageIO_h_

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkImageIOBase.h"

namespace itk
{

  class ITK_EXPORT GISImageIO : public ImageIOBase
  {

  public:
    typedef GISImageIO         Self;
    typedef ImageIOBase        Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro (Self);
    itkTypeMacro(GISImageIO, ImageIOBase);

    /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();
  
  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can write the
   * file specified. */
  virtual bool CanWriteFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();
  
  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void* buffer);

  GISImageIO();
  ~GISImageIO();
  void PrintSelf(std::ostream& os, Indent indent) const {};

  private:
  GISImageIO(const Self&);
  void operator=(const Self&);

  void SwapBytesIfNecessary(void* buffer, unsigned long numberOfPixels);

  bool m_IsBinary;
  
  };


} // end of namespace


#endif
