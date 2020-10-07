/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataImageReaderCommand.h>

#include <itkImageIOBase.h>

#include <dtkCoreSupport/dtkAbstractDataReader.h>

namespace itk
{
    void DataImageReaderCommand::Execute (Object *caller, const EventObject &event)
    {       
        ImageIOBase *po = dynamic_cast<ImageIOBase *>(caller);
        
        if(po && typeid(event) == typeid(itk::ProgressEvent))
        {
            if (m_Reader)
            {
                m_Reader->setProgress((int)(po->GetProgress()*100.0));
            }
        }
    }
        
    void DataImageReaderCommand::Execute (const Object *caller, const EventObject &event)
    {
        ImageIOBase *po = dynamic_cast<ImageIOBase *>(const_cast<Object *>(caller) );
        
        if(po && typeid(event) == typeid ( itk::ProgressEvent  )  )
        {
            if (m_Reader)
            {
                m_Reader->setProgress ( (int)(po->GetProgress()*100.0) );
            }
        }
    }
}
