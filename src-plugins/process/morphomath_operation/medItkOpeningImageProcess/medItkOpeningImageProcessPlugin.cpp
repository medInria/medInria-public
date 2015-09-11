/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkOpeningImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkOpeningImageProcess.h>


void medItkOpeningImageProcessPlugin::initialize(void)
{
    medProcessLayer::morphomathOperation::openingImage::pluginFactory().record("medItkOpeningImageProcess",
                                                                               medItkOpeningImageProcessCreator);
}

void medItkOpeningImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkOpeningImageProcess)

