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

void medItkArithmeticOperationProcessPlugin::initialize(void)
{
    medCore::arithmeticalOperation::addImage::pluginFactory().record(medItkAddImageProcess::staticMetaObject.className()
                                                                     , medItkAddImageProcessCreator);
    medCore::arithmeticalOperation::subtractImage::pluginFactory().record(medItkSubtractImageProcess::staticMetaObject.className()
                                                                          , medItkSubtractImageProcessCreator);
    medCore::arithmeticalOperation::multiplyImage::pluginFactory().record(medItkMultiplyImageProcess::staticMetaObject.className()
                                                                          , medItkMultiplyImageProcessCreator);
    medCore::arithmeticalOperation::divideImage::pluginFactory().record(medItkDivideImageProcess::staticMetaObject.className()
                                                                        , medItkDivideImageProcessCreator);
}

void medItkArithmeticOperationProcessPlugin::uninitialize(void)
{

}
DTK_DEFINE_PLUGIN(medItkAddImageProcess)
DTK_DEFINE_PLUGIN(medItkSubtractImageProcess)
DTK_DEFINE_PLUGIN(medItkMultiplyImageProcess)
DTK_DEFINE_PLUGIN(medItkDivideImageProcess)

