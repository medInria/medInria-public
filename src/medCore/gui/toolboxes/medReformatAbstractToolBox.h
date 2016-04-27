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
class medReformatSelectorToolBox;
class medReformatAbstractToolBoxPrivate;

//! Base class for custom Reformat algorithms
class MEDCORE_EXPORT medReformatAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
    medReformatAbstractToolBox(QWidget *parent = 0);
    virtual ~medReformatAbstractToolBox();

    virtual dtkPlugin* plugin() = 0;

    virtual medAbstractData *processOutput() = 0;

public slots:

    void updateView(){}

protected:
    medReformatSelectorToolBox *selectorToolBox();

private:
    medReformatAbstractToolBoxPrivate *d;
};


