#include "itkMultiThreadedImageIOBase.h"

namespace itk
{

  MultiThreadedImageIOBase::MultiThreadedImageIOBase()
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
    m_FileNames = filenames;
    if( filenames.size() )
      this->SetFileName ( filenames[0].c_str() );
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
  

  int MultiThreadedImageIOBase::SplitRequestedRegion (int id, int total, RegionType& region)
  {
    int fileCount       = (int)( m_FileNames.size() );
    int threadFileCount = (int)::ceil( fileCount/(double)total );
    
    RegionType::IndexType start;
    start[0] = id * threadFileCount;
    RegionType::SizeType length;
    length[0] = threadFileCount;
    
    int maxThreadInUse = (int)::ceil(fileCount/(double)threadFileCount) - 1;
    
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
