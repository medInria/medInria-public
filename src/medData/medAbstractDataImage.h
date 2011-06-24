/* dtkAbstractDataImage.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Feb  2 09:08:09 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Jun 16 14:58:37 2011 (+0200)
 *           By: Julien Wintz
 *     Update #: 42
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTIMAGEDATA_H
#define DTKABSTRACTIMAGEDATA_H

#include <typeinfo>
#include <dtkCore/dtkAbstractData.h>

class DTKCORE_EXPORT dtkAbstractDataImage: public dtkAbstractData
{
public:
    typedef std::type_info PixId;

             dtkAbstractDataImage(void);
             dtkAbstractDataImage(const dtkAbstractDataImage& other);
    virtual ~dtkAbstractDataImage(void);

    void *image(void);

    virtual int   Dimension(void) const;
    virtual const PixId& PixelType(void) const;

    virtual int xDimension(void);
    virtual int yDimension(void);
    virtual int zDimension(void);
    virtual int tDimension(void);

    virtual int minRangeValue(void);
    virtual int maxRangeValue(void);

    virtual int scalarValueCount(int value);
    virtual int scalarValueMinCount(void);
    virtual int scalarValueMaxCount(void);

    static const char* PixelMeaningMetaData;
};

#endif
