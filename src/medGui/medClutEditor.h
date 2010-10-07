/* medClutEditor.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue May  5 12:22:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:19:50 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDCLUTEDITOR_H
#define MEDCLUTEDITOR_H

#include "medGuiExport.h"

#include <QtGui>

class dtkAbstractData;
class medAbstractView;

class medClutEditorPrivate;
class medClutEditorVertexPrivate;

class medClutEditorVertex : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
     medClutEditorVertex(int x, int y, QColor color = Qt::yellow,int upperBound = 0, QGraphicsItem *parent = 0);
    ~medClutEditorVertex(void);

    void setPrev( medClutEditorVertex * v );
    void setNext( medClutEditorVertex * v );
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    QRectF boundingRect(void) const;
    QPoint position(void) const;
    QColor color(void) const;
    int upperBound();

    void interpolate();

    static bool LessThan(const medClutEditorVertex *v1,
  		       const medClutEditorVertex *v2) {
        return (v1->position().x() < v2->position().x());
    }

 public slots:
    void onDeleteAction();
    void onSetColorAction();
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private :
    void setAlpha();
    medClutEditorVertexPrivate * d;
};


class MEDGUI_EXPORT medClutEditor : public QWidget
{
    Q_OBJECT

public:
     medClutEditor(QWidget *parent = 0);
    ~medClutEditor(void);

    void setData(dtkAbstractData *data);
    void setView(medAbstractView *view);

protected:
    void mousePressEvent(QMouseEvent *event);

protected slots:
    void onNewTableAction(void);
    void onClearTablesAction(void);
    void onApplyTablesAction(void);

private:
    medClutEditorPrivate *d;
};

#endif // MEDCLUTEDITOR_H
