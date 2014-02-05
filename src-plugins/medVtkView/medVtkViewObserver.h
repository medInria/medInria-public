#pragma once

#include <vtkCommand.h>

#include <medVtkViewPluginExport.h>

class medVtkView;
class vtkImageView2D;
class vtkImageView3D;
class medVtkViewSignalsEmitter;

class MEDVTKVIEWPLUGIN_EXPORT medVtkViewObserver : public vtkCommand
{

public:

    static medVtkViewObserver* New()
    {
        return new medVtkViewObserver;
    }

    void setView (medVtkView *view);

    inline void   lock()
    {
        this->m_lock = 1;
    }
    inline void unlock()
    {
        this->m_lock = 0;
    }

    void Execute (vtkObject *caller, unsigned long event, void *callData);


protected:
    medVtkViewObserver();
    ~medVtkViewObserver();

private:
    int m_lock;
    medVtkView *m_view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    medVtkViewSignalsEmitter *emitter;
};
