/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractAddImageProcessPlugin.h>
#include <medAbstractSubtractImageProcessPlugin.h>
#include <medAbstractMultiplyImageProcessPlugin.h>
#include <medAbstractDivideImageProcessPlugin.h>
#include <medAbstractProcess.h>

namespace medProcessLayer
{
    medProcessDetails readDetailsFromJson(QString const& filePath);

    namespace pluginManager
    {
        void initialize(const QString& path = QString());
    }

    namespace arithmeticalOperation
    {
        namespace addImage
        {
            medAbstractAddImageProcessPluginManager& pluginManager(void);
            medAbstractAddImageProcessPluginFactory& pluginFactory(void);
            void initialize(const QString& path);

        }
        namespace subtractImage
        {
            medAbstractSubtractImageProcessPluginManager& pluginManager(void);
            medAbstractSubtractImageProcessPluginFactory& pluginFactory(void);
            void initialize(const QString& path);

        }
        namespace multiplyImage
        {
            medAbstractMultiplyImageProcessPluginManager& pluginManager(void);
            medAbstractMultiplyImageProcessPluginFactory& pluginFactory(void);
            void initialize(const QString& path);

        }
        namespace divideImage
        {
            medAbstractDivideImageProcessPluginManager& pluginManager(void);
            medAbstractDivideImageProcessPluginFactory& pluginFactory(void);
            void initialize(const QString& path);

        }
    }
}
