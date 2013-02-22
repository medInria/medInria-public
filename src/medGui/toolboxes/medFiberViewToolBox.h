#ifndef medFiberViewToolBox_H
#define medFiberViewToolBox_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medFiberViewToolBoxPrivate;

class MEDGUI_EXPORT medFiberViewToolBox : public medToolBox
{
    Q_OBJECT
public:
    medFiberViewToolBox(QWidget *parent);
    ~medFiberViewToolBox();

signals:
    void fiberRadiusSet(int);
    void fiberColorModeChanged(int);
    void GPUActivated(bool);
    void lineModeSelected(bool);
    void ribbonModeSelected(bool);
    void tubeModeSelected(bool);

public:

    /* Returns the selected fiber radius. */
    int fiberRadius();

    /* Returns the selected fiber color mode. */
    int fiberColorMode();

    /* Returns whether the GPU acceleration is activated. */
    bool isGPUActivated();

    /* Returns whether line mode is selected or not. */
    bool isLineModeSelected();

    /* Returns whether ribbon mode is selected or not. */
    bool isRibbonModeSelected();

    /* Returns whether tubes mode is selected or not. */
    bool isTubesModeSelected();


public slots:
    void update (dtkAbstractView *view);

protected:
    medFiberViewToolBoxPrivate *d;
};

#endif
