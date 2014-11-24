/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <medAbstractDiffusionModelImageData.h>

template <unsigned DIM,typename T>
class medAbstractTypedDiffusionModelImageData: public medAbstractDiffusionModelImageData
{
public:

    medAbstractTypedDiffusionModelImageData(): medAbstractDiffusionModelImageData() {}
    
    virtual ~medAbstractTypedDiffusionModelImageData() {}
    
    virtual int                                dimension() const { return DIM;       }
    virtual const medAbstractImageData::PixId& PixelType() const { return typeid(T); }
};


