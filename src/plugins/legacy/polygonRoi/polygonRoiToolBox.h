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
//    CURSORSTATE getCursorState(){return cursorState;}

public slots:
//    void setCursorState(CURSORSTATE state){cursorState = state;}

    void updateView();
    void onViewClosed();
    void onLayerClosed();
    void clickClosePolygon(bool state);
    void activateRepulsor(bool state);
    void generateAndSaveBinaryImage();
    void interpolateCurve(bool state);
    void updateTableWidgetView(unsigned int row, unsigned int col);
    void updateTableWidgetItems();

    void showExtractROIWarningPopUp();
    void extractROI();

    void copyContours(); // For the time these function copy and paste all the contours present on a slice. No selection of a contour is possible.
    void pasteContours();
    void undo();
    void redo();
    void manageTick();
    void manageRoisVisibility();
private slots:

    void disableButtons();
    void buttonsStateWhenROI();

protected:

    void generateBinaryImage();
    void binaryImageFromPolygon(QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > polys);
    QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > createImagePolygons(QList<QPair<vtkPolyData*,PlaneIndexSlicePair> > &listPoly);

    QList<medSeriesOfRoi*> * getListOfView(medAbstractView *view);
    void initializeMaskData( medAbstractData *imageData, medAbstractData *maskData ); // copy of a function in painttoolbox
    int computePlaneIndex();
    void clear();

private:

    polygonEventFilter *viewEventFilter;
//    CURSORSTATE cursorState;
    medAbstractImageView *currentView;
    medToolBox *roiManagementToolBox;
    QPushButton *addNewCurve;
    QPushButton *generateBinaryImage_button;
    QPushButton *repulsorTool;
    QPushButton *splitViewButton;
    QCheckBox *interpolate;
    QPushButton *extractRoiButton;
    medTableWidgetChooser* tableViewChooser;

    MapPlaneIndex viewsPlaneIndex;

    QHash<medAbstractView*,contourWidgetListener*> * hashViewObserver;
    contourWidgetListener* contourObserver;

    QShortcut *undo_shortcut, *redo_shortcut, *copy_shortcut, *paste_shortcut;

    RoiStatistics stats;

    friend class contourWidgetListener;

    dtkSmartPointer<medAbstractData> m_maskData; // Output

    dtkSmartPointer<medImageMaskAnnotationData> m_maskAnnotationData;
    dtkSmartPointer<medAbstractData> medPolyData;

    UChar3ImageType::Pointer castToUChar3(medAbstractImageData *input);
    void addPointToBuffer(vtkImageView2D* view2d, vtkSmartPointer<vtkPoints> bufferPoints, itk::ImageBase<3>::IndexType point);
    void getContourIndices(vtkImageView2D*, UChar3ImageType::Pointer, UChar3ImageType::IndexType, vtkSmartPointer<vtkPoints>);

    static const int HANDLE_PERIOD  = 5;
};
