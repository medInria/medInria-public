/* medAbstractDataImage3D.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Feb 11 12:38:11 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Jul 31 23:18:21 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDABSTRACTDATAIMAGE3D_H
#define MEDABSTRACTDATAIMAGE3D_H

#include "medCoreExport.h"
#include <medAbstractDataImage.h>

class medAbstractDataImage3DPrivate;

class medAbstractDataImage3D : public medAbstractDataImage
{
    Q_OBJECT

public:
    medAbstractDataImage3D(void);
   ~medAbstractDataImage3D(void);

    virtual int Dimension() const;

private:
   medAbstractDataImage3DPrivate *d;
};

#endif
