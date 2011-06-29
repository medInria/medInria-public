/* medAbstractDataImage.cpp --- 
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

#include <medCore/medAbstractDataImage.h>

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medAbstractDataImagePrivate
// /////////////////////////////////////////////////////////////////

//  Unneeded in this case.

// /////////////////////////////////////////////////////////////////
// medAbstractDataImage
// /////////////////////////////////////////////////////////////////

const char* medAbstractDataImage::PixelMeaningMetaData = "PixelMeaning";

medAbstractDataImage::medAbstractDataImage(void) : medAbstractData()
{

}

medAbstractDataImage::medAbstractDataImage(const medAbstractDataImage& other): medAbstractData(other)
{

}

medAbstractDataImage::~medAbstractDataImage(void)
{
}

void *medAbstractDataImage::image(void)
{
    return this->data();
}

int medAbstractDataImage::Dimension() const
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

const medAbstractDataImage::PixId& medAbstractDataImage::PixelType() const
{
    DTK_DEFAULT_IMPLEMENTATION;

    return typeid(void);
}

int medAbstractDataImage::xDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractDataImage::yDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractDataImage::zDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractDataImage::tDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;
	
    return 0;
}

int medAbstractDataImage::minRangeValue(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractDataImage::maxRangeValue(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractDataImage::scalarValueCount(int value)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(value);

    return 0;
}

int medAbstractDataImage::scalarValueMinCount(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractDataImage::scalarValueMaxCount(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}
