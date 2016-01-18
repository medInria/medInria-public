/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkMorphomathOperationProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>

#include <medItkOpeningImageProcess.h>
#include <medItkClosingImageProcess.h>
#include <medItkDilateImageProcess.h>
#include <medItkErodeImageProcess.h>

#include <medAbstractMorphomathOperationProcessPresenter.h>

#include <medItkOpeningImageProcessPresenter.h>
#include <medItkClosingImageProcessPresenter.h>

void medItkMorphomathOperationProcessPlugin::initialize(void)
{
    medCore::morphomathOperation::pluginFactory().record(medItkOpeningImageProcess::staticMetaObject.className()
                                                                     , medItkOpeningImageProcessCreator);
    medCore::morphomathOperation::pluginFactory().record(medItkClosingImageProcess::staticMetaObject.className()
                                                                          , medItkClosingImageProcessCreator);
    medCore::morphomathOperation::pluginFactory().record(medItkDilateImageProcess::staticMetaObject.className()
                                                                          , medItkDilateImageProcessCreator);
    medCore::morphomathOperation::pluginFactory().record(medItkErodeImageProcess::staticMetaObject.className()
                                                                        , medItkErodeImageProcessCreator);

    medWidgets::morphomathOperation::presenterFactory().record(medItkOpeningImageProcess::staticMetaObject.className()
                                                                             , medItkOpeningImageProcessPresenterCreator);

    medWidgets::morphomathOperation::presenterFactory().record(medItkClosingImageProcess::staticMetaObject.className()
                                                                             , medItkClosingImageProcessPresenterCreator);

    medWidgets::morphomathOperation::presenterFactory().record(medItkDilateImageProcess::staticMetaObject.className()
                                                                             , medAbstractMorphomathOperationProcessPresenterCreator);

    medWidgets::morphomathOperation::presenterFactory().record(medItkErodeImageProcess::staticMetaObject.className()
                                                                             , medAbstractMorphomathOperationProcessPresenterCreator);


}

void medItkMorphomathOperationProcessPlugin::uninitialize(void)
{

}
DTK_DEFINE_PLUGIN(medItkOpeningImageProcess)
DTK_DEFINE_PLUGIN(medItkClosingImageProcess)
DTK_DEFINE_PLUGIN(medItkDilateImageProcess)
DTK_DEFINE_PLUGIN(medItkErodeImageProcess)
DTK_DEFINE_PLUGIN(medItkOpeningImageProcessPresenter)
DTK_DEFINE_PLUGIN(medItkClosingImageProcessPresenter)
