#ifndef MEDCORE_H
#define MEDCORE_H

#include "medCoreExport.h"
#include "medAbstractGaussianFilter.h"

class medAbstractGaussianFilter;
class medAbstractGaussianFilterPluginFactory;
class medAbstractGaussianFilterPluginManager;

namespace medCore
{
    namespace filtering
    {
        namespace gaussian
        {
                MEDCORE_EXPORT medAbstractGaussianFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractGaussianFilterPluginManager& pluginManager(void);
        }
    }
}

#endif
