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
    void onDataSelected(int index);
    void onVisibilitySet(int state);

signals:
    void setVisibility(bool visible, int layer);

private:
    medViewerToolBoxViewPropertiesPrivate *d;

};

#endif /* MEDVIEWERTOOLBOXVIEWPROPERTIES_H_ */
