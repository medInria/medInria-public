/* medAbstractDataImage3D.h --- 
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

#ifndef MEDABSTRACTDATATYPEDIMAGE3D_H
#define MEDABSTRACTDATATYPEDIMAGE3D_H

#include "medCoreExport.h"
#include <medCore/medAbstractDataImage3D.h>

template <typename T>
class MEDCORE_EXPORT medAbstractDataTypedImage3D : public medAbstractDataImage3D {
public:

    medAbstractDataTypedImage3D(void);
   ~medAbstractDataTypedImage3D(void);

    const medAbstractDataImage::PixId& PixelType() const { return typeid(T); }
};

#endif

