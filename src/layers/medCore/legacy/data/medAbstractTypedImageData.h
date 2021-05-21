#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageData.h>

template <unsigned DIM,typename T>
class medAbstractTypedImageData: public medAbstractImageData {
public:

    virtual int                                Dimension() const { return DIM;       }
    virtual const medAbstractImageData::PixId& PixelType() const { return typeid(T); }

    virtual const QString PixelMeaning() const {
        if (hasMetaData(medAbstractImageData::PixelMeaningMetaData))
            return metadata(medAbstractImageData::PixelMeaningMetaData);
        return "";
    }
};


