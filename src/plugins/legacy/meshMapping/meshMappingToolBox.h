#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medAbstractSelectableToolBox.h>
#include <meshMappingPluginExport.h>

class meshMappingToolBoxPrivate;

/*!
 * \brief Toolbox to map data on a mesh: keep the intersection of the mesh and data, and set these values to the mesh.
 */
class MESHMAPPINGPLUGIN_EXPORT meshMappingToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Mesh Mapping",
                          "Map data on a mesh: keep the intersection of the mesh and data, and set these values to the mesh",
                          <<"Meshing")
    
public:
    meshMappingToolBox(QWidget *parent = nullptr);
    ~meshMappingToolBox();
    
    medAbstractData *processOutput();
    
    static bool registered();
    dtkPlugin *plugin();
    
signals:
    void success();
    void failure();
    
public slots:
    void run();
    void updateView();

protected slots:
    void addLayer(unsigned int layer);
    void resetComboBoxes();

private:
    meshMappingToolBoxPrivate *d;
};


