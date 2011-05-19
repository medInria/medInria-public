/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkWin32OpenGLRenderWindow.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkQtOpenGLRenderWindow.h"

#include <dtkCore/dtkLog.h>

#include <QApplication>
#include <QCursor>
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QVtkGraphicsView.h>
#include <QVTKWidget.h>

#include <vtkgl.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>

vtkCxxRevisionMacro(vtkQtOpenGLRenderWindow, "0");
vtkStandardNewMacro(vtkQtOpenGLRenderWindow);

vtkQtOpenGLRenderWindow::vtkQtOpenGLRenderWindow() :
  m_qtWidget (NULL),
  m_initiatedRepaint(false),
  InitializedOpenGl(false),
  IsCursorHidden(false)
{
  BuildStandardCursors ();
  typedef vtkSmartPointer< QVTKInteractor > QVTKInteractorPointer;
  QVTKInteractorPointer iren (QVTKInteractorPointer::New ());
  this->SetInteractor(iren);

  typedef vtkSmartPointer< vtkInteractorStyle > vtkInteractorStylePointer;
  vtkInteractorStylePointer
    s(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
  iren->Enable();
  this->GetInteractor()->SetInteractorStyle(s);

  this->DoubleBufferOff();
  this->FrontBuffer     = vtkgl::COLOR_ATTACHMENT0_EXT;
  this->FrontLeftBuffer = vtkgl::COLOR_ATTACHMENT0_EXT;
  this->FrontRightBuffer= vtkgl::COLOR_ATTACHMENT0_EXT;
  this->BackBuffer      = vtkgl::COLOR_ATTACHMENT0_EXT;
  this->BackLeftBuffer  = vtkgl::COLOR_ATTACHMENT0_EXT;
  this->BackRightBuffer = vtkgl::COLOR_ATTACHMENT0_EXT;

}

vtkQtOpenGLRenderWindow::~vtkQtOpenGLRenderWindow()
{
  this->Finalize();
}

void vtkQtOpenGLRenderWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf( os, indent );
}

void vtkQtOpenGLRenderWindow::Initialize(void)
{
  if ( m_qtWidget )
  {
    this->SetSize(this->m_qtWidget->viewport()->width(),
                  this->m_qtWidget->viewport()->height());
  }
}

void vtkQtOpenGLRenderWindow::Finalize(void)
{
  // Consider removing renderers here
  // Superclass::Finalize();
  // tell each of the renderers that this render window/graphics context
  // is being removed (the RendererCollection is removed by vtkRenderWindow's
  // destructor)
  vtkObject * obj;
  this->Renderers->InitTraversal();
  for ( obj = this->Renderers->GetNextItemAsObject(); obj != NULL;
        obj = this->Renderers->GetNextItemAsObject() )
  {
  vtkRenderer * ren = vtkOpenGLRenderer::SafeDownCast( obj );
    if ( ren != NULL )
    {
      ren->SetRenderWindow(NULL);
      ren->SetRenderWindow(this);
    }
  }
  this->InitializedOpenGl = false;
}

void vtkQtOpenGLRenderWindow::MakeCurrent()
{
  if ( this->m_qtWidget )
  {
    this->m_qtWidget->makeCurrent();
  }
  if ( !this->InitializedOpenGl
       && this->m_qtWidget
       && this->m_qtWidget->isCurrent() )
  {

    // To avoid recursion, need to set m_initializedOpenGl before
    // calling OpenGlInit.
    this->InitializedOpenGl = true;

    // wipe out any existing display lists
    vtkRenderer* ren;
    vtkCollectionSimpleIterator rsit;
    for (this->Renderers->InitTraversal(rsit);
         (ren = this->Renderers->GetNextRenderer(rsit));)
    {
      ren->SetRenderWindow(NULL);
      ren->SetRenderWindow(this);
    }

    this->OpenGLInit();
  }

  // Consider enclosing this in ifdef _DEBUG.
  // Note that it has a side-effect of resetting the OpenGL error
  // status though.
  if (this->IsCurrent())
    this->CheckAndPrintGraphicError( __FILE__, __LINE__ );
}

bool vtkQtOpenGLRenderWindow::IsCurrent()
{
  if ( this->m_qtWidget )
  {
    return this->m_qtWidget->isCurrent();
  }
  return false;
}

int vtkQtOpenGLRenderWindow::IsDirect()
{
  if ( this->m_qtWidget )
  {
    return 1;
  }
  return 0;
}

