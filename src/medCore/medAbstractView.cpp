#include "medAbstractView.h"


medAbstractView::medAbstractView(medAbstractView *parent):
dtkAbstractView(parent){

}


medAbstractView::medAbstractView(const medAbstractView& view):
dtkAbstractView(view){

}


void medAbstractView::setColorLookupTable(int min_range,
                                     int max_range,
                                     int size,
                                     const int & table)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::setColorLookupTable(QList<double>scalars,QList<QColor>colors)
{
    DTK_DEFAULT_IMPLEMENTATION;
}



