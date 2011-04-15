/*=========================================================================

  Program:   Visualization Toolkit
  Module:    QVtkGraphicsView.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*
 * Copyright 2004 Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the
 * U.S. Government. Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that this Notice and any
 * statement of authorship are reproduced on all copies.
 */

/*========================================================================
 For general information about using VTK and Qt, see:
 http://www.trolltech.com/products/3rdparty/vtksupport.html
=========================================================================*/

#include "QVtkGraphicsView.h"

#if defined(VTK_USE_TDX) && defined(Q_WS_WIN)
# include "vtkTDxWinDevice.h"
#endif

#if defined(VTK_USE_TDX) && defined(Q_WS_MAC)
# include "vtkTDxMacDevice.h"
#endif

# include "QVTKPaintEngine.h"

#include "QEvent"
#include "QKeyEvent"
#include "QGLWidget"
#include <QGLFramebufferObject>

#include <QDebug>

//included for the interactor class.
#include "QVTKWidget.h"

#include "vtkstd/map"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderWindow.h"
#if defined(QVTK_USE_CARBON)
#  include "vtkCarbonRenderWindow.h"
#endif
#include "vtkCommand.h"
#include "vtkOStrStreamWrapper.h"
#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkConfigure.h"
#include "vtkUnsignedCharArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkQtOpenGLRenderWindow.h"

typedef vtkSmartPointer< vtkQtOpenGLRenderWindow > vtkQtOpenGLRenderWindowPointer;

/*! constructor */
QVtkGraphicsView::QVtkGraphicsView(QWidget* p)
  : BaseClass(p), 
    mMouseEventCategory(MouseEventCategory_None),
    mProcessingPaintEvent(false),
    mFBO(NULL),
    mRenWin(NULL),
    mCacheVtkBackground(true),
    mLastRenderMTime(0)
{
    setViewport(new QGLWidget(this) );
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setMouseTracking(false);
    setCacheMode(CacheNone);

    // create a default vtk window
    vtkQtOpenGLRenderWindowPointer win (vtkQtOpenGLRenderWindowPointer::New());
    win->SetQtWidget(this);
    this->setRenderWindow(win);
}

/*! destructor */

QVtkGraphicsView::~QVtkGraphicsView()
{
  // get rid of the VTK window
  this->setRenderWindow(NULL);
  if(mFBO)
      delete mFBO;
}

/*! get the render window
 */
vtkRenderWindow* QVtkGraphicsView::getRenderWindow()
{
    // QGLWidget * qglWidget = qobject_cast< QGLWidget *>(this->viewport());
    if ( this->mRenWin ) {

        return this->mRenWin;
    } else {
        // create a default vtk window
        vtkQtOpenGLRenderWindowPointer win(
            vtkQtOpenGLRenderWindowPointer::New() );
        win->SetQtWidget(this);
        this->setRenderWindow(win);
        return win;
    } 
    return NULL;
}

/*! set the render window
  this will bind a VTK window with the Qt window
  it'll also replace an existing VTK window
*/
void QVtkGraphicsView::setRenderWindow(vtkQtOpenGLRenderWindow* w)
{
  // do nothing if we don't have to
  if(w == this->mRenWin)
    {
    return;
    }

  // unregister previous window
  if(this->mRenWin)
    {
    this->mRenWin->Finalize();
    this->mRenWin->SetQtWidget(NULL);
    this->mRenWin->Delete();
    }

  // now set the window
  this->mRenWin = w;

  if(this->mRenWin)
    {
    // if it is mapped somewhere else, unmap it
    this->mRenWin->Finalize();
    this->mRenWin->Register(NULL);
    this->mRenWin->SetQtWidget(this);

    // tell the vtk window what the size of this window is
    this->mRenWin->SetSize(this->width(), this->height());
    this->mRenWin->SetPosition(this->x(), this->y());

    // have VTK start this window and create the necessary graphics resources
//    makeCurrent();
//    this->mRenWin->OpenGLInit();

    // if an interactor wasn't provided, we'll make one by default
    if(!this->mRenWin->GetInteractor())
      {
      // create a default interactor
      QVTKInteractor* iren = QVTKInteractor::New();
      iren->SetUseTDx(this->UseTDx);
      this->mRenWin->SetInteractor(iren);
      iren->Initialize();

      // now set the default style
      vtkInteractorStyle* s = vtkInteractorStyleTrackballCamera::New();
      iren->SetInteractorStyle(s);

      iren->Delete();
      s->Delete();
      }

    // tell the interactor the size of this window
    this->mRenWin->GetInteractor()->SetSize(this->width(), this->height());

    }
}



