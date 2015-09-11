/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkMultiplyImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkMultiplyImageProcess.h>


void medItkMultiplyImageProcessPlugin::initialize(void)
{
    medProcessLayer::arithmeticalOperation::multiplyImage::pluginFactory().record("medItkMultiplyImageProcess",
                                                                                  medItkMultiplyImageProcessCreator);
}

void medItkMultiplyImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkMultiplyImageProcess)

