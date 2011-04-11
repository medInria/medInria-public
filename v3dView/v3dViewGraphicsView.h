/*=========================================================================

  Program:   Visualization Toolkit
  Module:    v3dViewGraphicsView.h

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

// .NAME v3dViewGraphicsView - Display a vtkRenderWindow in a Qt's QWidget.
// .SECTION Description
// v3dViewGraphicsView provides a way to display VTK data in a Qt widget.

#ifndef V3DVIEWGRAPHICSVIEW_H
#define V3DVIEWGRAPHICSVIEW_H


#include "QVtkGraphicsView.h" 

class v3dViewGraphicsScene;
class v3dViewGraphicsViewPrivate;

//! v3dViewGraphicsView specializes the graphics view for use with the v3dview
class v3dViewGraphicsView : public QVtkGraphicsView
{
  typedef QVtkGraphicsView BaseClass;
  Q_OBJECT;

public:
  //! constructor
  v3dViewGraphicsView(QWidget* parent = NULL);
  //! destructor
  virtual ~v3dViewGraphicsView();

  void setScene( v3dViewGraphicsScene * scene );

signals:
  void mousePressed(QMouseEvent*);

protected:
  // overloaded mouse press handler
  virtual void mousePressEvent(QMouseEvent* event);

private:
  //! unimplemented operator=
  v3dViewGraphicsView const& operator=(v3dViewGraphicsView const&);
  //! unimplemented copy
  v3dViewGraphicsView(const v3dViewGraphicsView&);

  v3dViewGraphicsViewPrivate * d;
};

#endif //V3DVIEWGRAPHICSVIEW_H
