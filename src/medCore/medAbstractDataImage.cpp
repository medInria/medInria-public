/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataImage.h>

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
    qDebug() << "constructing medAbstractDataImage";
}

medAbstractDataImage::medAbstractDataImage(const medAbstractDataImage& other): medAbstractData(other)
{
}

medAbstractDataImage::~medAbstractDataImage(void)
{
    qDebug() << "deleting medAbstractDataImage";
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
