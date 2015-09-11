#ifndef MEDCORE_H
#define MEDCORE_H

#include "medCoreExport.h"

#include "medAbstractAddFilter.h"
#include "medAbstractCloseFilter.h"
#include "medAbstractDiffusionProcess.h"
#include "medAbstractDilateFilter.h"
#include "medAbstractDivideFilter.h"
#include "medAbstractErodeFilter.h"
#include "medAbstractGaussianFilter.h"
#include "medAbstractInvertFilter.h"
#include "medAbstractMedianFilter.h"
#include "medAbstractMultiplyFilter.h"
#include "medAbstractNormalizeFilter.h"
#include "medAbstractOpenFilter.h"
#include "medAbstractProcess.h"
#include "medAbstractRegistrationProcess.h"
#include "medAbstractShrinkFilter.h"
#include "medAbstractSubstractFilter.h"
#include "medAbstractWindowingFilter.h"

class medAbstractAddFilter;
class medAbstractAddFilterPluginFactory;
class medAbstractAddFilterPluginManager;

class medAbstractCloseFilter;
class medAbstractCloseFilterPluginFactory;
class medAbstractCloseFilterPluginManager;

class medAbstractDilateFilter;
class medAbstractDilateFilterPluginFactory;
class medAbstractDilateFilterPluginManager;

class medAbstractDivideFilter;
class medAbstractDivideFilterPluginFactory;
class medAbstractDivideFilterPluginManager;

class medAbstractErodeFilter;
class medAbstractErodeFilterPluginFactory;
class medAbstractErodeFilterPluginManager;

class medAbstractInvertFilter;
class medAbstractInvertFilterPluginFactory;
class medAbstractInvertFilterPluginManager;

class medAbstractMedianFilter;
class medAbstractMedianFilterPluginFactory;
class medAbstractMedianFilterPluginManager;

class medAbstractMultiplyFilter;
class medAbstractMultiplyFilterPluginFactory;
class medAbstractMultiplyFilterPluginManager;

class medAbstractNormalizeFilter;
class medAbstractNormalizeFilterPluginFactory;
class medAbstractNormalizeFilterPluginManager;

class medAbstractOpenFilter;
class medAbstractOpenFilterPluginFactory;
class medAbstractOpenFilterPluginManager;

class medAbstractShrinkFilter;
class medAbstractShrinkFilterPluginFactory;
class medAbstractShrinkFilterPluginManager;

class medAbstractSubstractFilter;
class medAbstractSubstractFilterPluginFactory;
class medAbstractSubstractFilterPluginManager;

class medAbstractWindowingFilter;
class medAbstractWindowingFilterPluginFactory;
class medAbstractWindowingFilterPluginManager;

class medAbstractGaussianFilter;
class medAbstractGaussianFilterPluginFactory;
class medAbstractGaussianFilterPluginManager;

namespace medCore
{
    namespace filtering
    {
        namespace add
        {
                MEDCORE_EXPORT medAbstractAddFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractAddFilterPluginManager& pluginManager(void);
        }

        namespace close
        {
                MEDCORE_EXPORT medAbstractCloseFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractCloseFilterPluginManager& pluginManager(void);
        }

        namespace dilate
        {
                MEDCORE_EXPORT medAbstractDilateFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractDilateFilterPluginManager& pluginManager(void);
        }

        namespace divide
        {
                MEDCORE_EXPORT medAbstractDivideFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractDivideFilterPluginManager& pluginManager(void);
        }

        namespace erode
        {
                MEDCORE_EXPORT medAbstractErodeFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractErodeFilterPluginManager& pluginManager(void);
        }

        namespace gaussian
        {
                MEDCORE_EXPORT medAbstractGaussianFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractGaussianFilterPluginManager& pluginManager(void);
        }

        namespace invert
        {
                MEDCORE_EXPORT medAbstractInvertFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractInvertFilterPluginManager& pluginManager(void);
        }

        namespace median
        {
                MEDCORE_EXPORT medAbstractMedianFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractMedianFilterPluginManager& pluginManager(void);
        }

        namespace multiply
        {
                MEDCORE_EXPORT medAbstractMultiplyFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractMultiplyFilterPluginManager& pluginManager(void);
        }

        namespace normalize
        {
                MEDCORE_EXPORT medAbstractNormalizeFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractNormalizeFilterPluginManager& pluginManager(void);
        }

        namespace open
        {
                MEDCORE_EXPORT medAbstractOpenFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractOpenFilterPluginManager& pluginManager(void);
        }

        namespace shrink
        {
                MEDCORE_EXPORT medAbstractShrinkFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractShrinkFilterPluginManager& pluginManager(void);
        }

        namespace substract
        {
                MEDCORE_EXPORT medAbstractSubstractFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractSubstractFilterPluginManager& pluginManager(void);
        }

        namespace windowing
        {
                MEDCORE_EXPORT medAbstractWindowingFilterPluginFactory& pluginFactory(void);
                MEDCORE_EXPORT medAbstractWindowingFilterPluginManager& pluginManager(void);
        }

    }
}

#endif
