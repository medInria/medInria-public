/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkMaskImageProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>

#include <medItkMaskImageProcess.h>
#include <medItkMaskImageProcessPresenter.h>

void medItkMaskImageProcessPlugin::initialize(void)
{
    medCore::maskImage::pluginFactory().record(medItkMaskImageProcess::staticMetaObject.className()
                                               , medItkMaskImageProcessCreator);

    medWidgets::maskImage::presenterFactory().record(medItkMaskImageProcess::staticMetaObject.className()
                                                     , medItkMaskImageProcessPresenterCreator);


}

void medItkMaskImageProcessPlugin::uninitialize(void)
{

}
DTK_DEFINE_PLUGIN(medItkMaskImageProcess)
DTK_DEFINE_PLUGIN(medItkMaskImageProcessPresenter)
