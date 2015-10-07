/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkImageIOBase.h>
#include <itkMultiThreader.h>
#include <itkImageRegion.h>

#include <medImageIOExport.h>

namespace itk //TODO no namespace please ! (and no class name prifixed with other librarie pefix)
{

  class MEDIMAGEIO_EXPORT MultiThreadedImageIOBase: public ImageIOBase
  {
    
  public:
    typedef MultiThreadedImageIOBase Self;
    typedef ImageIOBase              Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro (MultiThreadedImageIOBase, ImageIOBase);


    typedef std::vector<std::string> FileNameVectorType;
    typedef ImageRegion<1>           RegionType;

    
    itkGetObjectMacro (MultiThreader, MultiThreader);


    /**
       Set/Get the number of threads for this reader.
     */
    itkSetMacro (NumberOfThreads, int);
    itkGetMacro (NumberOfThreads, int);


    virtual void AddFileName (const char* filename);


    virtual void SetFileNames (FileNameVectorType filenames);

    
    virtual FileNameVectorType GetFileNames() const
    { return m_FileNames; }

    
    virtual void Read(void* buffer);

    
  protected:
    MultiThreadedImageIOBase();
    ~MultiThreadedImageIOBase(){};

    virtual void ThreadedRead (void* buffer, RegionType region, int threadId){};

    static ITK_THREAD_RETURN_TYPE ThreaderCallback( void *arg );
    
    struct ThreadStruct
    {
      Pointer Reader;
      void*   Buffer;
    };

    virtual unsigned int SplitRequestedRegion (unsigned int id, unsigned int total, RegionType& region);
    
  private:
    MultiThreadedImageIOBase (const Self&);
    void operator=(const Self&);


    FileNameVectorType m_FileNames;

    
    MultiThreader::Pointer m_MultiThreader;
    int                    m_NumberOfThreads;
    
  };

  
  itkEventMacro (SliceReadEvent, AnyEvent);
  

} // end of namespace



