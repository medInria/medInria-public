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

#include <QGraphicsView>
#include <QCursor>

#ifdef VTK_USE_OGLR
    #include <QX11Info>
#endif
#include <QVTKWidget.h>
#include <QVtkGraphicsView.h>

#include "vtkObjectFactory.h"
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkQtOpenGLRenderWindow, "0");
vtkStandardNewMacro(vtkQtOpenGLRenderWindow);

vtkQtOpenGLRenderWindow::vtkQtOpenGLRenderWindow() :
   m_qtWidget (NULL),
   m_initiatedRepaint(false)
{
    BuildStandardCursors ();
    typedef vtkSmartPointer< QVTKInteractor > QVTKInteractorPointer;
    QVTKInteractorPointer iren (QVTKInteractorPointer::New ());
    this->SetInteractor(iren);

    typedef vtkSmartPointer< vtkInteractorStyle > vtkInteractorStylePointer;
    vtkInteractorStylePointer s (vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
    iren->Enable();
    this->GetInteractor()->SetInteractorStyle(s);
}

vtkQtOpenGLRenderWindow::~vtkQtOpenGLRenderWindow()
{
  this->Finalize();
}

void vtkQtOpenGLRenderWindow::MakeCurrent()
{
    if ( this->m_qtWidget ) {
        this->m_qtWidget->makeCurrent();
    }
}

void vtkQtOpenGLRenderWindow::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkQtOpenGLRenderWindowBase::PrintSelf( os, indent );
}

void vtkQtOpenGLRenderWindow::Initialize(void)
{
    if ( ! m_qtWidget ) {
        return;
    }

#ifdef _WIN32
    this->DeviceContext = this->m_qtWidget->getDC();
    this->WindowId = this->m_qtWidget->viewport()->winId();
    this->OwnWindow = 1;
#endif

#ifdef VTK_USE_OGLR
    this->WindowId = this->m_qtWidget->viewport()->winId();
    this->OwnWindow = 1;
    this->DisplayId = QX11Info::display();
#endif

    this->SetSize(this->m_qtWidget->viewport()->width(), this->m_qtWidget->viewport()->height());
}

void vtkQtOpenGLRenderWindow::Finalize(void)
{
    // Consider removing renderers here
}

// This overrides the base class. We do not need to create a window as Qt does that
void vtkQtOpenGLRenderWindow::CreateAWindow()
{
}

// This overrides the base class. We do not need to destroy a window as Qt does that
void vtkQtOpenGLRenderWindow::DestroyWindow()
{
}

// Render defers to the widget. 
void vtkQtOpenGLRenderWindow::Render()
{
    if ( m_qtWidget )
    {
      QVtkGraphicsView * qvtkview =
        qobject_cast< QVtkGraphicsView * >( m_qtWidget );
      if ( qvtkview->isProcessingPaintEvent() )
      {
        vtkQtOpenGLRenderWindowBase::Render ();
      }
      else
      {
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
    vtkQtOpenGLRenderWindowBase::Render ();
}

void vtkQtOpenGLRenderWindow::SetQtWidget( QVtkGraphicsView * w)
{
    this->m_qtWidget = w;
    if ( m_qtWidget ) {
        this->m_qtWidget->setCursor( GetCurrentQtCursor() );
    }
}

void vtkQtOpenGLRenderWindow::SetCurrentCursor(int shape)
{
    this->vtkRenderWindow::SetCurrentCursor(shape);
    if ( m_qtWidget ) {
        this->m_qtWidget->setCursor( GetQtCursorForVtkCursorId(shape) );
    }
}

const QCursor & vtkQtOpenGLRenderWindow::GetCurrentQtCursor()
{
    return GetQtCursorForVtkCursorId(this->GetCurrentCursor() );
}

void vtkQtOpenGLRenderWindow::SetQtCursorForVtkCursorId(const QCursor & cursor, int id)
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
