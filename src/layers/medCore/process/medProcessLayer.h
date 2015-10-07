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

#include <medCoreExport.h>

#include <medAbstractErodeImageProcess.h>
#include <medAbstractDilateImageProcess.h>
#include <medAbstractOpeningImageProcess.h>
#include <medAbstractClosingImageProcess.h>

namespace medProcessLayer
{
    MEDCORE_EXPORT medProcessDetails readDetailsFromJson(QString const& filePath);

    namespace pluginManager
    {
        MEDCORE_EXPORT void initialize(const QString& path = QString(), bool verbose = true);
    }

    namespace arithmeticalOperation
    {
        namespace addImage
        {
            MEDCORE_EXPORT medAbstractAddImageProcessPluginManager& pluginManager(void);
            MEDCORE_EXPORT medAbstractAddImageProcessPluginFactory& pluginFactory(void);
            MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);

        }
        namespace subtractImage
        {
            MEDCORE_EXPORT medAbstractSubtractImageProcessPluginManager& pluginManager(void);
            MEDCORE_EXPORT medAbstractSubtractImageProcessPluginFactory& pluginFactory(void);
            MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);

        }
        namespace multiplyImage
        {
            MEDCORE_EXPORT medAbstractMultiplyImageProcessPluginManager& pluginManager(void);
            MEDCORE_EXPORT medAbstractMultiplyImageProcessPluginFactory& pluginFactory(void);
            MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);

        }
        namespace divideImage
        {
            MEDCORE_EXPORT medAbstractDivideImageProcessPluginManager& pluginManager(void);
            MEDCORE_EXPORT medAbstractDivideImageProcessPluginFactory& pluginFactory(void);
            MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);
        }
    }

    namespace morphomathOperation
    {
        namespace erodeImage
        {
            MEDCORE_EXPORT medAbstractErodeImageProcessPluginManager& pluginManager(void);
            MEDCORE_EXPORT medAbstractErodeImageProcessPluginFactory& pluginFactory(void);
            MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);
        }
        namespace dilateImage
        {
            MEDCORE_EXPORT medAbstractDilateImageProcessPluginManager& pluginManager(void);
            MEDCORE_EXPORT medAbstractDilateImageProcessPluginFactory& pluginFactory(void);
            MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);
        }
        namespace openingImage
        {
            MEDCORE_EXPORT medAbstractOpeningImageProcessPluginManager& pluginManager(void);
            MEDCORE_EXPORT medAbstractOpeningImageProcessPluginFactory& pluginFactory(void);
            MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);
        }
        namespace closingImage
        {
            MEDCORE_EXPORT medAbstractClosingImageProcessPluginManager& pluginManager(void);
            MEDCORE_EXPORT medAbstractClosingImageProcessPluginFactory& pluginFactory(void);
            MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);
        }
    }
}
