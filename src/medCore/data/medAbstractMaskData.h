/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractData.h>


class MEDCORE_EXPORT medAbstractMaskData : public medAbstractData
{
    Q_OBJECT

public:
             medAbstractMaskData();
             medAbstractMaskData(const  medAbstractMaskData& other);
    virtual ~medAbstractMaskData();


private:
};


