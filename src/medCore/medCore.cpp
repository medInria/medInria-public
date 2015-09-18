#include "medCore.h"

namespace medCore
{
    namespace filtering
    {
        namespace add
        {
            namespace _private
            {
                medAbstractAddFilterPluginFactory factory;
                medAbstractAddFilterPluginManager manager;
            }

            medAbstractAddFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractAddFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace close
        {
            namespace _private
            {
                medAbstractCloseFilterPluginFactory factory;
                medAbstractCloseFilterPluginManager manager;
            }

            medAbstractCloseFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractCloseFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace dilate
        {
            namespace _private
            {
                medAbstractDilateFilterPluginFactory factory;
                medAbstractDilateFilterPluginManager manager;
            }

            medAbstractDilateFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractDilateFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace divide
        {
            namespace _private
            {
                medAbstractDivideFilterPluginFactory factory;
                medAbstractDivideFilterPluginManager manager;
            }

            medAbstractDivideFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractDivideFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace erode
        {
            namespace _private
            {
                medAbstractErodeFilterPluginFactory factory;
                medAbstractErodeFilterPluginManager manager;
            }

            medAbstractErodeFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractErodeFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }
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

        namespace invert
        {
            namespace _private
            {
                medAbstractInvertFilterPluginFactory factory;
                medAbstractInvertFilterPluginManager manager;
            }

            medAbstractInvertFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractInvertFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace median
        {
            namespace _private
            {
                medAbstractMedianFilterPluginFactory factory;
                medAbstractMedianFilterPluginManager manager;
            }

            medAbstractMedianFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractMedianFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace multiply
        {
            namespace _private
            {
                medAbstractMultiplyFilterPluginFactory factory;
                medAbstractMultiplyFilterPluginManager manager;
            }

            medAbstractMultiplyFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractMultiplyFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace normalize
        {
            namespace _private
            {
                medAbstractNormalizeFilterPluginFactory factory;
                medAbstractNormalizeFilterPluginManager manager;
            }

            medAbstractNormalizeFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractNormalizeFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace open
        {
            namespace _private
            {
                medAbstractOpenFilterPluginFactory factory;
                medAbstractOpenFilterPluginManager manager;
            }

            medAbstractOpenFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractOpenFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace shrink
        {
            namespace _private
            {
                medAbstractShrinkFilterPluginFactory factory;
                medAbstractShrinkFilterPluginManager manager;
            }

            medAbstractShrinkFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractShrinkFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace substract
        {
            namespace _private
            {
                medAbstractSubstractFilterPluginFactory factory;
                medAbstractSubstractFilterPluginManager manager;
            }

            medAbstractSubstractFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractSubstractFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }

        namespace windowing
        {
            namespace _private
            {
                medAbstractWindowingFilterPluginFactory factory;
                medAbstractWindowingFilterPluginManager manager;
            }

            medAbstractWindowingFilterPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractWindowingFilterPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }
    }

    namespace registration
    {
        namespace nonRigid
        {
            namespace _private
            {
                medAbstractRegistrationProcessPluginFactory factory;
                medAbstractRegistrationProcessPluginManager manager;
                medAbstractRegistrationProcessWidgetFactory widgetFactory;
            }

            medAbstractRegistrationProcessPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractRegistrationProcessPluginManager& pluginManager(void)
            {
                return _private::manager;
            }

            medAbstractRegistrationProcessWidgetFactory& widgetFactory(void)
            {
                return _private::widgetFactory;
            }
        }

        namespace rigid
        {
            namespace _private
            {
                medAbstractRigidRegistrationPluginFactory factory;
                medAbstractRigidRegistrationPluginManager manager;
            }

            medAbstractRigidRegistrationPluginFactory& pluginFactory(void)
            {
                return _private::factory;
            }

            medAbstractRigidRegistrationPluginManager& pluginManager(void)
            {
                return _private::manager;
            }
        }
    }
}
