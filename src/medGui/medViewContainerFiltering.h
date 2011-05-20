#ifndef MEDVIEWCONTAINERFILTERING_H
#define MEDVIEWCONTAINERFILTERING_H

#include <medGui/medViewContainerCustom.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>

#include "medGuiExport.h"

class medViewContainerFilteringPrivate;

class MEDGUI_EXPORT medViewContainerFiltering : public medViewContainerCustom
{
    Q_OBJECT

public:
    medViewContainerFiltering(QWidget * parent = 0);
    ~medViewContainerFiltering();

public slots:

    void updateInput(dtkAbstractData *data);
    void updateOutput(dtkAbstractData *data);

private:
    medViewContainerFilteringPrivate *d3;
};

#endif // MEDVIEWCONTAINERFILTERING_H
