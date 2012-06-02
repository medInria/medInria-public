#ifndef MEDQTVIEW_H
#define MEDQTVIEW_H

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

    virtual QString description(void) const;

    static QString s_description();
    static bool registered();

    QWidget *widget(void);

    void setData (dtkAbstractData *data);
    void setData (dtkAbstractData *data, int layer);

    void *data(void);
    void *data(int layer);

protected:
private:
    medQtViewPrivate *d;
};

#endif
