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
#include <medLabelProperty.h>
#include <polygonLabel.h>
#include <medTableWidgetChooser.h>
#include <polygonRoiPluginExport.h>
#include <medViewEventFilter.h>
#include <medDataIndex.h>
#include <medAbstractImageData.h>


class baseViewEvent;
class medContours;
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

    explicit polygonRoiToolBox( QWidget *parent );
    ~polygonRoiToolBox() override;

    static bool registered();
    dtkPlugin* plugin() override;
    medAbstractData *processOutput() override;
    QList<dtkSmartPointer<medAbstractData> > processOutputs() override;
    void drawCross(double *position);
    void eraseCross();

    QPushButton *repulsorTool;

public slots:

    void updateView() override;
    void clickClosePolygon(bool state);
    void activateRepulsor(bool state);
    void interpolateCurve(bool state);
    void showHelp() const;

private slots:

    void disableButtons();
    void saveContours();
    void saveBinaryImage();

    void onLayerRemoveOnOrientedViews(medAbstractData *data);
    void onLayerRemoved(medAbstractData *data);
    void onDataIndexActivated();

    void clear() override;

private:
    QHash<const medAbstractData*, baseViewEvent *> viewEventHash;
    medAbstractData* activeData;
    qint32 specialityPreference;
    medToolBox *pMedToolBox;
    QPushButton *activateTBButton;
    QPushButton *saveBinaryMaskButton;
    QCheckBox *interpolate;
    QPushButton *saveContourButton;
    QPushButton *helpButton;
    QLabel *repulsorLabel;
    QLabel *saveLabel;
    void createAndConnectEventFilter(const medAbstractData *data, medAbstractImageView *imageView);
    QList<medAbstractData *> getITKImageDataInSelectedView(medAbstractView *view);
    QList<medContours *> getContoursInSelectedView(medAbstractView *view);
    void highLightContainer(medAbstractView *pView);

};
