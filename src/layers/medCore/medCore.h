/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>

#include <medAbstractArithmeticOperationProcess.h>
#include <medAbstractAddImageProcess.h>
#include <medAbstractSubtractImageProcess.h>
#include <medAbstractMultiplyImageProcess.h>
#include <medAbstractDivideImageProcess.h>

#include <medAbstractSingleFilterOperationProcess.h>
#include <medAbstractAddFilterProcess.h>
#include <medAbstractDivideFilterProcess.h>
#include <medAbstractGaussianFilterProcess.h>
#include <medAbstractInvertFilterProcess.h>
#include <medAbstractMedianFilterProcess.h>
#include <medAbstractMultiplyFilterProcess.h>
#include <medAbstractNormalizeFilterProcess.h>
#include <medAbstractSymmetryPlaneAlignmentProcess.h>
#include <medAbstractShrinkFilterProcess.h>
#include <medAbstractSubtractFilterProcess.h>
#include <medAbstractWindowingFilterProcess.h>
#include <medAbstractImageDenoisingProcess.h>
#include <medAbstractRelaxometryEstimationProcess.h>

#include <medAbstractMorphomathOperationProcess.h>
#include <medAbstractErodeImageProcess.h>
#include <medAbstractDilateImageProcess.h>
#include <medAbstractOpeningImageProcess.h>
#include <medAbstractClosingImageProcess.h>

#include <medAbstractMaskImageProcess.h>
#include <medAbstractDWIMaskingProcess.h>
#include <medAbstractDiffusionModelEstimationProcess.h>
#include <medAbstractDiffusionScalarMapsProcess.h>
#include <medAbstractTractographyProcess.h>

#include <medCoreExport.h>

namespace medCore
{
    namespace pluginManager
    {
        MEDCORE_EXPORT void initialize(const QString& path = QString());
    }

    DTK_DECLARE_CONCEPT(medAbstractProcess,MEDCORE_EXPORT,generic)
    DTK_DECLARE_CONCEPT(medAbstractDWIMaskingProcess,MEDCORE_EXPORT,dwiMasking)
    DTK_DECLARE_CONCEPT(medAbstractDiffusionModelEstimationProcess,MEDCORE_EXPORT,diffusionModelEstimation)
    DTK_DECLARE_CONCEPT(medAbstractDiffusionScalarMapsProcess,MEDCORE_EXPORT,diffusionScalarMaps)
    DTK_DECLARE_CONCEPT(medAbstractTractographyProcess,MEDCORE_EXPORT,tractography)
    DTK_DECLARE_CONCEPT(medAbstractMaskImageProcess,MEDCORE_EXPORT,maskImage)

    namespace arithmeticOperation
    {
        MEDCORE_EXPORT medAbstractArithmeticOperationProcessPluginManager& pluginManager(void);
        MEDCORE_EXPORT medAbstractArithmeticOperationProcessPluginFactory& pluginFactory(void);
        MEDCORE_EXPORT void initialize(const QString& path);

        namespace addImage
        {
            MEDCORE_EXPORT medAbstractAddImageProcessPluginFactory& pluginFactory(void);
        }
        namespace subtractImage
        {
            MEDCORE_EXPORT medAbstractSubtractImageProcessPluginFactory& pluginFactory(void);
        }
        namespace multiplyImage
        {
            MEDCORE_EXPORT medAbstractMultiplyImageProcessPluginFactory& pluginFactory(void);
        }
        namespace divideImage
        {
            MEDCORE_EXPORT medAbstractDivideImageProcessPluginFactory& pluginFactory(void);
        }
    }

    namespace singleFilterOperation
    {
        MEDCORE_EXPORT medAbstractSingleFilterOperationProcessPluginManager& pluginManager(void);
        MEDCORE_EXPORT void initialize(const QString& path);

        namespace addFilter
        {
            MEDCORE_EXPORT medAbstractAddFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace divideFilter
        {
            MEDCORE_EXPORT medAbstractDivideFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace gaussianFilter
        {
            MEDCORE_EXPORT medAbstractGaussianFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace invertFilter
        {
            MEDCORE_EXPORT medAbstractInvertFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace medianFilter
        {
            MEDCORE_EXPORT medAbstractMedianFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace multiplyFilter
        {
            MEDCORE_EXPORT medAbstractMultiplyFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace normalizeFilter
        {
            MEDCORE_EXPORT medAbstractNormalizeFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace shrinkFilter
        {
            MEDCORE_EXPORT medAbstractShrinkFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace subtractFilter
        {
            MEDCORE_EXPORT medAbstractSubtractFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace windowingFilter
        {
            MEDCORE_EXPORT medAbstractWindowingFilterProcessPluginFactory& pluginFactory(void);
        }

        namespace imageDenoising
        {
            MEDCORE_EXPORT medAbstractImageDenoisingProcessPluginFactory& pluginFactory(void);
        }

        namespace relaxometryEstimation
        {
            MEDCORE_EXPORT medAbstractRelaxometryEstimationProcessPluginFactory& pluginFactory(void);
        }

        namespace symmetryAlignment
        {
            MEDCORE_EXPORT medAbstractSymmetryPlaneAlignmentProcessPluginFactory& pluginFactory(void);
        }
    }

    namespace morphomathOperation
    {
        MEDCORE_EXPORT medAbstractMorphomathOperationProcessPluginManager& pluginManager(void);
        MEDCORE_EXPORT medAbstractMorphomathOperationProcessPluginFactory& pluginFactory(void);
        MEDCORE_EXPORT void initialize(const QString& path);

        namespace erodeImage
        {
            MEDCORE_EXPORT medAbstractErodeImageProcessPluginFactory& pluginFactory(void);   
        }
        namespace dilateImage
        {
            MEDCORE_EXPORT medAbstractDilateImageProcessPluginFactory& pluginFactory(void);
        }
        namespace openingImage
        {
            MEDCORE_EXPORT medAbstractOpeningImageProcessPluginFactory& pluginFactory(void);
        }
        namespace closingImage
        {
            MEDCORE_EXPORT medAbstractClosingImageProcessPluginFactory& pluginFactory(void);
        }
    }
}
