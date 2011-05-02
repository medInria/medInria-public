/*
 * medViewerToolBoxViewProperties.h
 *
 *  Created on: 15 févr. 2011
 *      Author: aabadie
 */

#ifndef MEDVIEWERTOOLBOXVIEWPROPERTIES_H_
#define MEDVIEWERTOOLBOXVIEWPROPERTIES_H_

#include "medGuiExport.h"
#include <medGui/medToolBox.h>

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
    
public slots:
    void onDataAdded(int layer);
    void onDataAdded(dtkAbstractData*);
    void onDataAdded(dtkAbstractData *data, int layer);
    void onViewClosed(void);
    void onVisibilitySet(int state);
    void onOpacitySliderSet(int opacity);
    void on2LayersOpacitySliderSet(int opacity);
    void onLUTChanged(int index);
    void onItemClicked(QTreeWidgetItem * item, int column);
    void onContextTreeMenu( const QPoint point );
    void onDeleteLayer();
    void onSwitchLayersButtonClicked();
    void clear();
    
    
    void onColorSelected(const QColor& color);
   
    
    void onMeshLUTChanged (int value);

    void onEdgeVisibilitySet(int state);
    void onRenderingChanged (int state);
    void on_comboBox_currentIndexChanged(int selection);
signals:
    void setVisibility(bool visible, int layer);
    void setOpacity(double opacity, int layer);

private:
    medViewerToolBoxViewPropertiesPrivate *d;
    void raiseSlider(bool isVisible, double opacity = -1.0);
    QIcon createIcon(QString colorName);
};

#endif /* MEDVIEWERTOOLBOXVIEWPROPERTIES_H_ */
