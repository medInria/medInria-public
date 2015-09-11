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

#include <medInriaExport.h>

namespace medGuiLayer
{
    namespace pluginManager
    {
        MEDINRIA_EXPORT void initialize(const QString& path = QString());
    }

    namespace area
    {
        MEDINRIA_EXPORT medAbstractAreaPluginManager& pluginManager(void);
        MEDINRIA_EXPORT medAbstractAreaPluginFactory& pluginFactory(void);
        MEDINRIA_EXPORT void initialize(const QString& path);
    }
}
