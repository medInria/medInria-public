/*=========================================================================

  Program:   Visualization Toolkit
  Module:    v3dViewGraphicsView.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "v3dViewGraphicsView.h"

#include <QMouseEvent>

#include "v3dViewGraphicsScene.h"

class v3dViewGraphicsViewPrivate
{
public:
    v3dViewGraphicsScene * scene;
};

/*! constructor */
v3dViewGraphicsView::v3dViewGraphicsView( QWidget * p )
    : BaseClass(p)
    , d( new v3dViewGraphicsViewPrivate )
{
    this->setRenderHints( QPainter::Antialiasing          |
                          QPainter::SmoothPixmapTransform |
                          QPainter::TextAntialiasing      );
    d->scene = NULL;
}

/*! destructor */
v3dViewGraphicsView::~v3dViewGraphicsView()
{
    delete d;
}

void v3dViewGraphicsView::setScene( v3dViewGraphicsScene * scene )
{
    BaseClass::setScene( scene );
    d->scene = scene;
}


void v3dViewGraphicsView::mousePressEvent( QMouseEvent* e )
{
    // Trigger context menu in some cases
    if ( e->button() == Qt::RightButton && !this->itemAt(e->pos()) )
        emit this->mousePressed( e );
    else
        BaseClass::mousePressEvent( e );
}

