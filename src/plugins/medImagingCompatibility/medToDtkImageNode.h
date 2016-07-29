/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkComposer>

#include <medDataReaderWriter.h>
#include <medAbstractDataConverter.h>
#include <dtkImaging>

#include <medComposerImagingExtensionExport.h>

class medToDtkImageNodePrivate;

class MEDCOMPOSERIMAGINGEXTENSION_EXPORT medToDtkImageNode : public dtkComposerNodeObject<medAbstractDataConverter>
{
public:
     medToDtkImageNode(void);
    ~medToDtkImageNode(void);

public:
    void run(void);

private:
    medToDtkImageNodePrivate *d;
};
