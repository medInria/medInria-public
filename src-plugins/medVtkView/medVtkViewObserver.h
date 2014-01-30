#pragma once

#include <vtkCommand.h>

#include <v3dViewPluginExport.h>

class QSlider;
class medVtkView;

class V3DVIEWPLUGIN_EXPORT medVtkViewObserver : public vtkCommand
{
public:
    static medVtkViewObserver* New()
    {
        return new medVtkViewObserver;
    }

    void setSlider (QSlider *slider)
    {
        this->m_slider = slider;
    }

    void setView (medVtkView *view)
    {
        this->m_view = view;
    }

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
    QSlider *m_slider;
    medVtkView *m_view;
};
