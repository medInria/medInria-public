#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractObject.h>

#include <QMouseEvent>
#include <QWheelEvent>

#include <medCoreExport.h>

//Forward declarations
class medAbstractView;
class medAbstractViewScene;
class medSegmentationSelectorToolBox;
class QMouseEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;

/** class medViewEventFilter
 * This class can be added as an EventFilter on a medAbstractView.
 * The class allows the retreival of the mouse clicks, including where the mouse was clicked in the coordinates defined by the view.
 *
 * (see QObject::eventFilter & QObject::installEventFilter)
 */
class MEDCORE_EXPORT medViewEventFilter : public dtkAbstractObject
{
    Q_OBJECT

public:
    medViewEventFilter(dtkAbstractObject * parent = nullptr);
    virtual ~medViewEventFilter();

    virtual QString description() const;
    virtual QString identifier() const;

    void installOnView(medAbstractView * view);

    void removeFromView(medAbstractView * view);

    void removeFromAllViews();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

    virtual bool mousePressEvent( medAbstractView *view, QMouseEvent *mouseEvent );
    virtual bool mouseReleaseEvent( medAbstractView *view, QMouseEvent *mouseEvent );
    virtual bool mouseMoveEvent( medAbstractView *view, QMouseEvent *mouseEvent );
    virtual bool mouseWheelEvent( medAbstractView *view, QWheelEvent *wheelEvent );

    virtual bool mousePressEvent( medAbstractView *view, QGraphicsSceneMouseEvent *mouseEvent );
    virtual bool mouseReleaseEvent( medAbstractView *view, QGraphicsSceneMouseEvent *mouseEvent );
    virtual bool mouseMoveEvent( medAbstractView *view, QGraphicsSceneMouseEvent *mouseEvent );
    virtual bool mouseWheelEvent( medAbstractView *view, QGraphicsSceneWheelEvent *wheelEvent );

    static QObject * objectToFilter( medAbstractView * view );

protected slots:
    void onViewDestroyed(QObject*);

private:
    static QString s_description();

    medSegmentationSelectorToolBox * m_segmentationController;
    QSet<medAbstractView *> m_views;
    typedef QHash<QObject *, medAbstractView *> FilterObjToViewType;
    FilterObjToViewType m_filterObjToView;
};
