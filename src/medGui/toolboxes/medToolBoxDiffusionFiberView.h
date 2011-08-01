#ifndef MEDTOOLBOXDIFFUSIONFIBERVIEW_H
#define MEDTOOLBOXDIFFUSIONFIBERVIEW_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medToolBoxDiffusionFiberViewPrivate;

class MEDGUI_EXPORT medToolBoxDiffusionFiberView : public medToolBox
{
    Q_OBJECT
public:
    medToolBoxDiffusionFiberView(QWidget *parent);
    ~medToolBoxDiffusionFiberView();
    
signals:
    void fiberRadiusSet(int);
    void fiberColorModeChanged(int);
    void GPUActivated(bool);
    void lineModeSelected(bool);
    void ribbonModeSelected(bool);
    void tubeModeSelected(bool);

public slots:
    void update (dtkAbstractView *view);
    
protected:
    medToolBoxDiffusionFiberViewPrivate *d;
};

#endif
