#ifndef MEDTOOLBOXFIBERVIEW_H
#define MEDTOOLBOXFIBERVIEW_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxFiberViewPrivate;

class MEDGUI_EXPORT medToolBoxFiberView : public medToolBox
{
	Q_OBJECT
public:
	medToolBoxFiberView(QWidget *parent);
	~medToolBoxFiberView();
	
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
	medToolBoxFiberViewPrivate *d;
};

#endif
