/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkDataSHImageReaderBase.h>
#include <itkDataSHImagePluginExport.h>

class ITKDATASHIMAGEPLUGIN_EXPORT itkNrrdDataSHImageReader: public itkDataSHImageReaderBase
{
    Q_OBJECT
	
public:
    itkNrrdDataSHImageReader();
    virtual ~itkNrrdDataSHImageReader();
	
    virtual QString description() const;
    virtual QString identifier() const;
	
    static bool registered();	
};

dtkAbstractDataReader *createItkNrrdDataSHImageReader();
