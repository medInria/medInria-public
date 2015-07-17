/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSubtractImageProcessComposerFactoryExtension.h>

#include <medSubtractImageProcessNode.h>


// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

medSubtractImageProcessComposerFactoryExtension::medSubtractImageProcessComposerFactoryExtension(void) : dtkComposerNodeFactoryExtension()
{

}

medSubtractImageProcessComposerFactoryExtension::~medSubtractImageProcessComposerFactoryExtension(void)
{

}

void medSubtractImageProcessComposerFactoryExtension::extend(dtkComposerNodeFactory *factory)
{
    factory->record("/home/rdebroiz/medinria2/src/src/medCore/process/arithmetic_operation/medSubtractImageProcessNode.json", dtkComposerNodeCreator<medSubtractImageProcessNode>);
}

