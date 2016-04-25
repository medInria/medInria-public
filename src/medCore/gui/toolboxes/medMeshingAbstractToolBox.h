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
class medMeshingSelectorToolBox;
class medMeshingAbstractToolBoxPrivate;

//! Base class for custom Meshing algorithms
class MEDCORE_EXPORT medMeshingAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
    medMeshingAbstractToolBox(QWidget *parent = 0);
    virtual ~medMeshingAbstractToolBox();

    virtual dtkPlugin* plugin() = 0;

    virtual medAbstractData *processOutput() = 0;

public slots:

    void updateView(){}

protected:
    medMeshingSelectorToolBox *meshingToolBox();

private:
    medMeshingAbstractToolBoxPrivate *d;
};


