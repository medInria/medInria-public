#pragma once
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
        this->FiberPickerCallback = 0;
        this->FibersManager       = 0;
    }

    ~vtkFiberKeyboardCallback()
    {
    }

private:
    vtkFiberPickerCallback *FiberPickerCallback;
    vtkFibersManager       *FibersManager;
};
