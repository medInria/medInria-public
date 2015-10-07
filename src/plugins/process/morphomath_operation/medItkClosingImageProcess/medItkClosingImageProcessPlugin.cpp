/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkClosingImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkClosingImageProcess.h>


void medItkClosingImageProcessPlugin::initialize(void)
{
    medProcessLayer::morphomathOperation::closingImage::pluginFactory().record(medItkClosingImageProcess::staticMetaObject.className(),
                                                                               medItkClosingImageProcessCreator);
}

void medItkClosingImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkClosingImageProcess)

