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

class itkFiltersShrinkProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersShrinkProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    static const unsigned int defaultShrinkFactors[3];

    itkFiltersShrinkProcess(itkFiltersShrinkProcess *parent = nullptr);
    itkFiltersShrinkProcess(const itkFiltersShrinkProcess& other);
    virtual ~itkFiltersShrinkProcess();
    static bool registered ();
    virtual QString description() const;
    
public slots:
    void setParameter(int data, int channel);
    int tryUpdate();

protected:
    template <class ImageType> int updateProcess(medAbstractData *inputData);

private:
    itkFiltersShrinkProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersShrinkProcess();
