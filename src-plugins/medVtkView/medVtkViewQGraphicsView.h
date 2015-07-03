/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QGraphicsView>
#include <medVtkViewPluginExport.h>

class QVTKGraphicsItem;

class medVtkViewQGraphicsViewPrivate;
class MEDVTKVIEWPLUGIN_EXPORT medVtkViewQGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    medVtkViewQGraphicsView(QWidget * parent = NULL);
    virtual ~medVtkViewQGraphicsView();

    void setQVtkGraphicsItem(QVTKGraphicsItem *vtkItem);
    QVTKGraphicsItem* qVtkGraphicsItem() const;
    

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    medVtkViewQGraphicsViewPrivate *d;

};