/*! get the Qt/VTK interactor
 */
QVTKInteractor* QVtkGraphicsView::getInteractor()
{
  return QVTKInteractor
    ::SafeDownCast(this->getRenderWindow()->GetInteractor());
}

vtkRenderWindowInteractor * QVtkGraphicsView::GetRenderWindowInteractor ()
{
    vtkRenderWindow * renWin = this->getRenderWindow();
    if ( !renWin ) 
        return NULL;
    return renWin->GetInteractor();
}


/*! handle resize event
 */
void QVtkGraphicsView::resizeEvent(QResizeEvent* e)
{
    BaseClass::resizeEvent(e);

    QRect viewRect = this->viewport()->rect();
    QRectF sceneRect(viewRect);
    setSceneRect(sceneRect);
    if (this->scene()) {
        this->scene()->setSceneRect(sceneRect);
    }
    
    if(vtkRenderWindow * renWin = this->getRenderWindow() )
    {
        // give the size to the interactor and vtk window
        renWin->SetSize(viewRect.width(), viewRect.height());

        // The renderwindow sets the interactor size, so we do not need to do it too.
        //if(renWin->GetInteractor())
        //{
        //    renWin->GetInteractor()->SetSize(this->width(), this->height());
        //}
    }

    emit sizeChanged( this->width(), this->height());

}


/*! handle paint event
 */
void QVtkGraphicsView::paintEvent(QPaintEvent* e )
{
    QRectF sceneRect(this->viewport()->rect());
    if (this->sceneRect () != sceneRect ) {
        setSceneRect(sceneRect);
    }
    if (this->scene() && this->scene()->sceneRect () != sceneRect) {
        this->scene()->setSceneRect(sceneRect);
    }

    mProcessingPaintEvent = true;
    vtkRenderWindow * renWin = this->getRenderWindow();
    if (renWin) {
        renWin->SwapBuffersOff();
    }

    BaseClass::paintEvent(e);

    if (renWin) {
        renWin->SwapBuffersOn();
        renWin->Frame();
    }

    mProcessingPaintEvent = false;
}

