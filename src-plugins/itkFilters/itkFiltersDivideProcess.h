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

class itkFiltersDivideProcessPrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersDivideProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    static const double defaultDivideFactor;

    itkFiltersDivideProcess(itkFiltersDivideProcess * parent = 0);
    itkFiltersDivideProcess(const itkFiltersDivideProcess& other);
    virtual ~itkFiltersDivideProcess(void);
    static bool registered ( void );

    virtual QString description(void) const;
    
public slots:

    void setParameter ( double  data, int channel );
    int tryUpdate();

protected:
    template <class PixelType> int updateProcess();

private:
    itkFiltersDivideProcessPrivate *d;
};

dtkAbstractProcess * createitkFiltersDivideProcess(void);


