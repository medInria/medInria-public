#ifndef MEDTOOLBOXFIBERVIEW_H
#define MEDTOOLBOXFIBERVIEW_H

#include <medGui/medToolBox.h>

class medToolBoxFiberViewPrivate;

class medToolBoxFiberView : public medToolBox
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
