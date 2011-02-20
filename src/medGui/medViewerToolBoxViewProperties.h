/*
 * medViewerToolBoxViewProperties.h
 *
 *  Created on: 15 févr. 2011
 *      Author: aabadie
 */

#ifndef MEDVIEWERTOOLBOXVIEWPROPERTIES_H_
#define MEDVIEWERTOOLBOXVIEWPROPERTIES_H_

#include <medGui/medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class medViewerToolBoxViewPropertiesPrivate;

class medViewerToolBoxViewProperties : public medToolBox
{
    Q_OBJECT

public:
     medViewerToolBoxViewProperties(QWidget *parent = 0);
    ~medViewerToolBoxViewProperties(void);

    void update(dtkAbstractView *view);

public slots:
    void onDataAdded(int layer);
    void onViewClosed(void);
    void onVisibilitySet(int state);
    void onOpacitySet(double opacity);
    void onOpacitySliderSet(int opacity);
    void onLUTChanged(int index);
    void onItemPressed(QTreeWidgetItem * item, int column);

signals:
    void setVisibility(bool visible, int layer);
    void setOpacity(double opacity, int layer);

private:
    medViewerToolBoxViewPropertiesPrivate *d;

};

#endif /* MEDVIEWERTOOLBOXVIEWPROPERTIES_H_ */
