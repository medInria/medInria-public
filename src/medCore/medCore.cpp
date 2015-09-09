#include "medCore.h"

namespace medCore
{
    namespace filtering
    {
        namespace gaussian
        {
            namespace _private
            {
                medAbstractGaussianFilterPluginFactory factory;
                medAbstractGaussianFilterPluginManager manager;
            }

            medAbstractGaussianFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractGaussianFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }
    }
}
