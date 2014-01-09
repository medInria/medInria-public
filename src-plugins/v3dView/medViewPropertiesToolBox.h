/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "v3dViewPluginExport.h"
#include <medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class medViewPropertiesToolBoxPrivate;
/**

   \class medViewPropertiesToolBox
   \brief This is designed to help users manipulate the image or mesh data. It can be put into the specific workspace as the other toolboxes

   \ingroup Toolboxes
   \author Alexandre Abadie and Fatih Arslan, INRIA
*/
class V3DVIEWPLUGIN_EXPORT medViewPropertiesToolBox : public medToolBox
{
    Q_OBJECT

public:
     medViewPropertiesToolBox(QWidget *parent = 0);
    ~medViewPropertiesToolBox();
    /**
    Whenever the view is changed, update metdhod is triggered.
    */
    void update(dtkAbstractView *view);
    void constructImageLayer(dtkAbstractData* data, int imageLayer);
    void constructMeshLayer(dtkAbstractData* data, int meshLayer);

    /**
     * @brief Checks the interaction button, according to the medSettings
     *
     * Uses the medSettingsManager to push the right interaction button;
     */
    void setCurrentInteractionFromSettings();

    static bool registered();

public slots:
    void onDataAdded(int layer);
    void onDataAdded(dtkAbstractData*);
    void onDataAdded(dtkAbstractData *data, int layer);
    void clear();
    void onVisibilitySet(int state);
    void onOpacitySliderSet(int opacity);
    void on2LayersOpacitySliderSet(int opacity);
    void onLUTChanged(int index);
    void onPresetChanged(int index);
    void onItemClicked(QTreeWidgetItem * item);
    void onContextTreeMenu( const QPoint point );
    void onDeleteLayer();
    void onSwitchLayersButtonClicked();
    void onWindowingChanged(bool);
    void onZoomingChanged(bool);
    void onSlicingChanged(bool);
    void onMeasuringChanged(bool);

    void onAxialChanged(bool);
    void onSagittalChanged(bool);
    void onCoronalChanged(bool);
    void onView3DChanged(bool);

    void on2DTriggered(dtkAbstractView* d);
    void on3DTriggered(dtkAbstractView* d);

    void onColorSelected(const QColor& color);

    void onScalarBarVisibilityChanged(bool);
    void onAxisVisibilityChanged(bool);
    void onRulerVisibilityChanged(bool);
    void onAnnotationsVisibilityChanged(bool);


    void onAttrBoxChanged(int index);
    void onEdgeVisibilitySet(int state);
    void onRenderingChanged (int state);
    void onColorChanged(int selection);

    void onModeChanged(QString mode);
    void onVRModeChanged(QString mode);
    void onLodChanged(int value);
    void onCroppingChanged(bool checked);
    void onDepthPeelingChanged(bool checked);

signals:
    /** setVisibility is both for images and meshes*/
    void setVisibility(bool visible, int layer);
    /** setVisibility is both for images and meshes*/
    void setOpacity(double opacity, int layer);

private:
    medViewPropertiesToolBoxPrivate *d;
    void raiseSlider(bool isVisible, double opacity = 0.5);
    QIcon createIcon(QString colorName);
};


