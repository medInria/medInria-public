#include "vtkFiberKeyboardCallback.h"
#include <vtkRenderWindowInteractor.h>

void vtkFiberKeyboardCallback::Execute(vtkObject *caller, unsigned long event, void*)
{
    vtkRenderWindowInteractor *rwin = vtkRenderWindowInteractor::SafeDownCast(caller);

    switch (rwin->GetKeyCode())
    {
        case 'h':
        case 'H':
            if (this->FibersManager)
            {
                if (this->FibersManager->GetHelpMessageVisibility())
                {
                    this->FibersManager->HideHelpMessage();
                }
                else
                {
                    this->FibersManager->ShowHelpMessage();
                }
                rwin->Render();
            }
            break;

        case 'b':
            if (this->FibersManager)
            {
                if (this->FibersManager->GetBoxWidgetVisibility())
                {
                    this->FibersManager->BoxWidgetOff();
                }
                else
                {
                    this->FibersManager->BoxWidgetOn();
                }
            }
            break;

        case '4':
            if (this->FibersManager)
            {
                this->FibersManager->SetRenderingModeToPolyLines();
                rwin->Render();
            }
            break;


        case '5':
            if (this->FibersManager)
            {
                this->FibersManager->SetRenderingModeToRibbons();
                rwin->Render();
            }
            break;


        case '6':
            if (this->FibersManager)
            {
                this->FibersManager->SetRenderingModeToTubes();
                rwin->Render();
            }
            break;


        case 'n':
        case 'N':
            if (this->FibersManager)
            {
                this->FibersManager->SwapInputOutput();
                rwin->Render();
            }
            break;


        case 'u':
        case 'U':
            if (this->FibersManager)
            {
                this->FibersManager->Reset();
                rwin->Render();
            }
            break;

        case 'd':
        case 'D':
            if (this->FiberPickerCallback)
            {
                this->FiberPickerCallback->DeletePickedCell();
                rwin->Render();
            }
            break;

        default:
            break;
    }
}
