/* medAbstractDataTypedImage2D.h --- 
 * 
 * Author: Theo Papadopoulo
 * Copyright (C) 2011 - Theo Papadopoulo, Inria.
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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


