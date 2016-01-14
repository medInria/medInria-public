/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSingleFilterOperationProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>
#include <medItkGaussianProcess.h>
#include <medItkGaussianProcessPresenter.h>


void medItkSingleFilterOperationProcessPlugin::initialize(void)
{

    medCore::singleFilterOperation::gaussianFilter::pluginFactory().record(medItkGaussianProcess::staticMetaObject.className()
                                                                     , medItkGaussianProcessCreator);

    medWidgets::singleFilterOperation::gaussianFilter::presenterFactory().record(medItkGaussianProcess::staticMetaObject.className(),
                                                      medItkGaussianProcessPresenterCreator);
void medItkSingleFilterOperationProcessPlugin::uninitialize(void)
{

}
DTK_DEFINE_PLUGIN(medItkGaussianProcess)


