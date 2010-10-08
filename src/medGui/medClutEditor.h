/* medClutEditor.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue May  5 12:22:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 14:09:46 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 15
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


// /////////////////////////////////////////////////////////////////
// medClutEditorVertex
// /////////////////////////////////////////////////////////////////

class medClutEditorVertexPrivate;
class medClutEditorTable;

class medClutEditorVertex : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem)

public:
     medClutEditorVertex(int x, int y, QColor color = Qt::yellow,
			 int upperBound = 0, QGraphicsItem *parent = 0);
    ~medClutEditorVertex(void);

    void setPrev( medClutEditorVertex * v );
    void setNext( medClutEditorVertex * v );
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    QRectF boundingRect(void) const;
    QPoint position(void) const;
    QColor color(void) const;
    int upperBound();

    void forceGeometricalConstraints();
    void interpolate();

    static bool LessThan(const medClutEditorVertex *v1,
  		       const medClutEditorVertex *v2) {
        return (v1->position().x() < v2->position().x());
    }

public slots:
    void onSetColorAction();
    void onDeleteAction();

signals:
    void deleteFromTable( medClutEditorVertex * );

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    // void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private :
    void setAlpha();
    medClutEditorVertexPrivate * d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditorTable
// /////////////////////////////////////////////////////////////////
class medClutEditorTablePrivate;

class medClutEditorTable : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem)

public:
     medClutEditorTable(QGraphicsItem *parent = 0);
    ~medClutEditorTable(void);

    void setAllowedArea( QRect  rect );
    void setAllowedArea( QRectF rect );
    QRect & allowedArea();
    void addVertex(medClutEditorVertex *vertex, bool interpolate = false);
    QRectF boundingRect(void) const;

    void setup(int min, int max, int size, int *table);
    void setupTransferFunction(QList<double> &scalars,QList<QColor> &colors );

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	       QWidget *widget = 0);


    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public slots:
    void onDeleteVertex(medClutEditorVertex * v);

private:
    void linkVertices();

    medClutEditorTablePrivate *d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditor
// /////////////////////////////////////////////////////////////////

class medClutEditorPrivate;

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
