/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: INRIAviz4D.cxx 1302 2009-10-27 21:57:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-27 21:57:16 +0000 (Tue, 27 Oct 2009) $
Version:   $Revision: 1302 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWApplication.h"
#include "vtkKWMainWindowInteractor.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>

#include <vtkKWToolbar.h>
#include "vtkKWWidgetsPaths.h"
#include "vtkKWSplashScreen.h"
#include <itkObject.h>
#include <kwcommon.h>
#include <string>

#ifdef ITK_USE_REVIEW
#include <itkPhilipsRECImageIOFactory.h>
#endif

// We define several classes in this example, and we want to be able to use
// their C++ methods as callbacks for our user interface. To do so, we created
// a library and wrapped it automatically for the Tcl language, which is used
// as a bridge between C++ objects at run-time. An initialization function is
// automatically created in this library to allow classes and C++ methods to
// be used as commands and callbacks inside the Tcl interpreter; let's *not* 
// forget to declare *and* call this function right after we initialize the Tcl
// interpreter in our application. The name of this function is built from the
// library name in lower-case (except for the first letter) and suffixed with
// "_Init" (for example: KWCallbacksExampleLib => Kwcallbacksexamplelib_Init).
// This whole process is required to use C++ methods as callbacks; it is not
// needed if you use VTK's C++ command/observer pattern directly, which is
// demonstrated in KWWidgets's C++ 'Callbacks' example.

extern "C" int Kwaddon_Init(Tcl_Interp *interp);
extern "C" int Vtkimageviewlib_Init(Tcl_Interp *interp);
extern "C" int Vtkdatamanagementlib_Init(Tcl_Interp *interp);
//extern "C" int Vtkvisumanagementlib_Init(Tcl_Interp *interp);


int my_main(int argc, char *argv[])
{
  // Initialize Tcl

  Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cerr);
  if (!interp)
  {
    cerr << "Error: InitializeTcl failed" << endl ;
    return 1;
  }
  

  // Initialize our Tcl library (i.e. our classes wrapped in Tcl).
  // This *is* required for the C++ methods to be used as callbacks.
  // See comment at the top of this file.
  Vtkdatamanagementlib_Init (interp);
  Vtkimageviewlib_Init (interp);
//   Vtkvisumanagementlib_Init (interp);
  Kwaddon_Init (interp);

  // Process some command-line arguments
  // The --test option here is used to run this example as a non-interactive 
  // test for software quality purposes. You can ignore it.

  int option_test = 0;
  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  args.AddArgument(
    "--test", vtksys::CommandLineArguments::NO_ARGUMENT, &option_test, "");
  args.Parse();
  
  // Create the application
  // Restore the settings that have been saved to the registry, like
  // the geometry of the user interface so far.

  vtkKWApplication *app = vtkKWApplication::New();
  app->SetName("INRIAviz4D");
  
  // Set a help link. Can be a remote link (URL), or a local file
  app->SetHelpDialogStartingPage("http://www-sop.inria.fr/asclepios/software/vtkINRIA3D");
  app->RestoreApplicationSettingsFromRegistry();
  app->SupportSplashScreenOff();
  app->SplashScreenVisibilityOff();

  
#if TK_MAJOR_VERSION==8 && TK_MINOR_VERSION>4
  app->Script("::ttk::setTheme clam");
#endif
  
  // Add our window
  // Set 'SupportHelp' to automatically add a menu entry for the help link  

  
  vtkKWMainWindowInteractor *win = vtkKWMainWindowInteractor::New();
  
  win->UseClassNameInWidgetNameOn();
  win->SetTitle ("INRIAViz4D - (c) INRIA 2007");
  win->SupportHelpOn();
  app->AddWindow(win);
  
  win->Create();
  
  vtkKWToolbar::SetGlobalToolbarAspect(vtkKWToolbar::ToolbarAspectFlat);
  vtkKWToolbar::SetGlobalWidgetsAspect(vtkKWToolbar::WidgetsAspectFlat);
  win->UpdateToolbarState();


  // Start the application
  // If --test was provided, do not enter the event loop and run this example
  // as a non-interactive test for software quality purposes.
  int ret = 0;
  win->Display();


#ifdef ITK_USE_REVIEW
  itk::PhilipsRECImageIOFactory::RegisterOneFactory();
#endif
  
  if (!option_test)
  {
    app->Start(argc, argv);
    ret = app->GetExitStatus();
  }
  // Deallocate and exit
  win->Close();
  app->RemoveWindow(win);
  win->RemoveAllChildren();
  win->Delete();
  while (app->GetReferenceCount() > 1)    
    app->Delete();
  app->Delete();
  
  return ret;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
  int argc;
  char **argv;
  vtksys::SystemTools::ConvertWindowsCommandLineToUnixArguments(
    lpCmdLine, &argc, &argv);
  int ret = my_main(argc, argv);
  for (int i = 0; i < argc; i++) { delete [] argv[i]; }
  delete [] argv;
  return ret;
}
#else
int main(int argc, char *argv[])
{
  return my_main(argc, argv);
}
#endif
