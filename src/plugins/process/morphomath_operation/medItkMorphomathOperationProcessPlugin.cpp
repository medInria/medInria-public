/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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
#include <medItkErodeImageProcessPresenter.h>
#include <medItkDilateImageProcessPresenter.h>

void medItkMorphomathOperationProcessPlugin::initialize(void)
{
    medCore::morphomathOperation::openingImage::pluginFactory().record(medItkOpeningImageProcess::staticMetaObject.className()
                                                                       , medItkOpeningImageProcessCreator);
    medCore::morphomathOperation::closingImage::pluginFactory().record(medItkClosingImageProcess::staticMetaObject.className()
                                                                       , medItkClosingImageProcessCreator);
    medCore::morphomathOperation::dilateImage::pluginFactory().record(medItkDilateImageProcess::staticMetaObject.className()
                                                                      , medItkDilateImageProcessCreator);
    medCore::morphomathOperation::erodeImage::pluginFactory().record(medItkErodeImageProcess::staticMetaObject.className()
                                                                     , medItkErodeImageProcessCreator);

    medWidgets::morphomathOperation::openingImage::presenterFactory().record(medItkOpeningImageProcess::staticMetaObject.className()
                                                                             , medItkOpeningImageProcessPresenterCreator);

    medWidgets::morphomathOperation::closingImage::presenterFactory().record(medItkClosingImageProcess::staticMetaObject.className()
                                                                             , medItkClosingImageProcessPresenterCreator);

    medWidgets::morphomathOperation::dilateImage::presenterFactory().record(medItkDilateImageProcess::staticMetaObject.className()
                                                                            , medItkDilateImageProcessPresenterCreator);

    medWidgets::morphomathOperation::erodeImage::presenterFactory().record(medItkErodeImageProcess::staticMetaObject.className()
                                                                           , medItkErodeImageProcessPresenterCreator);


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
