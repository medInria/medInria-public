#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersProcessBase.h>

class itkFiltersComponentSizeThresholdProcessPrivate;

namespace itk
{
template <typename PixelType, unsigned int Dimension> class Image;
}

class ITKFILTERSPLUGIN_EXPORT itkFiltersComponentSizeThresholdProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    typedef itk::Image<unsigned short, 3> OutputImageType;

    static const int defaultMinimumSize;
    static const bool defaultBinarize;

    itkFiltersComponentSizeThresholdProcess(itkFiltersComponentSizeThresholdProcess *parent = nullptr);
    itkFiltersComponentSizeThresholdProcess(const itkFiltersComponentSizeThresholdProcess& other);
    virtual ~itkFiltersComponentSizeThresholdProcess();
    static bool registered();
    virtual QString description() const;
    
public slots:
    void setParameter(int data, int channel);
    int tryUpdate();

protected:
    template <class InputImageType> dtkSmartPointer<medAbstractData> castToOutputType(medAbstractData *inputData);
    template <class InputImageType> int updateProcess(medAbstractData *inputData);

private:
    itkFiltersComponentSizeThresholdProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersComponentSizeThresholdProcess();
