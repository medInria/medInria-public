/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkCommand.h>

#include <medVtkViewPluginExport.h>

class medVtkView;
class vtkImageView2D;
class vtkImageView3D;

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
        this->m_locked = true;
    }
    inline void unlock()
    {
        this->m_locked = false;
    }

    void Execute (vtkObject *caller, unsigned long event, void *callData);


protected:
    medVtkViewObserver();
    ~medVtkViewObserver();

private:
    bool m_locked;
    medVtkView *m_view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
};
