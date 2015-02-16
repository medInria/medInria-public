/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkSHImageDataWriterBase.h>
#include <medItkSHImageExport.h>

class MEDITKSHIMAGEPLUGIN_EXPORT medItkSHImageMetaDataWriter : public medItkSHImageDataWriterBase
{
    Q_OBJECT

public:
    medItkSHImageMetaDataWriter();
    virtual ~medItkSHImageMetaDataWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();

    static dtkAbstractDataWriter* create();
};
