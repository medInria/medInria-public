/*=========================================================================

  Program:   Visualization Toolkit
  Module:    QVtkGraphicsView.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/*=========================================================================

  Copyright 2004 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
  license for use of this work by or on behalf of the
  U.S. Government. Redistribution and use in source and binary forms, with
  or without modification, are permitted provided that this Notice and any
  statement of authorship are reproduced on all copies.

=========================================================================*/

/*========================================================================
 For general information about using VTK and Qt, see:
 http://www.trolltech.com/products/3rdparty/vtksupport.html
=========================================================================*/

// .NAME QVtkGraphicsView - Display a vtkRenderWindow in a Qt's QWidget.
// .SECTION Description
// QVtkGraphicsView provides a way to display VTK data in a Qt widget.

#ifndef Q_VTK_GRAPHICSVIEW_H
#define Q_VTK_GRAPHICSVIEW_H

#include <QGraphicsView>
class QPaintEngine;

class vtkRenderWindow;
class vtkQtOpenGLRenderWindow;

class QVTKInteractor;
class QVTKPaintEngine;
class QGLFramebufferObject;

#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkConfigure.h>
#include <vtkToolkits.h>
class vtkImageData;

#if defined(Q_WS_MAC)
# if defined(QT_MAC_USE_COCOA) && defined(VTK_USE_COCOA)
#  define QVTK_USE_COCOA
# elif !defined(QT_MAC_USE_COCOA) && defined(VTK_USE_CARBON)
#  define QVTK_USE_CARBON
# elif defined(VTK_USE_COCOA)
#  error "VTK configured to use Cocoa, but Qt configured to use Carbon"
# elif defined(VTK_USE_CARBON)
#  error "VTK configured to use Carbon, but Qt configured to use Cocoa"
# endif
#endif


#if defined(QVTK_USE_CARBON)
#include <Carbon/Carbon.h>    // Event handling for dirty region
#endif

#include "vtkTDxConfigure.h" // defines VTK_USE_TDX
#if defined(VTK_USE_TDX) && defined(Q_WS_WIN)
class vtkTDxWinDevice;
#endif
#if defined(VTK_USE_TDX) && defined(Q_WS_MAC)
class vtkTDxMacDevice;
#endif

/**
 * QVtkGraphicsView displays a VTK window in a Qt window.
 * 
 * Note that the viewport is slightly smaller than the widget : there
 * is a margin around the viewport widget (2 pixels on all sides for
 * the current style).
 */
class QVtkGraphicsView : public QGraphicsView
{
  typedef QGraphicsView BaseClass;
  Q_OBJECT;

public:
  //! constructor
  QVtkGraphicsView(QWidget* parent = NULL);
  //! destructor
  virtual ~QVtkGraphicsView();

  // Description:
  // Get the vtk render window.
  vtkRenderWindow* getRenderWindow();

  // Description:
  // Set the vtk render window, if you wish to use your own vtkRenderWindow
  void setRenderWindow(vtkQtOpenGLRenderWindow*);
    
  // Description:
  // Get the Qt/vtk interactor that was either created by default or set by the user
  virtual QVTKInteractor* getInteractor();
    
  // Description:
  // Enum for additional event types supported.
  // These events can be picked up by command observers on the interactor
  enum vtkCustomEvents
  {
    ContextMenuEvent = vtkCommand::UserEvent + 100,
    DragEnterEvent,
    DragMoveEvent,
    DragLeaveEvent,
    DropEvent
  };


  // Description:
  // Use a 3DConnexion device. Initial value is false.
  // If VTK is not build with the TDx option, this is no-op.
  // If VTK is build with the TDx option, and a device is not connected,
  // a warning is emitted.
  // It is must be called before the first Render to be effective, otherwise
  // it is ignored.
  void setUseTDx(bool useTDx);
  bool getUseTDx() const;
  
  static const char* asciiToKeySym ( int i );
  static const char* qtKeyToKeySym (Qt::Key i);
  static void getKeyAndKeySymInformation ( const QKeyEvent* e, const char * & keysym, int & ascii_key );

  // Override base class
  void drawBackground(QPainter* painter, const QRectF& rect );

  bool isProcessingPaintEvent () const { return mProcessingPaintEvent; }

  const QCursor & vtkCursorToQtCursor( int iVtkCursor ) const;

  // Make the GL context current
  virtual void makeCurrent();

  void setCacheVtkBackground( bool value);
  bool cacheVtkBackground() const { return mCacheVtkBackground; }

signals:
  void sizeChanged( int x, int y);    // Emitted when the size of the window changes

public Q_SLOTS:

protected:

  // overloaded resize handler
  virtual void resizeEvent(QResizeEvent* event);
  // overloaded move handler
//  virtual void moveEvent(QMoveEvent* event);
  // overloaded paint handler
  virtual void paintEvent(QPaintEvent* event);

  // overloaded mouse press handler
  virtual void mousePressEvent(QMouseEvent* event);
  // overloaded mouse double click handler
  virtual void mouseDoubleClickEvent ( QMouseEvent * event);
  // overloaded mouse move handler
  virtual void mouseMoveEvent(QMouseEvent* event);
  // overloaded mouse release handler
  virtual void mouseReleaseEvent(QMouseEvent* event);
  // overloaded key press handler
  virtual void keyPressEvent(QKeyEvent* event);
  // overloaded key release handler
  virtual void keyReleaseEvent(QKeyEvent* event);
  // overloaded enter event
  virtual void enterEvent(QEvent*);
  // overloaded leave event
  virtual void leaveEvent(QEvent*);
#ifndef QT_NO_WHEELEVENT
  // overload wheel mouse event
  virtual void wheelEvent(QWheelEvent*);
#endif
  // overload focus event
//  virtual void focusInEvent(QFocusEvent*);
  // overload focus event
//  virtual void focusOutEvent(QFocusEvent*);
  // overload Qt's event() to capture more keys
//  bool event( QEvent* e );
    
  // overload context menu event
//  virtual void contextMenuEvent(QContextMenuEvent*);
  // overload drag enter event
  virtual void dragEnterEvent(QDragEnterEvent*);
  // overload drag move event
  virtual void dragMoveEvent(QDragMoveEvent*);
  // overload drag leave event
  virtual void dragLeaveEvent(QDragLeaveEvent*);
  // overload drop event
  virtual void dropEvent(QDropEvent*);

  vtkRenderWindowInteractor * GetRenderWindowInteractor();
protected: // attributes

  // the vtk render window
  bool UseTDx;
    
  enum MouseEventCategory { MouseEventCategory_None, MouseEventCategory_Item, MouseEventCategory_Background }; 
  MouseEventCategory mMouseEventCategory;
  bool mProcessingPaintEvent;

  // Cache the background graphics.
  // See QVTKGraphicsItem (VTK) for more.
  QGLFramebufferObject* mFBO;

  vtkQtOpenGLRenderWindow* mRenWin;
     
  // If true then a cached copy of the vtk image is used.
  bool mCacheVtkBackground;

  unsigned int mLastRenderMTime;
private:
  //! unimplemented operator=
  QVtkGraphicsView const& operator=(QVtkGraphicsView const&);
  //! unimplemented copy
  QVtkGraphicsView(const QVtkGraphicsView&);

};

#endif //Q_VTK_GRAPHICSVIEW_H