// This overrides the base class. We do not need to create a window as
// Qt does that
void vtkQtOpenGLRenderWindow::CreateAWindow()
{
}

// This overrides the base class. We do not need to destroy a window
// as Qt does that
void vtkQtOpenGLRenderWindow::DestroyWindow()
{
}

// Render defers to the widget. 
void vtkQtOpenGLRenderWindow::Render()
{
  if ( m_qtWidget )
  {
    if ( m_qtWidget->isProcessingPaintEvent() )
    {
      dtkDebug() <<  "vtkQtOpenGLRenderWindow::Render VTK Rendering";
      this->MakeCurrent();
      Superclass::Render ();
    }
    else
    {
      dtkDebug() <<  "vtkQtOpenGLRenderWindow::Render Initiate repaint";
      m_initiatedRepaint = true;
      m_qtWidget->resetCachedContent();
      m_qtWidget->viewport()->repaint();
      m_initiatedRepaint = false;
    }
  }
}

// The widget tells us to paint.
void vtkQtOpenGLRenderWindow::NativePaint ()
{
    dtkDebug() <<  "vtkQtOpenGLRenderWindow::NativePaint VTK Rendering";
  Superclass::Render ();
}

void vtkQtOpenGLRenderWindow::SetQtWidget( QVtkGraphicsView * w)
{
  if ( this->m_qtWidget == w )
    return;

  this->m_qtWidget = w;
  if ( m_qtWidget ) {
    this->Initialize();
    if ( this->IsCursorHidden ) {
      this->m_qtWidget->setCursor( Qt::BlankCursor );
    } else {
      this->m_qtWidget->setCursor( GetCurrentQtCursor() );
    }
    this->SetMapped( 1 );
  } else {
    this->SetMapped( 0 );
  }

  // wipe out any existing display lists
  vtkRenderer* ren;
  vtkCollectionSimpleIterator rsit;
  for (this->Renderers->InitTraversal(rsit);
       (ren = this->Renderers->GetNextRenderer(rsit));)
  {
    ren->SetRenderWindow(NULL);
    ren->SetRenderWindow(this);
  }

  InitializedOpenGl = false;
}
//---------------------------------------------------------------------
// Cursor-related functions

void vtkQtOpenGLRenderWindow::SetCurrentCursor(int shape)
{
  this->vtkRenderWindow::SetCurrentCursor(shape);
  if ( m_qtWidget && !this->IsCursorHidden) {
    this->m_qtWidget->setCursor( GetQtCursorForVtkCursorId(shape) );
  }
}

const QCursor & vtkQtOpenGLRenderWindow::GetCurrentQtCursor()
{
  return GetQtCursorForVtkCursorId(this->GetCurrentCursor() );
}

void vtkQtOpenGLRenderWindow::SetQtCursorForVtkCursorId(const QCursor & cursor,
                                                        int id)
{
  this->m_cursorMap[id] = cursor;
}

const QCursor & vtkQtOpenGLRenderWindow::GetQtCursorForVtkCursorId(int id) const
{
  CursorMapType::const_iterator it(this->m_cursorMap.find(id));
  if (it == this->m_cursorMap.end ()) {

    static const QCursor defaultCursor;
    return defaultCursor;
  }
  return it->second;
}

void vtkQtOpenGLRenderWindow::BuildStandardCursors()
{
  SetQtCursorForVtkCursorId( QCursor( Qt::ArrowCursor ), VTK_CURSOR_DEFAULT );
  SetQtCursorForVtkCursorId( QCursor(  Qt::ArrowCursor ), VTK_CURSOR_ARROW   );
  SetQtCursorForVtkCursorId( QCursor( Qt::CrossCursor ), VTK_CURSOR_CROSSHAIR );
  SetQtCursorForVtkCursorId( QCursor( Qt::SizeVerCursor ), VTK_CURSOR_SIZENS );
  SetQtCursorForVtkCursorId( QCursor( Qt::SizeHorCursor ), VTK_CURSOR_SIZEWE );
  SetQtCursorForVtkCursorId( QCursor( Qt::SizeBDiagCursor ), VTK_CURSOR_SIZENW );
  SetQtCursorForVtkCursorId( QCursor( Qt::SizeBDiagCursor ), VTK_CURSOR_SIZESE );
  SetQtCursorForVtkCursorId( QCursor( Qt::SizeFDiagCursor ), VTK_CURSOR_SIZENE );
  SetQtCursorForVtkCursorId( QCursor( Qt::SizeFDiagCursor ), VTK_CURSOR_SIZESW );
  SetQtCursorForVtkCursorId( QCursor( Qt::SizeAllCursor ), VTK_CURSOR_SIZEALL );
  SetQtCursorForVtkCursorId( QCursor( Qt::PointingHandCursor ), VTK_CURSOR_HAND );
}

