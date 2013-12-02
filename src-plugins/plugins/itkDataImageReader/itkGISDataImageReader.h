/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkDataImageBase/itkDataImageReaderBase.h>
#include "itkDataImageReaderPluginExport.h"


class ITKDATAIMAGEREADERPLUGIN_EXPORT itkGISDataImageReader: public itkDataImageReaderBase {

public:

    itkGISDataImageReader();
    virtual ~itkGISDataImageReader();
 
    virtual QString identifier()  const;
    virtual QString description() const;
    
    QStringList handled() const;

    static QStringList s_handled();
    
    static bool registered();
    
private:    
    
    static const char ID[];
        
};

dtkAbstractDataReader *createItkGISDataImageReader();


