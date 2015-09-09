/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkDivideImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkDivideImageProcess.h>


void medItkDivideImageProcessPlugin::initialize(void)
{
    medProcessLayer::arithmeticalOperation::divideImage::pluginFactory().record("medItkDivideImageProcess",
                                                                                medItkDivideImageProcessCreator);
}

void medItkDivideImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkDivideImageProcess)

