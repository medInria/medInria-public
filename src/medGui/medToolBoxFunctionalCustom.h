#ifndef MEDTOOLBOXFUNCTIONALCUSTOM_H
#define MEDTOOLBOXFUNCTIONALCUSTOM_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medToolBoxFunctional;
class medToolBoxFunctionalCustomPrivate;
class dtkAbstractProcess;
class dtkAbstractData;

class MEDGUI_EXPORT medToolBoxFunctionalCustom : public medToolBox
{
    Q_OBJECT
public:
             medToolBoxFunctionalCustom(QWidget *parent = 0);
    virtual ~medToolBoxFunctionalCustom(void);

    virtual void setFunctionalToolBox(medToolBoxFunctional *toolbox);

    virtual QString description (void) const = 0;

    virtual dtkAbstractData *output (void) const;

    virtual void setData(dtkAbstractData* data) = 0;

protected:
    medToolBoxFunctional *parent(void);

private:
    medToolBoxFunctionalCustomPrivate *d;
};

#endif // MEDTOOLBOXFUNCTIONALCUSTOM_H