void QVtkGraphicsView::drawBackground(QPainter *painter, const QRectF& rect )
{
    bool reRender = !mCacheVtkBackground;

    if ( this->mRenWin && (mLastRenderMTime < this->mRenWin->GetMTime()) ) {
        reRender = true;
    }
    //reRender = false;
    QSize sz (rect.width(), rect.height());
    if(!mFBO || sz != mFBO->size() || reRender)
    {
        reRender = true;
    }

#if QT_VERSION >= 0x040600
    // tell Qt we're doing our own GL calls
    // if necessary, it'll put us in an OpenGL 1.x compatible state.
    painter->beginNativePainting();
#endif
    // Possibly other flags may need to be altered.
    const bool isDepthEnabled       = glIsEnabled( GL_DEPTH_TEST );
    const bool isLightingEnabled    = glIsEnabled( GL_LIGHTING );

    if ( reRender ) {
    
        if (mCacheVtkBackground) {

            if ( !mFBO || sz != mFBO->size() )
            {
                if(mFBO) {
                    delete mFBO;
                    mFBO = NULL;
                }

                if(!sz.isEmpty())
                    mFBO = new QGLFramebufferObject(sz, QGLFramebufferObject::Depth);

            }
            if (mFBO)
                mFBO->bind();
        }

        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // Do VTK Render.
        glShadeModel(GL_FLAT);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);

        vtkRenderWindow * renWin = this->getRenderWindow();
        // Render VTK
        if ( renWin ) {
            renWin->GetInteractor()->Render() ;
                // qDebug() << "Rendering vtk";

            mLastRenderMTime = renWin->GetMTime();
        } 

        // Restore OpenGL state
        isDepthEnabled    ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        isLightingEnabled ? glEnable(GL_LIGHTING)   : glDisable(GL_LIGHTING);

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopAttrib();
        glPopClientAttrib();

        if (mCacheVtkBackground)
            mFBO->release();

    } 

    if (mCacheVtkBackground) {
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // mFBO->drawTexture(rect, mFBO->texture());

       // simply draw the already existing texture to the scene
        // modifications to the texture is done using the VTK api (e.g. vtkRenderWindow::Render())
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mFBO->texture());

        QColor c = this->palette().color(QPalette::Window);
        glColor4ub(c.red(),c.green(),c.blue(),c.alpha());

        if(c.alpha() < 255)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }
                // qDebug() << "Rendering vtk from cache";
        glBegin(GL_QUADS);
        glTexCoord2i(0,1);
        glVertex2f(rect.left(),rect.top());
        glTexCoord2i(1,1);
        glVertex2f(rect.right(),rect.top());
        glTexCoord2i(1,0);
        glVertex2f(rect.right(),rect.bottom());
        glTexCoord2i(0,0);
        glVertex2f(rect.left(),rect.bottom());
        glEnd();

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopAttrib();
        glPopClientAttrib();
        // Restore OpenGL state
        isDepthEnabled    ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        isLightingEnabled ? glEnable(GL_LIGHTING)   : glDisable(GL_LIGHTING);
    }



#if QT_VERSION >= 0x040600
    painter->endNativePainting();
#endif
}

/*! handle mouse press event
 */
void QVtkGraphicsView::mousePressEvent(QMouseEvent* e)
{

  // Emit a mouse press event for anyone who might be interested
    //emit mouseEvent(e);
    if(this->itemAt(e->pos()))
    {
        e->ignore();
        QGraphicsView::mousePressEvent(e);
        if ( e->isAccepted() )
        {
          mMouseEventCategory = MouseEventCategory_Item;
          return;
        }
    }
    mMouseEventCategory = MouseEventCategory_Background;

  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }

  // give interactor the event information
  iren->SetEventInformationFlipY(e->x(), e->y(), 
                              (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
                              (e->modifiers() & Qt::ShiftModifier ) > 0 ? 1 : 0,
                              0,
                              e->type() == QEvent::MouseButtonDblClick ? 1 : 0);

  // invoke appropriate vtk event
  switch(e->button())
    {
    case Qt::LeftButton:
      iren->InvokeEvent(vtkCommand::LeftButtonPressEvent, e);
      break;

    case Qt::MidButton:
      iren->InvokeEvent(vtkCommand::MiddleButtonPressEvent, e);
      break;

    case Qt::RightButton:
      iren->InvokeEvent(vtkCommand::RightButtonPressEvent, e);
      break;

    default:
      break;
    }
}

/*! handle mouse press event
 */
void QVtkGraphicsView::mouseDoubleClickEvent(QMouseEvent* e)
{

  // Emit a mouse press event for anyone who might be interested
    //emit mouseEvent(e);
    if(this->itemAt(e->pos()))
    {
        e->ignore();
        QGraphicsView::mousePressEvent(e);
        if ( e->isAccepted() )
        {
          mMouseEventCategory = MouseEventCategory_Item;
          return;
        }
    }
    mMouseEventCategory = MouseEventCategory_Background;

  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }

  // give interactor the event information
  iren->SetEventInformationFlipY(e->x(), e->y(), 
                              (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
                              (e->modifiers() & Qt::ShiftModifier ) > 0 ? 1 : 0,
                              0,
                              e->type() == QEvent::MouseButtonDblClick ? 1 : 0);

  // invoke appropriate vtk event
  switch(e->button())
    {
    case Qt::LeftButton:
      iren->InvokeEvent(vtkCommand::LeftButtonPressEvent, e);
      break;

    case Qt::MidButton:
      iren->InvokeEvent(vtkCommand::MiddleButtonPressEvent, e);
      break;

    case Qt::RightButton:
      iren->InvokeEvent(vtkCommand::RightButtonPressEvent, e);
      break;

    default:
      break;
    }
}
/*! handle mouse move event
 */
