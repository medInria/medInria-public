/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medAbstractSelectableToolBox.h>
#include <medCreateMeshFromMaskPluginExport.h>

class medCreateMeshFromMaskToolBoxPrivate;

/*!
 * \brief Toolbox to convert a mask to a closed surface mesh.
 */
class MEDCREATEMESHFROMMASKPLUGIN_EXPORT medCreateMeshFromMaskToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Create Mesh from Mask",
                          "Converts a mask to a closed surface mesh.",
                          <<"Meshing")
    
public:
    medCreateMeshFromMaskToolBox(QWidget *parent = nullptr);
    ~medCreateMeshFromMaskToolBox();
    
    medAbstractData *processOutput();
    
    static bool registered();
    dtkPlugin *plugin();
    
    void clear();

signals:
    void success();
    void failure();
    
public slots:
    void run();
    void displayNumberOfTriangles();
    void updateView();
    
protected slots :
    void addMeshToView();

private:
    medCreateMeshFromMaskToolBoxPrivate *d;
};


