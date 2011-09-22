/*
 * medViewerToolBoxViewProperties.h
 *
 *  Created on: 15 févr. 2011
 *      Author: aabadie
 */

#ifndef MEDVIEWERTOOLBOXVIEWPROPERTIES_H_
#define MEDVIEWERTOOLBOXVIEWPROPERTIES_H_

#include "medGuiExport.h"
#include <medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class medViewerToolBoxViewPropertiesPrivate;

class MEDGUI_EXPORT medViewerToolBoxViewProperties : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxViewProperties(QWidget *parent = 0);
    ~medViewerToolBoxViewProperties(void);

    void update(dtkAbstractView *view);
    void constructImageLayer(dtkAbstractData* data, int imageLayer);
    void constructMeshLayer(dtkAbstractData* data, int meshLayer);

public slots:
    void onDataAdded(int layer);
    void onDataAdded(dtkAbstractData*);
    void onDataAdded(dtkAbstractData *data, int layer);
    void clear(void);
    void onVisibilitySet(int state);
    void onOpacitySliderSet(int opacity);
    void on2LayersOpacitySliderSet(int opacity);
    void onLUTChanged(int index);
    void onPresetChanged(int index);
    void onItemClicked(QTreeWidgetItem * item, int column);
    void onContextTreeMenu( const QPoint point );
    void onDeleteLayer();
    void onSwitchLayersButtonClicked();
    void onWindowingChanged(bool);
    void onZoomingChanged(bool);
    void onSlicingChanged(bool);
    void onMeasuringChanged(bool);
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



signals:
    void setVisibility(bool visible, int layer);
    void setOpacity(double opacity, int layer);

private:
    medViewerToolBoxViewPropertiesPrivate *d;
    void raiseSlider(bool isVisible, double opacity = 0.5);
    QIcon createIcon(QString colorName);
};

#endif /* MEDVIEWERTOOLBOXVIEWPROPERTIES_H_ */
