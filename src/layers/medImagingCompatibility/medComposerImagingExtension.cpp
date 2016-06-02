/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerImagingExtension.h>
#include <medToMedImageNode.h>
#include <medToDtkImageNode.h>

medComposerImagingExtension::medComposerImagingExtension(void) : dtkComposerExtension()
{

}

medComposerImagingExtension::~medComposerImagingExtension(void)
{

}

void medComposerImagingExtension::extend(dtkComposerNodeFactory *factory)
{
    factory->record(":/process/medToMedImageNode.json", dtkComposerNodeCreator<medToMedImageNode>);
    factory->record(":/process/medToDtkImageNode.json", dtkComposerNodeCreator<medToDtkImageNode>);
}