void QVtkGraphicsView::mouseMoveEvent(QMouseEvent* e)
{
    if(mMouseEventCategory == MouseEventCategory_Item )
    {
        QGraphicsView::mouseMoveEvent(e);
        return;
    } else if ( mMouseEventCategory == MouseEventCategory_None &&
        this->itemAt(e->pos()) )
    {
        QGraphicsView::mouseMoveEvent(e);
        return;
    }

  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  if ( this->mRenWin ) {
      this->viewport()->setCursor( mRenWin->GetCurrentQtCursor() );
  }

  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // give interactor the event information
  iren->SetEventInformationFlipY(e->x(), e->y(), 
                             (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
                             (e->modifiers() & Qt::ShiftModifier ) > 0 ? 1 : 0);
  
  // invoke vtk event
  iren->InvokeEvent(vtkCommand::MouseMoveEvent, e);
}


/*! handle enter event
 */
void QVtkGraphicsView::enterEvent(QEvent* e)
{
  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }

  iren->InvokeEvent(vtkCommand::EnterEvent, e);
}

/*! handle leave event
 */
void QVtkGraphicsView::leaveEvent(QEvent* e)
{
  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  iren->InvokeEvent(vtkCommand::LeaveEvent, e);
}

/*! handle mouse release event
 */
void QVtkGraphicsView::mouseReleaseEvent(QMouseEvent* e)
{
    if( mMouseEventCategory == MouseEventCategory_Item )
    {
        QGraphicsView::mouseReleaseEvent(e);
        mMouseEventCategory = MouseEventCategory_None;
        return;
    }

  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // give vtk event information
  iren->SetEventInformationFlipY(e->x(), e->y(), 
                             (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
                             (e->modifiers() & Qt::ShiftModifier ) > 0 ? 1 : 0);
  
  // invoke appropriate vtk event
  switch(e->button())
    {
    case Qt::LeftButton:
      iren->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, e);
      break;

    case Qt::MidButton:
      iren->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, e);
      break;

    case Qt::RightButton:
      iren->InvokeEvent(vtkCommand::RightButtonReleaseEvent, e);
      break;

    default:
      break;
    }
    mMouseEventCategory = MouseEventCategory_None;
}

/*! handle key press event
 */
void QVtkGraphicsView::keyPressEvent(QKeyEvent* e)
{
  vtkRenderWindowInteractor* iren = GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // get key and keysym information
  int ascii_key;
  const char* keysym;
  getKeyAndKeySymInformation ( e, keysym, ascii_key );
  
  // give interactor event information
  iren->SetKeyEventInformation(
    (e->modifiers() & Qt::ControlModifier),
    (e->modifiers() & Qt::ShiftModifier),
    ascii_key, e->count(), keysym);

  // invoke vtk event
  iren->InvokeEvent(vtkCommand::KeyPressEvent, e);
  
  // invoke char event only for ascii characters
  if(ascii_key)
    {
    iren->InvokeEvent(vtkCommand::CharEvent, e);
    }
}

/*! handle key release event
 */
void QVtkGraphicsView::keyReleaseEvent(QKeyEvent* e)
{

  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // get key and keysym info
  int ascii_key;
  const char* keysym;
  getKeyAndKeySymInformation ( e, keysym, ascii_key );

  // give event information to interactor
  iren->SetKeyEventInformation(
    (e->modifiers() & Qt::ControlModifier),
    (e->modifiers() & Qt::ShiftModifier),
    ascii_key, e->count(), keysym);

  // invoke vtk event
  iren->InvokeEvent(vtkCommand::KeyReleaseEvent, e);
}

#ifndef QT_NO_WHEELEVENT
void QVtkGraphicsView::wheelEvent(QWheelEvent* e)
{
  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }

