/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <medAbstractProcessLegacy.h>

class medAbstractData;

class MEDCORE_EXPORT medAbstractDiffusionProcess : public medAbstractProcessLegacy
{
    Q_OBJECT

public:
    medAbstractDiffusionProcess(medAbstractProcessLegacy *parent = NULL);
    virtual ~medAbstractDiffusionProcess();

    virtual void setInputImage(medAbstractData *data) = 0;

};
