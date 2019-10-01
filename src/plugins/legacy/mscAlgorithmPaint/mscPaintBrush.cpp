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

#include <mscPaintBrush.h>

namespace msc
{

class mscPaintBrushPrivate
{
public:
    Mask2dType::Pointer slice;
    bool isMaster; //true when the ROI is new or has been modified (for interpolation)
    int label;
};

mscPaintBrush::mscPaintBrush(Mask2dType::Pointer slice, int id, bool isMaster, int label, medAbstractRoi* parent)
    : medAbstractRoi(parent), d(new mscPaintBrushPrivate)
{
    setIdSlice(id);
    d->slice = slice;
    setMasterRoi(isMaster);
    d->label = label;
}

mscPaintBrush::~mscPaintBrush()
{
    delete d;
    d = nullptr;
}

Mask2dType::Pointer mscPaintBrush::getSlice()
{
    return d->slice;
}

void mscPaintBrush::setRightColor()
{
}

void mscPaintBrush::Off()
{

}

void mscPaintBrush::On()
{

}

bool mscPaintBrush::isVisible()
{
    return false;
}

void mscPaintBrush::forceInvisibilityOn()
{

}

void mscPaintBrush::forceInvisibilityOff()
{

}

QString mscPaintBrush::info()
{
    return QString();
}

QString mscPaintBrush::type()
{
    return QString();
}

void mscPaintBrush::computeRoiStatistics()
{

}

bool mscPaintBrush::canRedo()
{
    return false;
}

bool mscPaintBrush::canUndo()
{
    return false;
}

void mscPaintBrush::undo()
{

}

void mscPaintBrush::redo()
{

}

void mscPaintBrush::saveState()
{

}

bool mscPaintBrush::copyROI(medAbstractView *view)
{
    return false;
}

medAbstractRoi *mscPaintBrush::getCopy(medAbstractView *view)
{
    return nullptr;
}

QList<medAbstractRoi *> *mscPaintBrush::interpolate(medAbstractRoi *roi)
{
    return nullptr;
}

int mscPaintBrush::getLabel()
{
    return d->label;
}

}
