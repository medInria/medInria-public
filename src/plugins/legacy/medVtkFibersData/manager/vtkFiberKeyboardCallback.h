#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkFibersDataPluginExport.h>

#include <vtkCommand.h>

#include <vtkFiberPickerCallback.h>

/**
   vtkFiberKeyboardCallback declaration & implementation
*/
class MEDVTKFIBERSDATAPLUGIN_EXPORT vtkFiberKeyboardCallback : public vtkCommand
{
public:
    static vtkFiberKeyboardCallback*New()
    {
        return new vtkFiberKeyboardCallback;
    }

    virtual void Execute ( vtkObject*, unsigned long, void*);

    void SetFiberPickerCallback (vtkFiberPickerCallback *callback)
    {
        this->FiberPickerCallback = callback;
    }

    void SetFiberManager(vtkFibersManager *man)
    {
        this->FibersManager = man;
    }

protected:
    vtkFiberKeyboardCallback()
    {
        this->FiberPickerCallback = nullptr;
        this->FibersManager       = nullptr;
    }

    ~vtkFiberKeyboardCallback()
    {
    }

private:
    vtkFiberPickerCallback *FiberPickerCallback;
    vtkFibersManager       *FibersManager;
};
