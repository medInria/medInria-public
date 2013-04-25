/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkDataImageBase/itkDataImageWriterBase.h>
#include <itkDataImageWriterPluginExport.h>

class ITKDATAIMAGEWRITERPLUGIN_EXPORT itkGISDataImageWriter: public itkDataImageWriterBase {
public:

    itkGISDataImageWriter();
    virtual ~itkGISDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled ();

    static bool registered();   

private:
    
    static const char ID[];
};

dtkAbstractDataWriter *createItkGISDataImageWriter();


