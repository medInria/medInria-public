/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medCoreExport.h"
#include <medAbstractDataImage.h>

template <unsigned DIM,typename T>
class medAbstractDataTypedImage: public medAbstractDataImage {
public:

    medAbstractDataTypedImage() { qDebug() << "constructing medAbstractDataTypeImage";}
    virtual ~medAbstractDataTypedImage() { qDebug() << "deleting medAbstractDataTypeImage";}

    int                                Dimension() const { return DIM;       }
    const medAbstractDataImage::PixId& PixelType() const { return typeid(T); }

    const QString PixelMeaning() const {
        if (hasMetaData(medAbstractDataImage::PixelMeaningMetaData))
            return metadata(medAbstractDataImage::PixelMeaningMetaData);
        return "";
    }
};


