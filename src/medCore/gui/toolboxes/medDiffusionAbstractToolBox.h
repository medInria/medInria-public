/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medToolBox.h"
#include "medCoreExport.h"

class medAbstractDiffusionProcess;
class dtkPlugin;

class MEDCORE_EXPORT medDiffusionAbstractToolBox : public medToolBox
{
    Q_OBJECT
public:
    medDiffusionAbstractToolBox(QWidget *parentToolBox = 0);
    virtual ~medDiffusionAbstractToolBox();

    /**
     * @brief Returns the plugin this custom Toolbox comes from.
     *
     * Used to fill the aboutPlugin button in the parent toolbox.
     *
     */
    virtual dtkPlugin *plugin()const = 0;
    
    virtual QString processName() = 0;
    virtual void setProcessParameters(medAbstractDiffusionProcess *process) = 0;
};

class MEDCORE_EXPORT medDiffusionScalarMapsAbstractToolBox : public medDiffusionAbstractToolBox
{
    Q_OBJECT
public:
    medDiffusionScalarMapsAbstractToolBox(QWidget *parentToolBox = 0);
    virtual ~medDiffusionScalarMapsAbstractToolBox();
    
signals:
    void processStartRequested();
};
