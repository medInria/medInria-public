/* dtkAbstractDataTypedImage4D.h --- 
 * 
 * Author: Theo Papadopoulo
 * Copyright (C) 2008 - Theo Papadopoulo, Inria.
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTDATATYPEDIMAGE4D_H
#define DTKABSTRACTDATATYPEDIMAGE4D_H

#include <dtkCore/dtkAbstractDataImage4D.h>

template <typename T>
class DTKCORE_EXPORT dtkAbstractDataTypedImage4D : public dtkAbstractDataImage4D {
public:

    dtkAbstractDataTypedImage4D(void);
   ~dtkAbstractDataTypedImage4D(void);

    const dtkAbstractDataImage::PixId& PixelType() const { return typeid(T); }
};

#endif
