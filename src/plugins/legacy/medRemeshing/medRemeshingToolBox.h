/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractSelectableToolBox.h>
#include <medRemeshingPluginExport.h>

class medRemeshingToolBoxPrivate;

/*! \brief Toolbox to refine, decimate, smooth, change the number of polygons in a mesh.
 *
 * This toolbox has several named widgets which can be accessed in python pipelines:\n\n
 * "topologyRadioButton" : QRadioButton\n
 * "resetButton" : QPushButton
 */
class MEDREMESHINGPLUGIN_EXPORT medRemeshingToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Remeshing",
                          "Tools for refining/decimating meshes.",
                          <<"Meshing")
public:
    enum ProcessName
    {
             REFINE,
             DECIMATE,
             SMOOTH,
             MANUAL
    };

    medRemeshingToolBox(QWidget *parent = nullptr);
    ~medRemeshingToolBox();

    medAbstractData *processOutput();

    static bool registered();
    dtkPlugin *plugin();

    void displayNewNumberOfCells(dtkSmartPointer<medAbstractData> output);
    void displayChanges(dtkSmartPointer<medAbstractData> output);
    double getNumberOfCells(dtkSmartPointer<medAbstractData> data);
    void displayComputedMesh(dtkSmartPointer<medAbstractData> output);
    void decimateMesh(double factor, dtkSmartPointer<medAbstractData> data = nullptr);
    void refineMesh(double factor, dtkSmartPointer<medAbstractData> data = nullptr);

signals:
    void success();
    void failure();

public slots:
    void reset();
    void launchDecimate();
    void launchRefine();
    void imposeNbOfTriangles();
    void updateView();
    void clearAndUpdate();
    void smoothMesh();

protected slots :
    void clear();
    void addMeshToView();
    void cantDecimateMore();
    void allowDecimate();

    //! Decimation is allowed if the topology button is uncheck
    void allowDecimateIfTopologyButtonUnchecked(bool checked);
    void disableButtons(bool val);
    void displayCastFailure();

private:
    medRemeshingToolBoxPrivate *d;
};
