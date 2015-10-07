/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerFactoryExtension.h>

//include nodes

#include <medAddImageProcessNode.h>
#include <medSubtractImageProcessNode.h>
#include <medMultiplyImageProcessNode.h>
#include <medDivideImageProcessNode.h>

#include <medDilateImageProcessNode.h>
#include <medErodeImageProcessNode.h>
#include <medOpeningImageProcessNode.h>
#include <medClosingImageProcessNode.h>

medComposerFactoryExtension::medComposerFactoryExtension(void) : dtkComposerNodeFactoryExtension()
{

}

medComposerFactoryExtension::~medComposerFactoryExtension(void)
{

}

void medComposerFactoryExtension::extend(dtkComposerNodeFactory *factory)
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
}
