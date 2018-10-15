/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medClutPluginExport.h"
#include <medToolBox.h>

#include <medAbstractWorkspace.h>

class dtkAbstractView;
class dtkAbstractData;
class medClutToolBoxPrivate;

/*!
 * \brief Clut (colour look-up table) Editor toolBox.
 */
class MEDCLUTPLUGIN_EXPORT medClutToolBox : public medToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Clut Editor ToolBox","Clut toolbox",<<"properties")

public:
     medClutToolBox(QWidget *parent = 0);
    ~medClutToolBox();

    static bool registered();

public slots:

    void changeLut(int);
    void ApplyLut();
    void updateRange();
    void updateRange(double min,double max);
    void changeAttribute(int);
    void exportMeshWithLUT();
    void updateView();

signals:

private:
    medClutToolBoxPrivate *d;
};


