/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkAddImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkAddImageProcess.h>


void medItkAddImageProcessPlugin::initialize(void)
{
    medProcessLayer::arithmeticalOperation::addImage::pluginFactory().record(medItkAddImageProcess::staticMetaObject.className(),
                                                                             medItkAddImageProcessCreator);
}

void medItkAddImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkAddImageProcess)

