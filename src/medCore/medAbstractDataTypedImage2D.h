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

#ifndef MEDABSTRACTDATATYPEDIMAGE2D_H
#define MEDABSTRACTDATATYPEDIMAGE2D_H

#include "medCoreExport.h"
#include <medAbstractDataImage2D.h>

template <typename T>
class MEDCORE_EXPORT medAbstractDataTypedImage2D : public medAbstractDataImage2D {
public:

    medAbstractDataTypedImage2D(void);
   ~medAbstractDataTypedImage2D(void);

    const medAbstractDataImage::PixId& PixelType() const { return typeid(T); }
};

#endif

