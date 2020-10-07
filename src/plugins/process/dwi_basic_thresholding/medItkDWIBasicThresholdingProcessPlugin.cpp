/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkDWIBasicThresholdingProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>
#include <medItkDWIBasicThresholdingProcess.h>
#include <medItkDWIBasicThresholdingProcessPresenter.h>

void medItkDWIBasicThresholdingProcessPlugin::initialize(void)
{
    medCore::dwiMasking::pluginFactory().record(medItkDWIBasicThresholdingProcess::staticMetaObject.className(),
                                                medItkDWIBasicThresholdingProcessCreator);
    medWidgets::dwiMasking::presenterFactory().record(medItkDWIBasicThresholdingProcess::staticMetaObject.className(),
                                                      medItkDWIBasicThresholdingProcessPresenterCreator);
}

void medItkDWIBasicThresholdingProcessPlugin::uninitialize(void)
{
}

DTK_DEFINE_PLUGIN(medItkDWIBasicThresholdingProcess)
DTK_DEFINE_PLUGIN(medItkDWIBasicThresholdingProcessPresenter)
