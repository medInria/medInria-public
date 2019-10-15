/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractSelectableToolBox.h>

#include <voiCutterPluginExport.h>
#include <vtkVector.h>
#include <vtkPolygon.h>

class vtkCutterObserver;
class voiCutterToolBoxPrivate;

enum MODE{Keep, Remove, Restore};

/*! \brief Toolbox to cut a VOI (Volume of Interest).
 *
 * This toolbox has several named widgets which can be accessed in python pipelines:\n\n
 * "Cut Volume" : QPushButton\n
 * "Save Image" : QPushButton
 */
class VOICUTTERPLUGIN_EXPORT voiCutterToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT

    MED_TOOLBOX_INTERFACE("VOI Cutter",
                          "Used to cut a VOI",
                          <<"Segmentation")
public:

    voiCutterToolBox(QWidget *parent = nullptr);
    ~voiCutterToolBox();
    
    static bool registered();
    
    void launchTheCutting(MODE m);

    dtkPlugin* plugin();
    medAbstractData *processOutput();

public slots:
    void activateButtons(bool param);
    void activateScissor(bool checked);
    void saveImage();
    void updateView();
    void onViewClosed();
    void switchToVR(bool checked);
    void adaptWidgetsToOrientationChange();

protected:
    void definePolygonsImage(std::vector<vtkVector2i>, MODE);
    int intersect3D_SegmentPlane(float *P0, float *P1, float *Pnormal, float *Ppoint,
                                 double* resultPt);

    template <typename IMAGE>
    void cutThroughImage(QList<vtkPolygon*>*RoiList, QList<vtkPoints*>*RoiPointList,
                         long stackMax, unsigned int stackOrientation, MODE m);

    template <typename IMAGE>
    void fillPolygon(IMAGE *img, int nbPoints, int *polyX,int *polyY,
                     int start_col, int end_col, int start_row, int end_row,
                     int stack, int stackOrientation);

    void saveRenderingParameters();
    void applyRenderingParameters();
    void fillOutputMetaData();

    virtual void clear();

private:
    friend class vtkCutterObserver;
    voiCutterToolBoxPrivate *d;
};
