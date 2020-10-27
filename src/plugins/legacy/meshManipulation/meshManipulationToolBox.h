#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medAbstractLayeredView.h>
#include <medAbstractSelectableToolBox.h>
#include <meshManipulationPluginExport.h>

#include <vtkSmartPointer.h>

class vtkBoxWidget;
class ManipulationCallback;
class vtkMetaDataSet;
class vtkTransformFilter;
class vtkTransform;
class vtkPointSet;

/*!
 * \brief Toolbox to translate, rotate, manipulate meshes.
 */
class MESHMANIPULATIONPLUGIN_EXPORT meshManipulationToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Mesh Manipulation",
                          "Toolbox to translate/rotate/manipulate meshes.",
                          <<"Meshing")
    
public:
    meshManipulationToolBox(QWidget *parent = nullptr);
    
    medAbstractData *processOutput();

    static bool registered();
    dtkPlugin * plugin();

public slots:
    void setupModificationBox();
    void cancel();
    void computeData();
    void save();
    void exportTransform();
    void importTransform();
    void updateView();
    void resetToolbox();

protected:
    virtual void clear();
    void reset();

protected slots:
    void handleLayerChange();
    void enableScaling(bool state);
    void enableRotation(bool state);
    void enableTranslation(bool state);
    void checkLayer();

private:
    vtkPointSet* transformDataSet(vtkMetaDataSet *dataset,
                                  vtkTransformFilter *transformFilter,
                                  vtkTransform *t);

private:
    QPointer<medAbstractLayeredView> _view;
    QList<medAbstractData*> _outputs;
    QList<medAbstractData*> _selectedData;

    QCheckBox *_scalingCheckBox;
    QCheckBox *_rotationCheckBox;
    QCheckBox *_translationCheckBox;
    QCheckBox *_modifyAllCheckBox;

    QPushButton *_saveButton;
    QPushButton *_cancelButton;
    QPushButton *_exportButton;
    QPushButton *_importButton;
    vtkSmartPointer<vtkBoxWidget> _boxWidget;
    vtkSmartPointer<ManipulationCallback> _callback;
};
