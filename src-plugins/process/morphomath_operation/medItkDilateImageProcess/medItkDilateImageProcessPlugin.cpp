/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkDilateImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkDilateImageProcess.h>


void medItkDilateImageProcessPlugin::initialize(void)
{
    medProcessLayer::morphomathOperation::dilateImage::pluginFactory().record("medItkDilateImageProcess",
                                                                              medItkDilateImageProcessCreator);
}

void medItkDilateImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkDilateImageProcess)

