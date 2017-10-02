/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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

    itkFiltersShrinkProcess(itkFiltersShrinkProcess * parent = 0);
    itkFiltersShrinkProcess(const itkFiltersShrinkProcess& other);
    virtual ~itkFiltersShrinkProcess(void);
    static bool registered ( void );

    virtual QString description(void) const;
    
public slots:

    void setParameter ( double  data, int channel );
    int tryUpdate();

protected:
    template <class PixelType> int updateProcess();

private:
    itkFiltersShrinkProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersShrinkProcess(void);


