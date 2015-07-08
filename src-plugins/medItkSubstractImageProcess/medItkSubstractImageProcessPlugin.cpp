/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubstractImageProcessPlugin.h>

#include <medProcessLayer.h>
#include <medItkSubstractImageProcessPresenter.h>


void medItkSubstractImageProcessPlugin::initialize(void)
{

    medProcessLayer::substractImage::pluginFactory().record("medItkSubstractImageProcess",
                                                            medItkSubstractImageProcessPresenterCreator);
}

void medItkSubstractImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkSubstractImageProcess)

