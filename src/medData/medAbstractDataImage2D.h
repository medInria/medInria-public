/* dtkAbstractDataImage2D.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Feb 11 12:38:11 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Jul 31 23:15:56 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTDATAIMAGE2D_H
#define DTKABSTRACTDATAIMAGE2D_H

#include <dtkCore/dtkAbstractDataImage.h>

class dtkAbstractDataImage2DPrivate;

class dtkAbstractDataImage2D : public dtkAbstractDataImage
{
    Q_OBJECT

public:

    dtkAbstractDataImage2D(void);
   ~dtkAbstractDataImage2D(void);

   virtual int Dimension() const;

private:
   dtkAbstractDataImage2DPrivate *d;
};

#endif
