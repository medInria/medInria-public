/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include <medItkImageDataReaderCommand.h>

#include <itkImageIOBase.h>

#include <dtkCore/dtkAbstractDataReader.h>

namespace itk
{
    void DataImageReaderCommand::Execute (Object *caller, const EventObject &event)
    {
        ImageIOBase *po = dynamic_cast<ImageIOBase *>(caller);

        if (!po)
            return;

        if(typeid(event) == typeid(itk::ProgressEvent))
        {
            if (m_Reader)
                m_Reader->setProgress((int)(po->GetProgress()*100.0));
        }
    }

    void DataImageReaderCommand::Execute (const Object *caller, const EventObject &event)
    {
        ImageIOBase *po = dynamic_cast<ImageIOBase *>(const_cast<Object *>(caller) );
        if (! po)
            return;

        if( typeid(event) == typeid ( itk::ProgressEvent  )  )
        {
            if (m_Reader)
                m_Reader->setProgress ( (int)(po->GetProgress()*100.0) );
        }
    }
}
