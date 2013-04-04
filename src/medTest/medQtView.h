#pragma once

#include "medTestExport.h"

#include <dtkCore/dtkSmartPointer.h>

#include "medAbstractView.h"

class dtkAbstractData;
class medQtViewPrivate;

class MEDTEST_EXPORT medQtView : public medAbstractView
{
    Q_OBJECT

public:
             medQtView();
    virtual ~medQtView();

    virtual QString description() const;

    static QString s_description();
    static bool registered();

    QWidget *widget();

    void setData (dtkAbstractData *data);
    void setData (dtkAbstractData *data, int layer);

    void *data();
    void *data(int layer);
    virtual medAbstractViewCoordinates * coordinates() {return NULL;}
protected:
private:
    medQtViewPrivate *d;
};


