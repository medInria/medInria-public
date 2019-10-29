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

class itkFiltersDivideProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersDivideProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    static const double defaultDivideFactor;

    itkFiltersDivideProcess(itkFiltersDivideProcess *parent = nullptr);
    itkFiltersDivideProcess(const itkFiltersDivideProcess& other);
    virtual ~itkFiltersDivideProcess();
    static bool registered();
    virtual QString description() const;
    
public slots:
    void setParameter(double data);
    int tryUpdate();

protected:
    template <class ImageType> int updateProcess(medAbstractData *inputData);

private:
    itkFiltersDivideProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersDivideProcess();
