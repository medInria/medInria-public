/* medAbstractDataImage.h --- 
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

#ifndef MEDABSTRACTIMAGEDATA_H
#define MEDABSTRACTIMAGEDATA_H

#include <typeinfo>
#include "medCoreExport.h"
#include <medAbstractData.h>

class MEDCORE_EXPORT medAbstractDataImage: public medAbstractData
{
    Q_OBJECT

public:
    typedef std::type_info PixId;

             medAbstractDataImage();
             medAbstractDataImage(const medAbstractDataImage& other);
    virtual ~medAbstractDataImage();

    void *image();

    virtual int   Dimension() const;
    virtual const PixId& PixelType() const;

    virtual int xDimension();
    virtual int yDimension();
    virtual int zDimension();
    virtual int tDimension();

    virtual int minRangeValue();
    virtual int maxRangeValue();

    virtual int scalarValueCount(int value);
    virtual int scalarValueMinCount();
    virtual int scalarValueMaxCount();

    static const char* PixelMeaningMetaData;
};

#endif
