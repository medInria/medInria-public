/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerExtension.h>

#include <medAddImageProcessNode.h>
#include <medSubtractImageProcessNode.h>
#include <medMultiplyImageProcessNode.h>
#include <medDivideImageProcessNode.h>

#include <medDilateImageProcessNode.h>
#include <medErodeImageProcessNode.h>
#include <medOpeningImageProcessNode.h>
#include <medClosingImageProcessNode.h>

#include <medAddFilterProcessNode.h>
#include <medDivideFilterProcessNode.h>
#include <medGaussianFilterProcessNode.h>
#include <medInvertFilterProcessNode.h>
#include <medMedianFilterProcessNode.h>
#include <medMultiplyFilterProcessNode.h>
#include <medNormalizeFilterProcessNode.h>
#include <medShrinkFilterProcessNode.h>
#include <medSubtractFilterProcessNode.h>
#include <medWindowingFilterProcessNode.h>
#include <medImageDenoisingProcessNode.h>

#include <medMaskImageProcessNode.h>

#include <medDiffusionModelEstimationProcessNode.h>
#include <medDiffusionScalarMapsProcessNode.h>
#include <medTractographyProcessNode.h>
#include <medDWIMaskingProcessNode.h>

#include <medReaderNodeBase.h>
#include <medWriterNodeBase.h>

medComposerExtension::medComposerExtension(void) : dtkComposerExtension()
{

}

medComposerExtension::~medComposerExtension(void)
{

}

void medComposerExtension::extend(dtkComposerNodeFactory *factory)
{
    factory->record(":/process/arithmetic_operation/medAbstractAddImageProcess.json",
                    dtkComposerNodeCreator<medAddImageProcessNode>);
    factory->record(":/process/arithmetic_operation/medAbstractSubtractImageProcess.json",
                    dtkComposerNodeCreator<medSubtractImageProcessNode>);
    factory->record(":/process/arithmetic_operation/medAbstractMultiplyImageProcess.json",
                    dtkComposerNodeCreator<medMultiplyImageProcessNode>);
    factory->record(":/process/arithmetic_operation/medAbstractDivideImageProcess.json",
                    dtkComposerNodeCreator<medDivideImageProcessNode>);

    factory->record(":/process/morphomath_operation/medAbstractErodeImageProcess.json",
                    dtkComposerNodeCreator<medErodeImageProcessNode>);
    factory->record(":/process/morphomath_operation/medAbstractDilateImageProcess.json",
                    dtkComposerNodeCreator<medDilateImageProcessNode>);
    factory->record(":/process/morphomath_operation/medAbstractOpeningImageProcess.json",
                    dtkComposerNodeCreator<medOpeningImageProcessNode>);
    factory->record(":/process/morphomath_operation/medAbstractClosingImageProcess.json",
                    dtkComposerNodeCreator<medClosingImageProcessNode>);

    factory->record(":/process/single_filter/medAbstractAddFilterProcess.json",
                    dtkComposerNodeCreator<medAddFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractDivideFilterProcess.json",
                    dtkComposerNodeCreator<medDivideFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractGaussianFilterProcess.json",
                    dtkComposerNodeCreator<medGaussianFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractInvertFilterProcess.json",
                    dtkComposerNodeCreator<medInvertFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractMedianFilterProcess.json",
                    dtkComposerNodeCreator<medMedianFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractMultiplyFilterProcess.json",
                    dtkComposerNodeCreator<medMultiplyFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractNormalizeFilterProcess.json",
                    dtkComposerNodeCreator<medNormalizeFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractShrinkFilterProcess.json",
                    dtkComposerNodeCreator<medShrinkFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractSubtractFilterProcess.json",
                    dtkComposerNodeCreator<medSubtractFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractWindowingFilterProcess.json",
                    dtkComposerNodeCreator<medWindowingFilterProcessNode>);
    factory->record(":/process/single_filter/medAbstractImageDenoisingProcess.json",
                    dtkComposerNodeCreator<medImageDenoisingProcessNode>);

    factory->record(":/process/mask_image/medAbstractMaskImageProcess.json",
                    dtkComposerNodeCreator<medMaskImageProcessNode>);

    factory->record(":/process/diffusion_processes/medAbstractDiffusionModelEstimationProcess.json",
                    dtkComposerNodeCreator<medDiffusionModelEstimationProcessNode>);
    factory->record(":/process/diffusion_processes/medAbstractDiffusionScalarMapsProcess.json",
                    dtkComposerNodeCreator<medDiffusionScalarMapsProcessNode>);
    factory->record(":/process/diffusion_processes/medAbstractDWIMaskingProcess.json",
                    dtkComposerNodeCreator<medDWIMaskingProcessNode>);
    factory->record(":/process/diffusion_processes/medAbstractTractographyProcess.json",
                    dtkComposerNodeCreator<medTractographyProcessNode>);

    factory->record(":/process/medGenericReaderNode.json", dtkComposerNodeCreator<medGenericReaderNode>);
    factory->record(":/process/medImageReaderNode.json", dtkComposerNodeCreator<medImageReaderNode>);
    factory->record(":/process/medMeshReaderNode.json", dtkComposerNodeCreator<medMeshReaderNode>);
    factory->record(":/process/medGenericWriterNode.json", dtkComposerNodeCreator<medWriterNodeBase>);
}
