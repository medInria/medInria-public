/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkFiltersProcessBase.h>
#include <medAbstractData.h>

#include <medItkFiltersPluginExport.h>

template <template<typename, typename> class Prototype> class medItkFiltersGaussianProcessPrivate;
class medAbstractData;
namespace itk
{
template <class T, class X>
class SmoothingRecursiveGaussianImageFilter;
}

class medItkFiltersPlugin_EXPORT medItkFiltersGaussianProcess : public medItkFiltersProcessBase
{
    Q_OBJECT
    
public:
    medItkFiltersGaussianProcess(medItkFiltersGaussianProcess * parent = 0);
    virtual ~medItkFiltersGaussianProcess(void);

    static bool registered ( void );
    
public slots:
    int update ( void );

public:
    QList<medAbstractParameter*> parameters();

private:
    medItkFiltersGaussianProcessPrivate<itk::SmoothingRecursiveGaussianImageFilter> *d;
};

dtkAbstractProcess * createmedItkFiltersGaussianProcess(void);


