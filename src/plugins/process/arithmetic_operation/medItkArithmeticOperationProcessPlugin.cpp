/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkArithmeticOperationProcessPlugin.h>

#include <medCore.h>
#include <medItkAddImageProcess.h>
#include <medItkSubtractImageProcess.h>
#include <medItkMultiplyImageProcess.h>
#include <medItkDivideImageProcess.h>

#include <medWidgets.h>
#include <medAbstractArithmeticOperationProcessPresenter.h>

void medItkArithmeticOperationProcessPlugin::initialize(void)
{
    medCore::arithmeticOperation::pluginFactory().record(medItkAddImageProcess::staticMetaObject.className()
                                                                     , medItkAddImageProcessCreator);
    medCore::arithmeticOperation::pluginFactory().record(medItkSubtractImageProcess::staticMetaObject.className()
                                                                          , medItkSubtractImageProcessCreator);
    medCore::arithmeticOperation::pluginFactory().record(medItkMultiplyImageProcess::staticMetaObject.className()
                                                                          , medItkMultiplyImageProcessCreator);
    medCore::arithmeticOperation::pluginFactory().record(medItkDivideImageProcess::staticMetaObject.className()
                                                                        , medItkDivideImageProcessCreator);

    medWidgets::arithmeticOperation::presenterFactory().record(medItkAddImageProcess::staticMetaObject.className()
                                                               , medAbstractArithmeticOperationProcessPresenterCreator);
    medWidgets::arithmeticOperation::presenterFactory().record(medItkSubtractImageProcess::staticMetaObject.className()
                                                               , medAbstractArithmeticOperationProcessPresenterCreator);
    medWidgets::arithmeticOperation::presenterFactory().record(medItkMultiplyImageProcess::staticMetaObject.className()
                                                               , medAbstractArithmeticOperationProcessPresenterCreator);
    medWidgets::arithmeticOperation::presenterFactory().record(medItkDivideImageProcess::staticMetaObject.className()
                                                               , medAbstractArithmeticOperationProcessPresenterCreator);
}

void medItkArithmeticOperationProcessPlugin::uninitialize(void)
{

}
DTK_DEFINE_PLUGIN(medItkAddImageProcess)
DTK_DEFINE_PLUGIN(medItkSubtractImageProcess)
DTK_DEFINE_PLUGIN(medItkMultiplyImageProcess)
DTK_DEFINE_PLUGIN(medItkDivideImageProcess)

