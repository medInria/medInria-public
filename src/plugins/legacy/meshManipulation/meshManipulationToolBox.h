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
class vtkDataArray;
class vtkMatrix4x4;
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
    
    medAbstractData *processOutput() override;

    static bool registered();
    dtkPlugin * plugin() override;

    void addTransformationMatrixToUndoStack(vtkMatrix4x4* matrix);

public slots:
    void setupModificationBox();
    void computeData();
    void save();
    void exportTransform();
    void importTransform();
    void updateView() override;
    void resetToolbox();

    void undo();
    void redo();
    void cancel();

protected:
    void clear() override;
    void reset();
    void retrieveRegistrationMatricesFromLayers();

protected slots:
    void handleLayerChange();
    void enableScaling(bool state);
    void enableRotation(bool state);
    void enableTranslation(bool state);
    void checkLayer();
    void applySelectedTransformation();
    void applySelectedTransformationInverted();

private:
    vtkPointSet* transformDataSet(vtkMetaDataSet *dataset,
                                  vtkTransformFilter *transformFilter,
                                  vtkTransform *t);

    bool arrayHoldsRegistrationMatrix(vtkDataArray* array);
    QVector<double> retrieveMatrixFromArray(vtkDataArray* array);
    void tryAddingRegistrationMatrix(const QVector<double>& matrixAsVector, QString registrationName);
    void initializeUndoStack();
    bool buildMatrixFromSelectedTransform(vtkSmartPointer<vtkMatrix4x4> matrix);
    void applyTransformationMatrix(vtkSmartPointer<vtkMatrix4x4> matrix);

private:
    QPointer<medAbstractLayeredView> _view;
    QList<medAbstractData*> _outputs;
    QList<medAbstractData*> _selectedData;

    QCheckBox *_scalingCheckBox;
    QCheckBox *_rotationCheckBox;
    QCheckBox *_translationCheckBox;
    QCheckBox *_modifyAllCheckBox;

    QPushButton *_saveButton;
    QPushButton *_undoButton;
    QPushButton *_redoButton;
    QPushButton *_cancelButton;
    QPushButton *_applyButton;
    QPushButton *_applyInverseButton;
    QPushButton *_exportButton;
    QPushButton *_importButton;
    vtkSmartPointer<vtkBoxWidget> _boxWidget;
    vtkSmartPointer<ManipulationCallback> _callback;

    std::list<std::array<double, 16>> _undoStack;
    std::list<std::array<double, 16>>::iterator _undoStackPos;

    QHash<QString, QVector<double>> _registrationMatrices;
    QListWidget* _availableMatricesWidget;
};
