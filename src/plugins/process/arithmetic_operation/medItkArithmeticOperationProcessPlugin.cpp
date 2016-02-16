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
#include <medItkAddImageProcessPresenter.h>
#include <medItkSubtractImageProcessPresenter.h>
#include <medItkMultiplyImageProcessPresenter.h>
#include <medItkDivideImageProcessPresenter.h>

void medItkArithmeticOperationProcessPlugin::initialize(void)
{
    medCore::arithmeticOperation::addImage::pluginFactory().record(medItkAddImageProcess::staticMetaObject.className()
                                                                   , medItkAddImageProcessCreator);
    medCore::arithmeticOperation::subtractImage::pluginFactory().record(medItkSubtractImageProcess::staticMetaObject.className()
                                                                        , medItkSubtractImageProcessCreator);
    medCore::arithmeticOperation::multiplyImage::pluginFactory().record(medItkMultiplyImageProcess::staticMetaObject.className()
                                                                        , medItkMultiplyImageProcessCreator);
    medCore::arithmeticOperation::divideImage::pluginFactory().record(medItkDivideImageProcess::staticMetaObject.className()
                                                                      , medItkDivideImageProcessCreator);

    medWidgets::arithmeticOperation::addImage::presenterFactory().record(medItkAddImageProcess::staticMetaObject.className()
                                                                         , medItkAddImageProcessPresenterCreator);
    medWidgets::arithmeticOperation::subtractImage::presenterFactory().record(medItkSubtractImageProcess::staticMetaObject.className()
                                                                              , medItkSubtractImageProcessPresenterCreator);
    medWidgets::arithmeticOperation::multiplyImage::presenterFactory().record(medItkMultiplyImageProcess::staticMetaObject.className()
                                                                              , medItkMultiplyImageProcessPresenterCreator);
    medWidgets::arithmeticOperation::divideImage::presenterFactory().record(medItkDivideImageProcess::staticMetaObject.className()
                                                                            , medItkDivideImageProcessPresenterCreator);
}

void medItkArithmeticOperationProcessPlugin::uninitialize(void)
{

}
DTK_DEFINE_PLUGIN(medItkAddImageProcess)
DTK_DEFINE_PLUGIN(medItkSubtractImageProcess)
DTK_DEFINE_PLUGIN(medItkMultiplyImageProcess)
DTK_DEFINE_PLUGIN(medItkDivideImageProcess)