// VTK supports wheel mouse events only in version 4.5 or greater
  // give event information to interactor
  iren->SetEventInformationFlipY(e->x(), e->y(), 
                             (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
                             (e->modifiers() & Qt::ShiftModifier ) > 0 ? 1 : 0);
  
  // invoke vtk event
  // if delta is positive, it is a forward wheel event
  if(e->delta() > 0)
    {
    iren->InvokeEvent(vtkCommand::MouseWheelForwardEvent, e);
    }
  else
    {
    iren->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, e);
    }
}
#endif

#if 0
void QVtkGraphicsView::focusInEvent(QFocusEvent*)
{
  // These prevent updates when the window 
  // gains or loses focus.  By default, Qt
  // does an update because the color group's 
  // active status changes.  We don't even use
  // color groups so we do nothing here.
  
  // For 3Dconnexion devices:
  QVTKInteractor* iren = this->GetInteractor();
  // Note that this class can have interactor of type
  // other than QVTKInteractor
  if (iren)
    {
    iren->StartListening();
    }
}
#endif

#if 0
void QVtkGraphicsView::focusOutEvent(QFocusEvent*)
{
  // These prevent updates when the window 
  // gains or loses focus.  By default, Qt
  // does an update because the color group's 
  // active status changes.  We don't even use
  // color groups so we do nothing here.
  
  // For 3DConnexion devices:
  QVTKInteractor* iren = this->GetInteractor();
  // Note that this class can have interactor of type
  // other than QVTKInteractor
  if (iren)
    {
    iren->StopListening();
    }
}


