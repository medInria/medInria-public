/* medAbstractDataImage.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sat Oct  3 18:28:59 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:17:53 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDABSTRACTIMAGEDATA_H
#define MEDABSTRACTIMAGEDATA_H

#include "medCoreExport.h"

#include <dtkCore/dtkAbstractDataImage.h>

class medAbstractDataImagePrivate;

class MEDCORE_EXPORT medAbstractDataImage : public dtkAbstractDataImage
{
    Q_OBJECT

public:
             medAbstractDataImage(void);
             medAbstractDataImage(const medAbstractDataImage& other);
    virtual ~medAbstractDataImage(void);

private:
    medAbstractDataImagePrivate *d;
};

#endif
