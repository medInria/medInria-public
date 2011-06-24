/* dtkAbstractDataImage.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Feb  2 09:08:09 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Jan 27 15:36:59 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 61
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <dtkCore/dtkAbstractDataImage.h>

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// dtkAbstractDataImagePrivate
// /////////////////////////////////////////////////////////////////

//  Unneeded in this case.

// /////////////////////////////////////////////////////////////////
// dtkAbstractDataImage
// /////////////////////////////////////////////////////////////////

const char* dtkAbstractDataImage::PixelMeaningMetaData = "PixelMeaning";

dtkAbstractDataImage::dtkAbstractDataImage(void) : dtkAbstractData()
{

}

dtkAbstractDataImage::dtkAbstractDataImage(const dtkAbstractDataImage& other): dtkAbstractData(other)
{

}

dtkAbstractDataImage::~dtkAbstractDataImage(void)
{
}

void *dtkAbstractDataImage::image(void)
{
    return this->data();
}

int dtkAbstractDataImage::Dimension() const
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

const dtkAbstractDataImage::PixId& dtkAbstractDataImage::PixelType() const
{
    DTK_DEFAULT_IMPLEMENTATION;

    return typeid(void);
}

int dtkAbstractDataImage::xDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int dtkAbstractDataImage::yDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int dtkAbstractDataImage::zDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int dtkAbstractDataImage::tDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;
	
    return 0;
}

int dtkAbstractDataImage::minRangeValue(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int dtkAbstractDataImage::maxRangeValue(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int dtkAbstractDataImage::scalarValueCount(int value)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(value);

    return 0;
}

int dtkAbstractDataImage::scalarValueMinCount(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int dtkAbstractDataImage::scalarValueMaxCount(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}
