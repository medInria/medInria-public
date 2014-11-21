/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkImageDataWriterBase.h>
#include <medItkImageExport.h>

class MEDITKIMAGEPLUGIN_EXPORT medItkImageDataNrrdWriter: public medItkImageDataWriterBase {
public:
    medItkImageDataNrrdWriter();
    virtual ~medItkImageDataNrrdWriter();

    virtual QString identifier()  const;
    virtual QString description() const;
    virtual QStringList handled() const;
    virtual QStringList supportedFileExtensions() const;

    static bool registered();	

    static dtkAbstractDataWriter * create();
};

