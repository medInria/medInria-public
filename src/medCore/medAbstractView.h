
#ifndef MEDABSTRACTVIEW_H
#define MEDABSTRACTVIEW_H
#include <dtkCore/dtkAbstractView.h>
#include "medCoreExport.h"

class MEDCORE_EXPORT medAbstractView: public dtkAbstractView
{
    Q_OBJECT
public:
    medAbstractView(medAbstractView * parent = 0);
    medAbstractView(const medAbstractView& view);
    virtual ~medAbstractView(void){}

    virtual void setColorLookupTable(int min_range,
                                     int max_range,
                                     int size,
                                     const int & table);

    virtual void setColorLookupTable( QList<double> scalars,
				      QList<QColor> colors );
    virtual void setTransferFunctions( QList<double> scalars,
				       QList<QColor> colors );
    virtual void getTransferFunctions( QList<double> & scalars,
				       QList<QColor> & colors );
};
#endif
