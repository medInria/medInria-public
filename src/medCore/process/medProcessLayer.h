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
}
