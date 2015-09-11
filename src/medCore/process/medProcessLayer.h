/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>
#include <medAbstractAddImageProcess.h>
#include <medAbstractSubtractImageProcess.h>
#include <medAbstractMultiplyImageProcess.h>
#include <medAbstractDivideImageProcess.h>

#include <medInriaExport.h>

#include <medAbstractErodeImageProcess.h>
#include <medAbstractDilateImageProcess.h>
#include <medAbstractOpeningImageProcess.h>
#include <medAbstractClosingImageProcess.h>

namespace medProcessLayer
{
    MEDINRIA_EXPORT medProcessDetails readDetailsFromJson(QString const& filePath);

    namespace pluginManager
    {
        MEDINRIA_EXPORT void initialize(const QString& path = QString(), bool verbose = true);
    }

    namespace arithmeticalOperation
    {
        namespace addImage
        {
            MEDINRIA_EXPORT medAbstractAddImageProcessPluginManager& pluginManager(void);
            MEDINRIA_EXPORT medAbstractAddImageProcessPluginFactory& pluginFactory(void);
            MEDINRIA_EXPORT void initialize(const QString& path, bool verbose = true);

        }
        namespace subtractImage
        {
            MEDINRIA_EXPORT medAbstractSubtractImageProcessPluginManager& pluginManager(void);
            MEDINRIA_EXPORT medAbstractSubtractImageProcessPluginFactory& pluginFactory(void);
            MEDINRIA_EXPORT void initialize(const QString& path, bool verbose = true);

        }
        namespace multiplyImage
        {
            MEDINRIA_EXPORT medAbstractMultiplyImageProcessPluginManager& pluginManager(void);
            MEDINRIA_EXPORT medAbstractMultiplyImageProcessPluginFactory& pluginFactory(void);
            MEDINRIA_EXPORT void initialize(const QString& path, bool verbose = true);

        }
        namespace divideImage
        {
            MEDINRIA_EXPORT medAbstractDivideImageProcessPluginManager& pluginManager(void);
            MEDINRIA_EXPORT medAbstractDivideImageProcessPluginFactory& pluginFactory(void);
            MEDINRIA_EXPORT void initialize(const QString& path, bool verbose = true);
        }
    }

    namespace morphomathOperation
    {
        namespace erodeImage
        {
            MEDINRIA_EXPORT medAbstractErodeImageProcessPluginManager& pluginManager(void);
            MEDINRIA_EXPORT medAbstractErodeImageProcessPluginFactory& pluginFactory(void);
            MEDINRIA_EXPORT void initialize(const QString& path, bool verbose = true);
        }
        namespace dilateImage
        {
            MEDINRIA_EXPORT medAbstractDilateImageProcessPluginManager& pluginManager(void);
            MEDINRIA_EXPORT medAbstractDilateImageProcessPluginFactory& pluginFactory(void);
            MEDINRIA_EXPORT void initialize(const QString& path, bool verbose = true);
        }
        namespace openingImage
        {
            MEDINRIA_EXPORT medAbstractOpeningImageProcessPluginManager& pluginManager(void);
            MEDINRIA_EXPORT medAbstractOpeningImageProcessPluginFactory& pluginFactory(void);
            MEDINRIA_EXPORT void initialize(const QString& path, bool verbose = true);
        }
        namespace closingImage
        {
            MEDINRIA_EXPORT medAbstractClosingImageProcessPluginManager& pluginManager(void);
            MEDINRIA_EXPORT medAbstractClosingImageProcessPluginFactory& pluginFactory(void);
            MEDINRIA_EXPORT void initialize(const QString& path, bool verbose = true);
        }
    }
}
