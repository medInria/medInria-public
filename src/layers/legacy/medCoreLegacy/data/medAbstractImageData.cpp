/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageData.h>

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medAbstractImageData
// /////////////////////////////////////////////////////////////////

const char* medAbstractImageData::PixelMeaningMetaData = "PixelMeaning";

medAbstractImageData::medAbstractImageData(void) : medAbstractData()
{
}

medAbstractImageData::medAbstractImageData(const medAbstractImageData& other): medAbstractData(other)
{
}

void *medAbstractImageData::image(void)
{
    return this->data();
}

int medAbstractImageData::Dimension() const
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

const medAbstractImageData::PixId& medAbstractImageData::PixelType() const
{
    DTK_DEFAULT_IMPLEMENTATION;

    return typeid(void);
}

medAbstractImageData::MatrixType medAbstractImageData::orientationMatrix()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return MatrixType();
}

int medAbstractImageData::xDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractImageData::yDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractImageData::zDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractImageData::tDimension(void)
{
    DTK_DEFAULT_IMPLEMENTATION;
	
    return 0;
}

int medAbstractImageData::minRangeValue(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractImageData::maxRangeValue(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractImageData::scalarValueCount(int value)
{
    DTK_DEFAULT_IMPLEMENTATION;
    Q_UNUSED(value);

    return 0;
}

int medAbstractImageData::scalarValueMinCount(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

int medAbstractImageData::scalarValueMaxCount(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}
