/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <itkImage.h>

#include <medAbstractRoi.h>
#include <medAbstractSelectableToolBox.h>
#include <medRoiManager.h>
#include <medTableWidgetChooser.h>

#include <polygonRoi.h>
#include <polygonRoiPluginExport.h>

#include <medImageMaskAnnotationData.h>
#include <vtkInriaInteractorStylePolygonRepulsor.h>
#include <vtkPolygon.h>
#include <vtkRenderWindowInteractor.h>

class medAbstractData;
class medAbstractImageView;
class contourWidgetListener;
class medAbstractImageData;
class polygonEventFilter;

typedef itk::Image<unsigned char, 3> UChar3ImageType;

/*! \brief Toolbox to create, use, interpolate Polygon ROI.
 *
 * This toolbox has several named widgets which can be accessed in python pipelines:\n\n
 * "closedPolygonButton" : QPushButton\n
 * "interpolateButton" : QPushButton\n
 * "extractRoiButton" : QPushButton\n
 * "repulsorTool" : QPushButton\n
 * "generateBinaryImageButton" : QPushButton
 */
class POLYGONROIPLUGIN_EXPORT polygonRoiToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT

    MED_TOOLBOX_INTERFACE("Polygon Roi",
                          "Used to create Polygon ROI",
                          <<"Segmentation")

public:

    static const char *generateBinaryImageButtonName;

    typedef QPair<unsigned int,unsigned int> PlaneIndexSlicePair;
    typedef QList<QPair<vtkSmartPointer<vtkContourWidget> , PlaneIndexSlicePair> > listOfPair_CurveSlice;
    typedef QList<medAbstractRoi*> ListRois;
    typedef QHash<medAbstractView*,QList<int> *> MapPlaneIndex;

    polygonRoiToolBox( QWidget *parent );
    ~polygonRoiToolBox();

    static bool registered();
    dtkPlugin* plugin();
    medAbstractData *processOutput();
    void setCurrentView(medAbstractView *view);

public slots:

    void updateView();
    void onViewClosed();
    void onLayerClosed();
    void clickClosePolygon(bool state);
    void activateRepulsor(bool state);
    void generateAndSaveBinaryImage();
    void interpolateCurve(bool state);
    void updateTableWidgetView(unsigned int row, unsigned int col);
    void updateTableWidgetItems();

    void manageTick();
    void manageRoisVisibility();
    void enableRepulsorButton(bool state);
    void enableGenerateMaskButton(bool state);
    void enableTableViewChooser(bool state);
private slots:

    void disableButtons();
    void toggleRepulsorButton(bool state);

protected:

    void binaryImageFromPolygon(QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > polys);
    QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > createImagePolygons(QList<QPair<vtkPolyData*,PlaneIndexSlicePair> > &listPoly);

    QList<medSeriesOfRoi*> * getListOfView(medAbstractView *view);
    void initializeMaskData( medAbstractData *imageData, medAbstractData *maskData ); // copy of a function in painttoolbox
    void clear();

private:

    polygonEventFilter *viewEventFilter;
    medAbstractImageView *currentView;
    QPushButton *addNewCurve;
    QPushButton *generateBinaryImage_button;
    QPushButton *repulsorTool;
    QCheckBox *interpolate;
    medTableWidgetChooser* tableViewChooser;
};
