/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medTestExport.h>

#include <medAbstractImageData.h>

class medQtDataImagePrivate;

/** A concrete implementation of a dtkAbstractDataImage for use in testing. */
class MEDTEST_EXPORT medQtDataImage : public medAbstractImageData
{
    Q_OBJECT
    MED_DATA_INTERFACE("Qt Image Data", "Test Image Data bast on QImage")
public:
    medQtDataImage();
    virtual ~medQtDataImage();
    static bool registered();

    // Implement dtkAbstractData
    virtual void setData(void* data);
    virtual void setData(void* data, int channel);
    virtual void *output();
    virtual void *data();
    virtual void *data(int channel);

    // Implement dtkAbstractDataImage
    void *image();

    virtual int   Dimension() const;
    virtual const PixId& PixelType() const;

    virtual int xDimension();
    virtual int yDimension();
    virtual int zDimension();
    virtual int tDimension();

    virtual qreal xSpacing() const;
    virtual qreal ySpacing() const;
    virtual qreal zSpacing() const;
    virtual qreal tSpacing() const;

    virtual int minRangeValue();
    virtual int maxRangeValue();

    virtual int scalarValueCount(int value);
    virtual int scalarValueMinCount();
    virtual int scalarValueMaxCount();

protected slots:
    virtual QImage generateThumbnailInGuiThread(QSize size);

private:
    medQtDataImagePrivate * d;
};
