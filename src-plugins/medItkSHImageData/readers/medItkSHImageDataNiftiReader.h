/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkSHImageDataReaderBase.h>
#include <medItkSHImageExport.h>

class MEDITKSHIMAGEPLUGIN_EXPORT medItkSHImageDataNiftiReader : public medItkSHImageDataReaderBase
{
    Q_OBJECT
	
public:
    medItkSHImageDataNiftiReader();
    virtual ~medItkSHImageDataNiftiReader();
	
    virtual QString description() const;
    virtual QString identifier() const;
	
    static bool registered();	
};

dtkAbstractDataReader *createItkNiftiDataSHImageReader();
