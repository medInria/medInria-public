/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubtractImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkSubtractImageProcess.h>


void medItkSubtractImageProcessPlugin::initialize(void)
{
    medProcessLayer::subtractImage::pluginFactory().record("medItkSubtractImageProcess",
                                                            medItkSubtractImageProcessCreator);
}

void medItkSubtractImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkSubtractImageProcess)

