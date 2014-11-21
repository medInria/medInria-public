/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkMultiThreadedImageIOBase.h>

#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>

namespace itk
{

  MultiThreadedImageIOBase::MultiThreadedImageIOBase() :
    m_NumberOfThreads(0)
  {
    m_MultiThreader = MultiThreader::New();
    this->SetNumberOfThreads ( m_MultiThreader->GetNumberOfThreads() );
  }


  void MultiThreadedImageIOBase::AddFileName (const char* filename)
  {
    m_FileNames.push_back ( filename );
    this->SetFileName ( filename );
  }


  void MultiThreadedImageIOBase::SetFileNames (FileNameVectorType filenames)
  {
    unsigned int fileCount = filenames.size();
    if( fileCount ) {

      this->SetFileName ( filenames[0].c_str() ); // tells the reader not to exit

      if( fileCount==1 ) { // special case when only one file is set
    // it can be a directory - we scan it for files

    if( itksys::SystemTools::FileIsDirectory ( filenames[0].c_str() ) ) {

      m_FileNames.clear();
      itksys::Directory directory;
      directory.Load( filenames[0].c_str() );
      for( unsigned long i=0; i<directory.GetNumberOfFiles(); i++ )
      {
        std::string name = directory.GetPath();
        name += directory.GetFile (i);
        if( this->CanReadFile ( name.c_str() ) ) {
          m_FileNames.push_back ( name );
        }
      }
    }
    else
      m_FileNames = filenames;
      }
      else
    m_FileNames = filenames;
    }
  }


  void MultiThreadedImageIOBase::Read (void* buffer)
  {
    // Set up the multithreaded processing
    ThreadStruct str;
    str.Reader = this;
    str.Buffer = buffer;

    this->GetMultiThreader()->SetNumberOfThreads( this->GetNumberOfThreads() );
    this->GetMultiThreader()->SetSingleMethod   ( this->ThreaderCallback, &str );

    itkDebugMacro (<< "Executing with " << this->GetMultiThreader()->GetNumberOfThreads() << " threads.\n");
    // multithread the execution
    this->GetMultiThreader()->SingleMethodExecute();
  }


  unsigned int MultiThreadedImageIOBase::SplitRequestedRegion (unsigned int id, unsigned int total, RegionType& region)
  {
    int fileCount       = (int)( m_FileNames.size() );
    int threadFileCount = (int)::ceil( fileCount/(double)total );

    RegionType::IndexType start;
    start[0] = id * threadFileCount;
    RegionType::SizeType length;
    length[0] = threadFileCount;

    unsigned int maxThreadInUse = (unsigned int)::ceil(fileCount/(double)threadFileCount) - 1;

    if( id == maxThreadInUse )
      length[0] = fileCount - start[0];

    region.SetIndex (start);
    region.SetSize (length);

    return maxThreadInUse+1;
  }


  ITK_THREAD_RETURN_TYPE MultiThreadedImageIOBase::ThreaderCallback( void *arg )
  {
    ThreadStruct *str;
    int total, threadId, threadCount;

    threadId    = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
    threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

    str = (ThreadStruct *)(((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

    RegionType region;
    total = str->Reader->SplitRequestedRegion (threadId, threadCount, region);

    if ( threadId < total )
    {
      str->Reader->ThreadedRead(str->Buffer, region, threadId);
    }
    // else
    //   {
    //   otherwise don't use this thread. Sometimes the threads dont
    //   break up very well and it is just as efficient to leave a
    //   few threads idle.
    //   }

    return ITK_THREAD_RETURN_VALUE;
  }


} // end of namespace
