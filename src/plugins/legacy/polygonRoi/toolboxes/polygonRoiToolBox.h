/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

// medInria
#include <medAbstractSelectableToolBox.h>
#include <medContourSharedInfo.h>
#include <medTagRoiManager.h>
#include <medTableWidgetChooser.h>
#include <polygonRoiPluginExport.h>


class polygonEventFilter;

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
    void onLayerClosed(uint index);
    void clickClosePolygon(bool state);
    void activateRepulsor(bool state);
    void interpolateCurve(bool state);
    void updateTableWidgetView(unsigned int row, unsigned int col);
    void updateTableWidgetItems();

    void manageTick();
    void manageRoisVisibility();
    void enableTableViewChooser(bool state);
    void resetToolboxBehaviour();
    void errorMessage(QString error);
    void checkRepulsor();

private slots:

    void disableButtons();
    void saveContours();
    void saveBinaryImage();

signals:
    void currentLabelsDisplayed();
    void deactivateContours();
protected:

    void binaryImageFromPolygon(QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > polys);
    QList<QPair<vtkPolygon*,PlaneIndexSlicePair> > createImagePolygons(QList<QPair<vtkPolyData*,PlaneIndexSlicePair> > &listPoly);

    void initializeMaskData( medAbstractData *imageData, medAbstractData *maskData ); // copy of a function in painttoolbox
    void clear();

private:

    polygonEventFilter *viewEventFilter;
    QPointer<medAbstractImageView> currentView;
    medToolBox *managementToolBox;
    QUuid mainContainerUUID;

    QPushButton *activateTBButton;
    QPushButton *saveBinaryMaskButton;
    QPushButton *repulsorTool;
    QCheckBox *interpolate;
    medTableWidgetChooser* tableViewChooser;
    QPushButton *saveContourButton;

};
