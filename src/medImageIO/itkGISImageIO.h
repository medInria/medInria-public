/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include <itkImageIOBase.h>

#include <medImageIOExport.h>

namespace itk //TODO no namespace please ! (and no class name prifixed with other librarie pefix)
{

  class MEDIMAGEIO_EXPORT GISImageIO : public ImageIOBase
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



