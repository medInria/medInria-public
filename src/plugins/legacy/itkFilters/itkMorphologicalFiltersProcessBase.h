/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkFiltersProcessBase.h>

class itkMorphologicalFiltersProcessBasePrivate;

class ITKFILTERSPLUGIN_EXPORT itkMorphologicalFiltersProcessBase : public itkFiltersProcessBase
{
    Q_OBJECT

public:
    enum KernelShape
    {
        BallKernel,
        CrossKernel,
        BoxKernel,
    };

    itkMorphologicalFiltersProcessBase(itkMorphologicalFiltersProcessBase *parent = nullptr);
    itkMorphologicalFiltersProcessBase(const itkMorphologicalFiltersProcessBase& other);
    void setParameter(double data, int channel);
    void setParameter(int data);

public slots:
    int tryUpdate();

protected:
    template <class ImageType> void convertMmInPixels(ImageType *image);
    template <class ImageType> int updateProcess(medAbstractData *inputData);

private:
    itkMorphologicalFiltersProcessBasePrivate *d;
};

