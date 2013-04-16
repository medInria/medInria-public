/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#include "medViewEventFilter.h"

#include <medAbstractView.h>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkLog/dtkLog.h>

#include <QObject>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>

// /////////////////////////////////////////////////////////////////
// medViewSegmentationPrivate
// /////////////////////////////////////////////////////////////////

class medViewSegmentationPrivate
{
public:
};

// /////////////////////////////////////////////////////////////////
// View
// /////////////////////////////////////////////////////////////////
QString medViewEventFilter::s_description()
{
    static QString name = "medViewSegmentation";
    return name;
}

medViewEventFilter::medViewEventFilter(dtkAbstractObject * parent)
    : dtkAbstractObject(parent)
{
}

medViewEventFilter::~medViewEventFilter(void)
{
    removeFromAllViews();
}

QString medViewEventFilter::description(void) const
{
    return s_description();
}

QString medViewEventFilter::identifier(void) const
{
    return s_description();
}

bool medViewEventFilter::mousePressEvent( medAbstractView * vscene, QGraphicsSceneMouseEvent *mouseEvent )
{
    return false;
}

bool medViewEventFilter::mousePressEvent( medAbstractView *view, QMouseEvent *mouseEvent )
{
    return false;
}

bool medViewEventFilter::mouseReleaseEvent( medAbstractView * vscene, QGraphicsSceneMouseEvent *mouseEvent )
{
    return false;
}

bool medViewEventFilter::mouseReleaseEvent( medAbstractView *view, QMouseEvent *mouseEvent )
{
    return false;
}

bool medViewEventFilter::mouseMoveEvent( medAbstractView * vscene, QGraphicsSceneMouseEvent *mouseEvent )
{
    return false;
}

bool medViewEventFilter::mouseMoveEvent( medAbstractView *view, QMouseEvent *mouseEvent )
{
    return false;

}



bool medViewEventFilter::eventFilter( QObject *obj, QEvent *event )
{
    FilterObjToViewType::const_iterator it( m_filterObjToView.find( obj ) );

    medAbstractView * view = NULL;
    if ( it == m_filterObjToView.end()) {
        dtkWarn() <<  __FILE__ << __LINE__ << " : Filtering events on unknown QObject";
        return false;
    }
    view = it.value();

    // Note : QEvent is not derived from QObject.
    // Using static_cast instead of dynamic_cast for speed of interaction.
    switch (event->type()) {
    case ( QEvent::GraphicsSceneMousePress) :
        {
            QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
            return this->mousePressEvent( view, mouseEvent );
        }
    case ( QEvent::GraphicsSceneMouseMove) :
        {
            QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
            return this->mouseMoveEvent( view, mouseEvent );
        }
    case ( QEvent::GraphicsSceneMouseRelease ) :
        {
            QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
            return this->mouseReleaseEvent( view, mouseEvent );
        }
    case ( QEvent::MouseButtonPress) :
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent *>(event);
            return this->mousePressEvent( view, mouseEvent );
        }
    case ( QEvent::MouseMove) :
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent *>(event);
            return this->mouseMoveEvent( view, mouseEvent );
        }
    case ( QEvent::MouseButtonRelease ) :
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent *>(event);
            return this->mouseReleaseEvent( view, mouseEvent );
        }
    default:
        {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
    }
}

void medViewEventFilter::installOnView( medAbstractView * view )
{
    if ( !view ) {
        dtkWarn() << "medViewEventFilter::installOnView : View is NULL";
        return;
    }

    if (m_views.contains(view)) {
        dtkWarn() << "Installing View when it has already been installed";
        return;
    }

    QObject * filterObj = medViewEventFilter::objectToFilter(view);

    m_views.insert(view);
    m_filterObjToView[filterObj] = view;

    filterObj->installEventFilter( this );

    connect(view, SIGNAL(destroyed(QObject*)), this, SLOT(onViewDestroyed(QObject*)));
}

void medViewEventFilter::removeFromView(medAbstractView * view)
{
    if ( m_views.contains(view)) {
        QObject * filterObj = medViewEventFilter::objectToFilter(view);
        filterObj->removeEventFilter( this );
        m_views.remove(view);
        disconnect(view, SIGNAL(destroyed(QObject*)), this, SLOT(onViewDestroyed(QObject*)));
    }
}

void medViewEventFilter::removeFromAllViews()
{
    while( !m_views.isEmpty() ) {
        this->removeFromView(*(m_views.begin()) );
    }
}



QObject * medViewEventFilter::objectToFilter( medAbstractView * view )
{
    return view->receiverWidget();
}

void medViewEventFilter::onViewDestroyed( QObject* obj)
{
    // Cannot use dynamic casting because the object is being destroyed.
    // For same reason, we cannot call this->removeFromView.
    medAbstractView*view = static_cast<medAbstractView*>(obj);
    m_views.remove(view);
}



