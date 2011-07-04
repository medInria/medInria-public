#include "medToolBoxFunctional.h"

#include "medToolBoxFunctionalCustom.h"
#include <dtkCore/dtkAbstractObject.h>

class medToolBoxFunctionalCustomPrivate
{
public:
    medToolBoxFunctional *parent;
};

medToolBoxFunctionalCustom::medToolBoxFunctionalCustom(QWidget *parent) : medToolBox(parent), d(new medToolBoxFunctionalCustomPrivate)
{
    d->parent  = NULL;
}

medToolBoxFunctionalCustom::~medToolBoxFunctionalCustom(void)
{
    delete d;
    d = NULL;
}

void medToolBoxFunctionalCustom::setFunctionalToolBox(medToolBoxFunctional *toolbox)
{
    d->parent = toolbox;
}

medToolBoxFunctional *medToolBoxFunctionalCustom::parent(void)
{
    return d->parent;
}

dtkAbstractData *medToolBoxFunctionalCustom::output (void) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return NULL;
}
