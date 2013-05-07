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

class medCompositeDataSetImporterSelectorToolBox;
class medCompositeDataSetImporterAbstractToolBoxPrivate;
class dtkAbstractProcess;
class dtkAbstractData;

class MEDGUI_EXPORT medCompositeDataSetImporterAbstractToolBox : public medToolBox
{
    Q_OBJECT
public:
             medCompositeDataSetImporterAbstractToolBox(QWidget *parent = 0);
    virtual ~medCompositeDataSetImporterAbstractToolBox();
    virtual void setCompositeDataSetImporterToolBox(medCompositeDataSetImporterSelectorToolBox *toolbox);

    virtual QString description() const = 0;

    virtual dtkAbstractData *output() const;

public slots:
    virtual bool import() = 0;
    virtual void reset()  {};
    virtual void load() {};

protected:
    medCompositeDataSetImporterSelectorToolBox *parent();

protected:
    medCompositeDataSetImporterAbstractToolBoxPrivate *d;
};


