/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medToolBox.h"
#include "medGuiExport.h"

class medDiffusionSelectorToolBox;
class medDiffusionAbstractToolBoxPrivate;
class dtkAbstractProcess;
class dtkAbstractData;
class dtkPlugin;

class MEDGUI_EXPORT medDiffusionAbstractToolBox : public medToolBox
{
    Q_OBJECT
public:
             medDiffusionAbstractToolBox(QWidget *parentToolBox = 0);
    virtual ~medDiffusionAbstractToolBox();

    virtual void setDiffusionToolBox(medDiffusionSelectorToolBox *toolbox);

    virtual QString description() const=0;

    virtual dtkAbstractData *output() const;

    /**
     * @brief Returns the plugin this custom Toolbox comes from.
     *
     * Used to fill the aboutPlugin button in the parent toolbox.
     *
     */
    virtual dtkPlugin *plugin()const = 0 ;

signals:
     void newOutput(dtkAbstractData * data);

protected slots:
     void processSuccess();

protected:
    medDiffusionSelectorToolBox *parentToolBox();

    medDiffusionAbstractToolBoxPrivate *d;
};


