/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCore.h>

namespace medCore
{

namespace pluginManager
{
    void initialize(const QString& path, bool verbose)
    {
        for(QString const& realpath : path.split(';'))
        {
            if(realpath.isEmpty())
                break;

            medCore::arithmeticalOperation::initialize(realpath, verbose);
            medCore::dwiMasking::initialize(realpath, verbose);
            medCore::diffusionModelEstimation::initialize(realpath, verbose);
            medCore::diffusionScalarMaps::initialize(realpath, verbose);
            medCore::tractography::initialize(realpath, verbose);
            medCore::morphomathOperation::initialize(realpath, verbose);
            medCore::maskImage::initialize(realpath, verbose);
            medCore::singleFilterOperation::initialize(realpath, verbose);
        }
    }
}

// arithmetic
namespace arithmeticalOperation
{
    namespace _private
    {
        medAbstractArithmeticOperationProcessPluginManager manager;
    }

    medAbstractArithmeticOperationProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }
    void initialize(const QString& path, bool verbose)
    {
        pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }

    namespace addImage
    {
        namespace _private
        {
            medAbstractAddImageProcessPluginFactory factory;
        }

        medAbstractAddImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace subtractImage
    {
        namespace _private
        {
            medAbstractSubtractImageProcessPluginFactory factory;
        }
        medAbstractSubtractImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace multiplyImage
    {
        namespace _private
        {
            medAbstractMultiplyImageProcessPluginFactory factory;
        }
        medAbstractMultiplyImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace divideImage
    {
        namespace _private
        {
            medAbstractDivideImageProcessPluginFactory factory;
        }

        medAbstractDivideImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
}

// single filter
namespace singleFilterOperation
{
    namespace _private
    {
        medAbstractSingleFilterOperationProcessPluginManager manager;
    }

    medAbstractSingleFilterOperationProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }
    void initialize(const QString& path, bool verbose)
    {
        pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }

    namespace gaussianFilter
    {
        namespace _private
        {
            medAbstractGaussianFilterProcessPluginFactory factory;
        }

        medAbstractGaussianFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
}

// DWI masking
namespace dwiMasking
{
    namespace _private
    {
        medAbstractDWIMaskingProcessPluginManager manager;
        medAbstractDWIMaskingProcessPluginFactory factory;
    }

    medAbstractDWIMaskingProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }

    void initialize(const QString& path, bool verbose)
    {
        if(verbose)
            pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }

    medAbstractDWIMaskingProcessPluginFactory& pluginFactory(void)
    {
        return _private::factory;
    }
}

// Diffusion model estimation
namespace diffusionModelEstimation
{
    namespace _private
    {
        medAbstractDiffusionModelEstimationProcessPluginManager manager;
        medAbstractDiffusionModelEstimationProcessPluginFactory factory;
    }

    medAbstractDiffusionModelEstimationProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }

    void initialize(const QString& path, bool verbose)
    {
        if(verbose)
            pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }

    medAbstractDiffusionModelEstimationProcessPluginFactory& pluginFactory(void)
    {
        return _private::factory;
    }
}

// Diffusion scalar maps
namespace diffusionScalarMaps
{
    namespace _private
    {
        medAbstractDiffusionScalarMapsProcessPluginManager manager;
        medAbstractDiffusionScalarMapsProcessPluginFactory factory;
    }

    medAbstractDiffusionScalarMapsProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }

    void initialize(const QString& path, bool verbose)
    {
        if(verbose)
            pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }

    medAbstractDiffusionScalarMapsProcessPluginFactory& pluginFactory(void)
    {
        return _private::factory;
    }
}

// Tractography
namespace tractography
{
    namespace _private
    {
        medAbstractTractographyProcessPluginManager manager;
        medAbstractTractographyProcessPluginFactory factory;
    }

    medAbstractTractographyProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }

    void initialize(const QString& path, bool verbose)
    {
        if(verbose)
            pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }

    medAbstractTractographyProcessPluginFactory& pluginFactory(void)
    {
        return _private::factory;
    }
}

// morphomath
namespace morphomathOperation
{

    namespace _private
    {
        medAbstractMorphomathOperationProcessPluginManager manager;
        medAbstractMorphomathOperationProcessPluginFactory factory;
    }

    medAbstractMorphomathOperationProcessPluginFactory& pluginFactory(void)
    {
        return _private::factory;
    }

    medAbstractMorphomathOperationProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }
    void initialize(const QString& path, bool verbose)
    {
        pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }



    namespace erodeImage
    {
        namespace _private
        {
            medAbstractErodeImageProcessPluginFactory factory;
        }
        medAbstractErodeImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace dilateImage
    {
        namespace _private
        {
            medAbstractDilateImageProcessPluginFactory factory;
        }

        medAbstractDilateImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace openingImage
    {
        namespace _private
        {
            medAbstractOpeningImageProcessPluginFactory factory;
        }

        medAbstractOpeningImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace closingImage
    {
        namespace _private
        {
            medAbstractClosingImageProcessPluginFactory factory;
        }
        medAbstractClosingImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
}

namespace maskImage
{
    namespace _private
    {
        medAbstractMaskImageProcessPluginManager manager;
    }

    medAbstractMaskImageProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }

    namespace _private
    {
        medAbstractMaskImageProcessPluginFactory factory;
    }

    medAbstractMaskImageProcessPluginFactory& pluginFactory(void)
    {
        return _private::factory;
    }

    void initialize(const QString& path, bool verbose)
    {
        pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }
}

}// end of medProcessLayer

