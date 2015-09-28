/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkErodeImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkErodeImageProcess.h>


void medItkErodeImageProcessPlugin::initialize(void)
{
    medProcessLayer::morphomathOperation::erodeImage::pluginFactory().record(medItkErodeImageProcess::staticMetaObject.className(),
                                                                             medItkErodeImageProcessCreator);
}

void medItkErodeImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkErodeImageProcess)

