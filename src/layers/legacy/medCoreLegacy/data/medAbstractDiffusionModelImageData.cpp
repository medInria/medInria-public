/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDiffusionModelImageData.h>
#include <dtkLog/dtkLog.h>

medAbstractDiffusionModelImageData::medAbstractDiffusionModelImageData() : medAbstractImageData()
{
}

const QString medAbstractDiffusionModelImageData::PixelMeaning() const
{
    if (hasMetaData(medAbstractImageData::PixelMeaningMetaData))
        return metadata(medAbstractImageData::PixelMeaningMetaData);
    return "";
}
