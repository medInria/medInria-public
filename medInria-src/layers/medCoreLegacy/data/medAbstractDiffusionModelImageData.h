/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageData.h>

#include <medCoreLegacyExport.h>

class MEDCORELEGACY_EXPORT medAbstractDiffusionModelImageData : public medAbstractImageData
{
public:
    medAbstractDiffusionModelImageData();

    virtual ~medAbstractDiffusionModelImageData() {}

    virtual const QString PixelMeaning() const;
};
