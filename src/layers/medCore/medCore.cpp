/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCore.h>

namespace medCore
{

DTK_DEFINE_LAYER_MANAGER

namespace pluginManager
{
    void initialize(const QString& path)
    {
        for(QString const& realpath : path.split(';'))
        {
            if(realpath.isEmpty())
                break;

            medCore::arithmeticOperation::pluginManager().initialize(realpath);
            medCore::dwiMasking::pluginManager().initialize(realpath);
            medCore::diffusionModelEstimation::pluginManager().initialize(realpath);
            medCore::diffusionScalarMaps::pluginManager().initialize(realpath);
            medCore::tractography::pluginManager().initialize(realpath);
            medCore::morphomathOperation::pluginManager().initialize(realpath);
            medCore::maskImage::pluginManager().initialize(realpath);
            medCore::singleFilterOperation::pluginManager().initialize(realpath);
            medCore::dataConverter::pluginManager().initialize(realpath);
        }
    }
}

DTK_DEFINE_CONCEPT(medAbstractProcess,generic,medCore)
DTK_DEFINE_CONCEPT(medAbstractDWIMaskingProcess,dwiMasking,medCore)
DTK_DEFINE_CONCEPT(medAbstractDiffusionModelEstimationProcess,diffusionModelEstimation,medCore)
DTK_DEFINE_CONCEPT(medAbstractDiffusionScalarMapsProcess,diffusionScalarMaps,medCore)
DTK_DEFINE_CONCEPT(medAbstractTractographyProcess,tractography,medCore)
DTK_DEFINE_CONCEPT(medAbstractMaskImageProcess,maskImage,medCore)
DTK_DEFINE_CONCEPT(medAbstractDataConverter,dataConverter,medCore)

// arithmetic
namespace arithmeticOperation
{
    namespace _private
    {
        medAbstractArithmeticOperationProcessPluginManager manager;
        medAbstractArithmeticOperationProcessPluginFactory factory;
    }

    medAbstractArithmeticOperationProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }

    medAbstractArithmeticOperationProcessPluginFactory& pluginFactory(void)
    {
        return _private::factory;
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

    namespace addFilter
    {
        namespace _private
        {
            medAbstractAddFilterProcessPluginFactory factory;
        }

        medAbstractAddFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace divideFilter
    {
        namespace _private
        {
            medAbstractDivideFilterProcessPluginFactory factory;
        }

        medAbstractDivideFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
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

    namespace invertFilter
    {
        namespace _private
        {
            medAbstractInvertFilterProcessPluginFactory factory;
        }

        medAbstractInvertFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace medianFilter
    {
        namespace _private
        {
            medAbstractMedianFilterProcessPluginFactory factory;
        }

        medAbstractMedianFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace multiplyFilter
    {
        namespace _private
        {
            medAbstractMultiplyFilterProcessPluginFactory factory;
        }

        medAbstractMultiplyFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace normalizeFilter
    {
        namespace _private
        {
            medAbstractNormalizeFilterProcessPluginFactory factory;
        }

        medAbstractNormalizeFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace shrinkFilter
    {
        namespace _private
        {
            medAbstractShrinkFilterProcessPluginFactory factory;
        }

        medAbstractShrinkFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace subtractFilter
    {
        namespace _private
        {
            medAbstractSubtractFilterProcessPluginFactory factory;
        }

        medAbstractSubtractFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace windowingFilter
    {
        namespace _private
        {
            medAbstractWindowingFilterProcessPluginFactory factory;
        }

        medAbstractWindowingFilterProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace imageDenoising
    {
        namespace _private
        {
            medAbstractImageDenoisingProcessPluginFactory factory;
        }

        medAbstractImageDenoisingProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace relaxometryEstimation
    {
        namespace _private
        {
            medAbstractRelaxometryEstimationProcessPluginFactory factory;
        }

        medAbstractRelaxometryEstimationProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace symmetryAlignment
    {
        namespace _private
        {
            medAbstractSymmetryPlaneAlignmentProcessPluginFactory factory;
        }

        medAbstractSymmetryPlaneAlignmentProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }

    namespace biasCorrection
    {
       namespace _private
       {
          medAbstractBiasCorrectionProcessPluginFactory factory;
       }

       medAbstractBiasCorrectionProcessPluginFactory& pluginFactory(void)
       {
          return _private::factory;
       }
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

} // end of medCore

