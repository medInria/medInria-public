/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medTtkTensorImageDataWriterBase.h>

#include <medTtkTensorImageExport.h>

class medTtkTensorImagePlugin_EXPORT medTtkTensorImageDataNiftiWriter : public medTtkTensorImageDataWriterBase
{
    Q_OBJECT

public:
    medTtkTensorImageDataNiftiWriter();
    virtual ~medTtkTensorImageDataNiftiWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();

    static dtkAbstractDataWriter* create();
};
