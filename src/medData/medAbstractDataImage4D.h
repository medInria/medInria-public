/* dtkAbstractDataImage4D.h --- 
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

#ifndef DTKABSTRACTDATAIMAGE4D_H
#define DTKABSTRACTDATAIMAGE4D_H

#include <dtkCore/dtkAbstractDataImage.h>

class dtkAbstractDataImage4DPrivate;

class dtkAbstractDataImage4D : public dtkAbstractDataImage
{
    Q_OBJECT

public:
    dtkAbstractDataImage4D(void);
   ~dtkAbstractDataImage4D(void);

    virtual int Dimension() const;

private:
   dtkAbstractDataImage4DPrivate *d;
};

#endif
