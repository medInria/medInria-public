/* dtkAbstractDataImage3D.h --- 
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

#ifndef DTKABSTRACTDATAIMAGE3D_H
#define DTKABSTRACTDATAIMAGE3D_H

#include <dtkCore/dtkAbstractDataImage.h>

class dtkAbstractDataImage3DPrivate;

class dtkAbstractDataImage3D : public dtkAbstractDataImage
{
    Q_OBJECT

public:
    dtkAbstractDataImage3D(void);
   ~dtkAbstractDataImage3D(void);

    virtual int Dimension() const;

private:
   dtkAbstractDataImage3DPrivate *d;
};

#endif
