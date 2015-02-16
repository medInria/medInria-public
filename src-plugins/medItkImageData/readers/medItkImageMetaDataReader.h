/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkImageDataReaderBase.h>
#include <medItkImageExport.h>

class MEDITKIMAGEPLUGIN_EXPORT medItkImageMetaDataReader: public medItkImageDataReaderBase
{
public:
    medItkImageMetaDataReader();
    virtual ~medItkImageMetaDataReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();

private:

    static const char ID[];
};

dtkAbstractDataReader *createmedItkImageMetaDataReader();


