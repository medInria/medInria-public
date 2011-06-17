#ifndef MEDVIEWCONTAINERCOMPARE_H
#define MEDVIEWCONTAINERCOMPARE_H


#include <medViewContainerCustom.h>


#include "medGuiExport.h"

class medViewContainerComparePrivate;

class MEDGUI_EXPORT medViewContainerCompare : public medViewContainerCustom
{
    Q_OBJECT
public:
    medViewContainerCompare(QWidget * parent = 0);
    ~medViewContainerCompare();
    
signals:
//  fixedViewAdded(dtkAbstractView*);
//  movingViewAdded(dtkAbstractView*);
    void droppedFixed  (const medDataIndex& index);
    void droppedMoving (const medDataIndex &index);
    
private:
    medViewContainerComparePrivate *d3;
};

#endif // MEDVIEWCONTAINERCOMPARE_H
