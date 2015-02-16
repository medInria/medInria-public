/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medTtkTensorImageDataReaderBase.h>
#include <medTtkTensorImageExport.h>

class medTtkTensorImagePlugin_EXPORT medTtkTensorImageDataNrrdReader: public medTtkTensorImageDataReaderBase {
    Q_OBJECT

public:

    medTtkTensorImageDataNrrdReader();
    virtual ~medTtkTensorImageDataNrrdReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();

private:

    static const char ID[];
};

dtkAbstractDataReader *createmedTtkTensorImageDataNrrdReader();


