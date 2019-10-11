/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkAddImageFilter.h>
#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkRGBPixel.h>
#include <itkScalarToRGBColormapImageFilter.h>

#include <medPaintBrush.h>

namespace med
{

class medPaintBrushPrivate
{
public:
    Mask2dType::Pointer slice;
    bool isMaster; //true when the ROI is new or has been modified (for interpolation)
    int label;
};

medPaintBrush::medPaintBrush(Mask2dType::Pointer slice, int id, bool isMaster, int label, medAbstractRoi* parent)
    : medAbstractRoi(parent), d(new medPaintBrushPrivate)
{
    setIdSlice(id);
    d->slice = slice;
    setMasterRoi(isMaster);
    d->label = label;
}

medPaintBrush::~medPaintBrush()
{
    delete d;
    d = nullptr;
}

Mask2dType::Pointer medPaintBrush::getSlice()
{
    return d->slice;
}

void medPaintBrush::setRightColor()
{
}

void medPaintBrush::Off()
{

}

void medPaintBrush::On()
{

}

bool medPaintBrush::isVisible()
{
    return false;
}

void medPaintBrush::forceInvisibilityOn()
{

}

void medPaintBrush::forceInvisibilityOff()
{

}

QString medPaintBrush::info()
{
    return QString();
}

QString medPaintBrush::type()
{
    return QString();
}

void medPaintBrush::computeRoiStatistics()
{

}

bool medPaintBrush::canRedo()
{
    return false;
}

bool medPaintBrush::canUndo()
{
    return false;
}

void medPaintBrush::undo()
{

}

void medPaintBrush::redo()
{

}

void medPaintBrush::saveState()
{

}

bool medPaintBrush::copyROI(medAbstractView *view)
{
    return false;
}

medAbstractRoi *medPaintBrush::getCopy(medAbstractView *view)
{
    return nullptr;
}

QList<medAbstractRoi *> *medPaintBrush::interpolate(medAbstractRoi *roi)
{
    return nullptr;
}

int medPaintBrush::getLabel()
{
    return d->label;
}

}
