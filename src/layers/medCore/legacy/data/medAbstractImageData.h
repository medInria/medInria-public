#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <typeinfo>
#include <vector>

#include <medAbstractData.h>

class MEDCORE_EXPORT medAbstractImageData: public medAbstractData
{
    Q_OBJECT

public:
    typedef std::type_info PixId;
    typedef std::vector < std::vector <double> > MatrixType;

    medAbstractImageData();
    medAbstractImageData(const medAbstractImageData& other);
    ~medAbstractImageData() override = default;

    void *image();

    virtual int   Dimension() const;
    virtual const PixId& PixelType() const;
    virtual MatrixType orientationMatrix();

    virtual int xDimension();
    virtual int yDimension();
    virtual int zDimension();
    virtual int tDimension();

    virtual int minRangeValue();
    virtual int maxRangeValue();

    virtual int scalarValueCount(int value);
    virtual int scalarValueMinCount();
    virtual int scalarValueMaxCount();

    virtual void deleteHistogram(){}

    static const char* PixelMeaningMetaData;
};

Q_DECLARE_METATYPE(medAbstractImageData)
Q_DECLARE_METATYPE(medAbstractImageData*)
