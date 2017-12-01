/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2017. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkBiasCorrectionProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>

#include <medItkBiasCorrectionProcess.h>
#include <medItkBiasCorrectionProcessPresenter.h>

void medItkBiasCorrectionProcessPlugin::initialize(void)
{
    medCore::singleFilterOperation::biasCorrection::pluginFactory().record(medItkBiasCorrectionProcess::staticMetaObject.className(), medItkBiasCorrectionProcessCreator);

    medWidgets::singleFilterOperation::biasCorrection::presenterFactory().record(medItkBiasCorrectionProcess::staticMetaObject.className(), medItkBiasCorrectionProcessPresenterCreator);
}

void medItkBiasCorrectionProcessPlugin::uninitialize(void)
{

}

DTK_DEFINE_PLUGIN(medItkBiasCorrectionProcess)
