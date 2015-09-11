/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractAreaPlugin.h>

#include <medCoreExport.h>

namespace medGuiLayer
{
    namespace pluginManager
    {
        MEDCORE_EXPORT void initialize(const QString& path = QString());
    }

    namespace area
    {
        MEDCORE_EXPORT medAbstractAreaPluginManager& pluginManager(void);
        MEDCORE_EXPORT medAbstractAreaPluginFactory& pluginFactory(void);
        MEDCORE_EXPORT void initialize(const QString& path);
    }
}
