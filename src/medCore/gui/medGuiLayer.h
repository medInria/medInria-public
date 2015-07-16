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

namespace medGuiLayer
{
    namespace pluginManager
    {
        void initialize(const QString& path = QString());
    }

    namespace area
    {
        medAbstractAreaPluginManager& pluginManager(void);
        medAbstractAreaPluginFactory& pluginFactory(void);
        void initialize(const QString& path);

    }
}
