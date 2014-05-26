/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMaskData.h>


medAbstractMaskData::medAbstractMaskData(void) : medAbstractData()
{
    this->setDataCategory(medData::MASK);
}

medAbstractMaskData::medAbstractMaskData(const  medAbstractMaskData& other): medAbstractData(other)
{
    this->setDataCategory(medData::MASK);
}

medAbstractMaskData::~medAbstractMaskData(void)
{

}