void vtkQtOpenGLRenderWindow::HideCursor()
{
  if (this->IsCursorHidden )
    return;

  if ( m_qtWidget ) {
    this->m_qtWidget->setCursor( Qt::BlankCursor );
  }

  this->IsCursorHidden = true;
  this->Modified();
}

void vtkQtOpenGLRenderWindow::ShowCursor()
{
  if (!this->IsCursorHidden )
    return;

    if ( m_qtWidget ) {
        this->m_qtWidget->setCursor( this->GetCurrentQtCursor() );
    }
    this->IsCursorHidden = false;
    this->Modified();
}

void vtkQtOpenGLRenderWindow::SetCursorPosition( int x, int y)
{
  if ( m_qtWidget ) {
    QCursor::setPos(x,y);
  }
}
// End of Cursor-related functions
//---------------------------------------------------------------------

void vtkQtOpenGLRenderWindow::Frame( void )
{
  glFlush();
}

void vtkQtOpenGLRenderWindow::Start()
{
  // set the current window
  this->MakeCurrent();
}

void vtkQtOpenGLRenderWindow::SetDisplayId( void * )
{

}

void vtkQtOpenGLRenderWindow::SetWindowId( void * )
{

}

void vtkQtOpenGLRenderWindow::SetNextWindowId( void * )
{

}

void vtkQtOpenGLRenderWindow::SetParentId( void * )
{

}

void * vtkQtOpenGLRenderWindow::GetGenericDisplayId()
{
  return NULL;
}

void * vtkQtOpenGLRenderWindow::GetGenericWindowId()
{
  if ( m_qtWidget ) {
    return (void*) this->m_qtWidget->viewport();
  }
  return NULL;
}

void * vtkQtOpenGLRenderWindow::GetGenericParentId()
{
  if ( m_qtWidget ) {
    return (void*) this->m_qtWidget;
  }
  return NULL;
}

void * vtkQtOpenGLRenderWindow::GetGenericContext()
{
  return NULL;
}

void * vtkQtOpenGLRenderWindow::GetGenericDrawable()
{
  return NULL;
}

void vtkQtOpenGLRenderWindow::SetWindowInfo( char * )
{

}

void vtkQtOpenGLRenderWindow::SetNextWindowInfo( char * )
{

}

void vtkQtOpenGLRenderWindow::SetParentInfo( char * )
{

}

void vtkQtOpenGLRenderWindow::WindowRemap()
{

}

void vtkQtOpenGLRenderWindow::SetFullScreen( int isFullScreen )
{
  if ( this->m_qtWidget ) {
    if ( isFullScreen ) {
      this->m_qtWidget->showFullScreen();
    } else{
      this->m_qtWidget->showNormal();
    }
  }
}

int vtkQtOpenGLRenderWindow::GetEventPending()
{
    return 0;
}

int * vtkQtOpenGLRenderWindow::GetScreenSize()
{
  QRect screenGeometry = QApplication::desktop()->screenGeometry();
  this->ScreenSize[0] = screenGeometry.width();;
  this->ScreenSize[1] = screenGeometry.height();;

  return this->ScreenSize;
}

void vtkQtOpenGLRenderWindow::CheckAndPrintGraphicError(
  const char * headerMessage , int line)
{
  if ( !this->IsCurrent() )
  {
    if ( headerMessage )
      std::cerr << headerMessage;
    if ( line >= 0 )
      std::cerr << "(" << line << ") :";
    std::cerr << "The vtkQtOpenGLRenderWindow does not have "
      "the current OpenGL Context" << std::endl;
    return;
  }

  // Calls glGetError and sets this->LastGraphicError.
  this->CheckGraphicError();

  if ( this->HasGraphicError() )
  {
    if ( headerMessage )
      std::cerr << headerMessage;
    if ( line >= 0 )
      std::cerr << "(" << line << ") :";
    std::cerr << ": ERROR (x" << std::hex << this->LastGraphicError
              << ")  " << std::dec << this->GetLastGraphicErrorString()
              << std::endl;
  }
}