void QVtkGraphicsView::contextMenuEvent(QContextMenuEvent* e)
{
  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // give interactor the event information
  iren->SetEventInformationFlipY(e->x(), e->y(), 
                             (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
                             (e->modifiers() & Qt::ShiftModifier ) > 0 ? 1 : 0);

  // invoke event and pass qt event for additional data as well
  iren->InvokeEvent(QVtkGraphicsView::ContextMenuEvent, e);
  
}

#endif

void QVtkGraphicsView::dragEnterEvent(QDragEnterEvent* e)
{
  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // invoke event and pass qt event for additional data as well
  iren->InvokeEvent(QVtkGraphicsView::DragEnterEvent, e);
}

void QVtkGraphicsView::dragMoveEvent(QDragMoveEvent* e)
{
  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // give interactor the event information
  iren->SetEventInformationFlipY(e->pos().x(), e->pos().y());

  // invoke event and pass qt event for additional data as well
  iren->InvokeEvent(QVtkGraphicsView::DragMoveEvent, e);
}

void QVtkGraphicsView::dragLeaveEvent(QDragLeaveEvent* e)
{
  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // invoke event and pass qt event for additional data as well
  iren->InvokeEvent(QVtkGraphicsView::DragLeaveEvent, e);
}



void QVtkGraphicsView::dropEvent(QDropEvent* e)
{
    if(this->itemAt(e->pos()))
    {
        QGraphicsView::dropEvent(e);
        return;
    } else {
        QWidget::dropEvent(e);
    }

  vtkRenderWindowInteractor* iren = this->GetRenderWindowInteractor();
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // give interactor the event information
  iren->SetEventInformationFlipY(e->pos().x(), e->pos().y());

  // invoke event and pass qt event for additional data as well
  iren->InvokeEvent(QVtkGraphicsView::DropEvent, e);
}


void QVtkGraphicsView::getKeyAndKeySymInformation( const QKeyEvent* e, const char * & keysym, int & ascii_key )
{
  ascii_key = e->text().length() ? e->text().unicode()->toLatin1() : 0;
  keysym = asciiToKeySym(ascii_key);
  if(!keysym)
    {
    // get virtual keys
    keysym = qtKeyToKeySym(static_cast<Qt::Key>(e->key()));
    }

  if(!keysym)
    {
    keysym = "None";
    }
}

const char*  QVtkGraphicsView::asciiToKeySym ( int i )
{
static const char *AsciiToKeySymTable[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, "Tab", 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  "space", "exclam", "quotedbl", "numbersign",
  "dollar", "percent", "ampersand", "quoteright",
  "parenleft", "parenright", "asterisk", "plus",
  "comma", "minus", "period", "slash",
  "0", "1", "2", "3", "4", "5", "6", "7",
  "8", "9", "colon", "semicolon", "less", "equal", "greater", "question",
  "at", "A", "B", "C", "D", "E", "F", "G",
  "H", "I", "J", "K", "L", "M", "N", "O",
  "P", "Q", "R", "S", "T", "U", "V", "W",
  "X", "Y", "Z", "bracketleft",
  "backslash", "bracketright", "asciicircum", "underscore",
  "quoteleft", "a", "b", "c", "d", "e", "f", "g",
  "h", "i", "j", "k", "l", "m", "n", "o",
  "p", "q", "r", "s", "t", "u", "v", "w",
  "x", "y", "z", "braceleft", "bar", "braceright", "asciitilde", "Delete"
  };

  if(i >= 0 && i < 128 )
    {
    return AsciiToKeySymTable[i];
    }
  return NULL;
}

const char* QVtkGraphicsView::qtKeyToKeySym (Qt::Key i)
{
#define QVTK_HANDLE(x,y) \
  case x : \
    ret = y; \
    break;

  const char* ret = 0;
  switch(i)
    {
    // Cancel
    QVTK_HANDLE(Qt::Key_Backspace, "BackSpace")
      QVTK_HANDLE(Qt::Key_Tab, "Tab")
      QVTK_HANDLE(Qt::Key_Backtab, "Tab")
      //QVTK_HANDLE(Qt::Key_Clear, "Clear")
      QVTK_HANDLE(Qt::Key_Return, "Return")
      QVTK_HANDLE(Qt::Key_Enter, "Return")
      QVTK_HANDLE(Qt::Key_Shift, "Shift_L")
      QVTK_HANDLE(Qt::Key_Control, "Control_L")
      QVTK_HANDLE(Qt::Key_Alt, "Alt_L")
      QVTK_HANDLE(Qt::Key_Pause, "Pause")
      QVTK_HANDLE(Qt::Key_CapsLock, "Caps_Lock")
      QVTK_HANDLE(Qt::Key_Escape, "Escape")
      QVTK_HANDLE(Qt::Key_Space, "space")
      //QVTK_HANDLE(Qt::Key_Prior, "Prior")
      //QVTK_HANDLE(Qt::Key_Next, "Next")
      QVTK_HANDLE(Qt::Key_End, "End")
      QVTK_HANDLE(Qt::Key_Home, "Home")
      QVTK_HANDLE(Qt::Key_Left, "Left")
      QVTK_HANDLE(Qt::Key_Up, "Up")
      QVTK_HANDLE(Qt::Key_Right, "Right")
      QVTK_HANDLE(Qt::Key_Down, "Down")

      // Select
      // Execute
      QVTK_HANDLE(Qt::Key_SysReq, "Snapshot")
      QVTK_HANDLE(Qt::Key_Insert, "Insert")
      QVTK_HANDLE(Qt::Key_Delete, "Delete")
      QVTK_HANDLE(Qt::Key_Help, "Help")
      QVTK_HANDLE(Qt::Key_0, "0")
      QVTK_HANDLE(Qt::Key_1, "1")
      QVTK_HANDLE(Qt::Key_2, "2")
      QVTK_HANDLE(Qt::Key_3, "3")
      QVTK_HANDLE(Qt::Key_4, "4")
      QVTK_HANDLE(Qt::Key_5, "5")
      QVTK_HANDLE(Qt::Key_6, "6")
      QVTK_HANDLE(Qt::Key_7, "7")
      QVTK_HANDLE(Qt::Key_8, "8")
      QVTK_HANDLE(Qt::Key_9, "9")
      QVTK_HANDLE(Qt::Key_A, "a")
      QVTK_HANDLE(Qt::Key_B, "b")
      QVTK_HANDLE(Qt::Key_C, "c")
      QVTK_HANDLE(Qt::Key_D, "d")
      QVTK_HANDLE(Qt::Key_E, "e")
      QVTK_HANDLE(Qt::Key_F, "f")
      QVTK_HANDLE(Qt::Key_G, "g")
      QVTK_HANDLE(Qt::Key_H, "h")
      QVTK_HANDLE(Qt::Key_I, "i")
      QVTK_HANDLE(Qt::Key_J, "h")
      QVTK_HANDLE(Qt::Key_K, "k")
      QVTK_HANDLE(Qt::Key_L, "l")
      QVTK_HANDLE(Qt::Key_M, "m")
      QVTK_HANDLE(Qt::Key_N, "n")
      QVTK_HANDLE(Qt::Key_O, "o")
      QVTK_HANDLE(Qt::Key_P, "p")
      QVTK_HANDLE(Qt::Key_Q, "q")
      QVTK_HANDLE(Qt::Key_R, "r")
      QVTK_HANDLE(Qt::Key_S, "s")
      QVTK_HANDLE(Qt::Key_T, "t")
      QVTK_HANDLE(Qt::Key_U, "u")
      QVTK_HANDLE(Qt::Key_V, "v")
      QVTK_HANDLE(Qt::Key_W, "w")
      QVTK_HANDLE(Qt::Key_X, "x")
      QVTK_HANDLE(Qt::Key_Y, "y")
      QVTK_HANDLE(Qt::Key_Z, "z")
      // KP_0 - KP_9
      QVTK_HANDLE(Qt::Key_Asterisk, "asterisk")
      QVTK_HANDLE(Qt::Key_Plus, "plus")
      // bar
      QVTK_HANDLE(Qt::Key_Minus, "minus")
      QVTK_HANDLE(Qt::Key_Period, "period")
      QVTK_HANDLE(Qt::Key_Slash, "slash")
      QVTK_HANDLE(Qt::Key_F1, "F1")
      QVTK_HANDLE(Qt::Key_F2, "F2")
      QVTK_HANDLE(Qt::Key_F3, "F3")
      QVTK_HANDLE(Qt::Key_F4, "F4")
      QVTK_HANDLE(Qt::Key_F5, "F5")
      QVTK_HANDLE(Qt::Key_F6, "F6")
      QVTK_HANDLE(Qt::Key_F7, "F7")
      QVTK_HANDLE(Qt::Key_F8, "F8")
      QVTK_HANDLE(Qt::Key_F9, "F9")
      QVTK_HANDLE(Qt::Key_F10, "F10")
      QVTK_HANDLE(Qt::Key_F11, "F11")
      QVTK_HANDLE(Qt::Key_F12, "F12")
      QVTK_HANDLE(Qt::Key_F13, "F13")
      QVTK_HANDLE(Qt::Key_F14, "F14")
      QVTK_HANDLE(Qt::Key_F15, "F15")
      QVTK_HANDLE(Qt::Key_F16, "F16")
      QVTK_HANDLE(Qt::Key_F17, "F17")
      QVTK_HANDLE(Qt::Key_F18, "F18")
      QVTK_HANDLE(Qt::Key_F19, "F19")
      QVTK_HANDLE(Qt::Key_F20, "F20")
      QVTK_HANDLE(Qt::Key_F21, "F21")
      QVTK_HANDLE(Qt::Key_F22, "F22")
      QVTK_HANDLE(Qt::Key_F23, "F23")
      QVTK_HANDLE(Qt::Key_F24, "F24")
      QVTK_HANDLE(Qt::Key_NumLock, "Num_Lock")
      QVTK_HANDLE(Qt::Key_ScrollLock, "Scroll_Lock")

      default:
    break;
    }
  return ret;
}

void QVtkGraphicsView::makeCurrent()
{
    QGLWidget * w = qobject_cast<QGLWidget*>(viewport());
    Q_ASSERT(w);
    w->makeCurrent();
}

bool QVtkGraphicsView::isCurrent()
{
    QGLWidget * w = qobject_cast<QGLWidget*>(viewport());
    Q_ASSERT(w);
    return w->context() == QGLContext::currentContext() && w->context()->isValid();
}

void QVtkGraphicsView::setCacheVtkBackground( bool value )
{
    if ( mCacheVtkBackground != value ) {
        mCacheVtkBackground = value;
        if (!mCacheVtkBackground) {

            if (mFBO) {
                delete mFBO;
                mFBO = NULL;
            }
        }
    }
}
