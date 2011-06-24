/* dtkAbstractDataTypedImage2D.h --- 
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

#ifndef DTKABSTRACTDATATYPEDIMAGE2D_H
#define DTKABSTRACTDATATYPEDIMAGE2D_H

#include <dtkCore/dtkAbstractDataImage2D.h>

template <typename T>
class DTKCORE_EXPORT dtkAbstractDataTypedImage2D : public dtkAbstractDataImage2D {
public:

    dtkAbstractDataTypedImage2D(void);
   ~dtkAbstractDataTypedImage2D(void);

    const dtkAbstractDataImage::PixId& PixelType() const { return typeid(T); }
};

#endif
