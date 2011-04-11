/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkQtOpenGLRenderWindow.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkQtOpenGLRenderWindow - OpenGL rendering window
// .SECTION Description
// vtkQtOpenGLRenderWindow is a concrete implementation of the abstract
// class vtkRenderWindow. vtkWin32OpenGLRenderer interfaces to the standard
// OpenGL graphics library in the Windows/NT environment..

#ifndef __vtkQtOpenGLRenderWindow_h
#define __vtkQtOpenGLRenderWindow_h

#include <QCursor>
#include <QGLWidget>            // needs to be included before
                                // vtkXOpenGLRenderWindow.h on linux
#include "vtkToolkits.h"        // defines VTK_USE_*

class QVtkGraphicsView;

#if defined( _WIN32 )
    #include <vtkWin32OpenGLRenderWindow.h>
    typedef vtkWin32OpenGLRenderWindow vtkQtOpenGLRenderWindowBase;
#elif defined( VTK_USE_OGLR )
    #include <vtkXOpenGLRenderWindow.h>
    typedef vtkXOpenGLRenderWindow      vtkQtOpenGLRenderWindowBase;
#elif defined (VTK_USE_CARBON)
    #include <vtkCarbonRenderWindow.h>
    typedef vtkCarbonRenderWindow       vtkQtOpenGLRenderWindowBase;
#elif defined (VTK_USE_COCOA)
    #include <vtkCocoaRenderWindow.h>
    typedef vtkCocoaRenderWindow        vtkQtOpenGLRenderWindowBase;
#else
    // VTK_USE_OSMESA and others
    #error Not yet implemented for your system.
#endif

#include <map>


class vtkQtOpenGLRenderWindow : public vtkQtOpenGLRenderWindowBase
{
public:
  static vtkQtOpenGLRenderWindow *New();
  vtkTypeRevisionMacro(vtkQtOpenGLRenderWindow,vtkQtOpenGLRenderWindowBase);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Initialize the rendering window.  This will setup all system-specific
  // resources.  This method and Finalize() must be symmetric and it
  // should be possible to call them multiple times, even changing WindowId
  // in-between.  This is what WindowRemap does.
  // Overrides base class (vtkRenderWindowBase)
  virtual void Initialize(void);

  // Description:
  // Finalize the rendering window.  This will shutdown all system-specific
  // resources.  After having called this, it should be possible to destroy
  // a window that was used for a SetWindowId() call without any ill effects.
  // Overrides base class (vtkRenderWindowBase)
  virtual void Finalize(void);

  // Description:
  // Make this windows OpenGL context the current context.
  // Overrides base class (vtkRenderWindow)
  virtual void MakeCurrent(); 

  void SetQtWidget( QVtkGraphicsView * w);

  // Called by widget when we actually want to paint.
  void NativePaint();

  // Set the cursor, overrides vtkRenderWindow.
  virtual void SetCurrentCursor(int shape);

  //! Get the current cursor in Qt form.
  virtual const QCursor & GetCurrentQtCursor();

  //! Set a Qt cursor to use for the given vtk id.
  virtual void SetQtCursorForVtkCursorId(const QCursor & cursor, int shape);

  //! Get the Qt cursor that will be used for a given id, or a default.
  const QCursor & GetQtCursorForVtkCursorId(int shape) const;

  bool IsInitiatedRepaint() const { return m_initiatedRepaint; }
protected:
  vtkQtOpenGLRenderWindow();
  ~vtkQtOpenGLRenderWindow();

  //!
  // Overrides vtkRenderWindowBase
  void CreateAWindow();

  //!
  // Overrides vtkRenderWindowBase
  void DestroyWindow();

  //! 
  // Overrides vtkRenderer
  void Render();

  virtual void BuildStandardCursors();

protected:
    QVtkGraphicsView * m_qtWidget;
    typedef vtkstd::map< int , QCursor > CursorMapType;
    CursorMapType m_cursorMap;
    bool m_initiatedRepaint; // true when we ask our window to repaint.

private:
  vtkQtOpenGLRenderWindow(const vtkQtOpenGLRenderWindow&);  // Not implemented.
  void operator=(const vtkQtOpenGLRenderWindow&);  // Not implemented.
};


#endif

