/* dtkAbstractDataImage3D.h --- 
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

#ifndef DTKABSTRACTDATATYPEDIMAGE3D_H
#define DTKABSTRACTDATATYPEDIMAGE3D_H

#include <dtkCore/dtkAbstractDataImage3D.h>

template <typename T>
class DTKCORE_EXPORT dtkAbstractDataTypedImage3D : public dtkAbstractDataImage3D {
public:

    dtkAbstractDataTypedImage3D(void);
   ~dtkAbstractDataTypedImage3D(void);

    const dtkAbstractDataImage::PixId& PixelType() const { return typeid(T); }
};

#endif
