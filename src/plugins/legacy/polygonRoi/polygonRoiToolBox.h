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

#include <polygonRoi.h>
#include <polygonRoiPluginExport.h>

#include <vtkInriaInteractorStylePolygonRepulsor.h>
#include <vtkPolygon.h>
#include <vtkRenderWindowInteractor.h>

class medAbstractData;
class medAbstractImageView;
class contourWidgetObserver;
class medAbstractImageData;

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
    void onAddNewCurve();
    void activateRepulsor();
    void generateAndSaveBinaryImage();
    void interpolateCurve();

    void showExtractROIWarningPopUp();
    void extractROI();

    void copyContours(); // For the time these function copy and paste all the contours present on a slice. No selection of a contour is possible.
    void pasteContours();
    void undo();
    void redo();

    ListRois interpolateBetween2Polygon(const QList<polygonRoi*> &idAndContour, int firstIndex);

private slots:

    void disableButtons();
    void buttonsStateAtDataOpeningBeforeROI();
    void buttonsStateWhenROI();

protected:

    void generateBinaryImage();
    void binaryImageFromPolygon(QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > polys);
    void reorderPolygon(vtkPolyData * poly);
    QList<vtkPolyData* > generateIntermediateCurves(vtkSmartPointer<vtkPolyData> curve1,vtkSmartPointer<vtkPolyData> curve2, int nb);
    QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > createImagePolygons(QList<QPair<vtkPolyData*,PlaneIndexSlicePair> > &listPoly);

    QList<medSeriesOfRoi*> * getListOfView(medAbstractView *view);
    void resampleCurve(vtkPolyData *poly, int nbPoints);
    void initializeMaskData( medAbstractData *imageData, medAbstractData *maskData ); // copy of a function in painttoolbox
    int computePlaneIndex();
    void clear();

private:

    medAbstractImageView *currentView;
    medToolBox *roiManagementToolBox;
    QPushButton *addNewCurve;
    QPushButton *generateBinaryImage_button;
    QPushButton *repulsorTool;
    QPushButton *interpolate;
    QPushButton *extractRoiButton;

    MapPlaneIndex viewsPlaneIndex;

    QHash<medAbstractView*,contourWidgetObserver*> * hashViewObserver;

    QShortcut *undo_shortcut, *redo_shortcut, *copy_shortcut, *paste_shortcut;

    RoiStatistics stats;

    vtkInriaInteractorStylePolygonRepulsor *interactorStyleRepulsor;

    friend class contourWidgetObserver;

    dtkSmartPointer<medAbstractData> m_maskData; // Output

    UChar3ImageType::Pointer castToUChar3(medAbstractImageData *input);
    void addPointToBuffer(vtkImageView2D* view2d, vtkSmartPointer<vtkPoints> bufferPoints, itk::ImageBase<3>::IndexType point);
    void getContourIndices(vtkImageView2D*, UChar3ImageType::Pointer, UChar3ImageType::IndexType, vtkSmartPointer<vtkPoints>);

    static const int HANDLE_PERIOD  = 5;
};
