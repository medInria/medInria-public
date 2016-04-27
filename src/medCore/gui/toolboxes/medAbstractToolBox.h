/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>

class medAbstractData;
class medSelectorToolBox;
class medAbstractToolBoxPrivate;

//! Base class for custom algorithms
class MEDCORE_EXPORT medAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
    medAbstractToolBox(QWidget *parent = 0);
    virtual ~medAbstractToolBox();

    virtual dtkPlugin* plugin() = 0;

    virtual medAbstractData *processOutput() = 0;

    virtual void updateType(medAbstractData *data){}

    void setSelectorToolBox(medSelectorToolBox *toolbox);

public slots:

    void updateView(){}

protected:
    medSelectorToolBox *selectorToolBox();

private:
    medAbstractToolBoxPrivate *d;
};


