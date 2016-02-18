/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewEventFilter.h>

#include <medAbstractView.h>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <medAbstractData.h>
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

//! Implement dtkAbstractObject.
QString medViewEventFilter::description(void) const
{
    return s_description();
}

//! Implement dtkAbstractObject.
QString medViewEventFilter::identifier(void) const
{
    return s_description();
}

/** Event handlers, derived classes should override those they need.
 *  \return     true if the event was processed and should not be passed to the next handler.
 *  The default implementations do nothing, returning false. */
bool medViewEventFilter::mousePressEvent( medAbstractView * vscene, QGraphicsSceneMouseEvent *mouseEvent )
{
    return false;
}

/** Event handlers, derived classes should override those they need.
 *  \return     true if the event was processed and should not be passed to the next handler.
 *  The default implementations do nothing, returning false. */
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

bool medViewEventFilter::mouseWheelEvent( medAbstractView *vscene, QGraphicsSceneWheelEvent *wheelEvent )
{
    return false;
}

bool medViewEventFilter::mouseWheelEvent( medAbstractView *view, QWheelEvent *wheelEvent )
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
    case (QEvent::Wheel) :
        {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent *>(event);
            return this->mouseWheelEvent( view, wheelEvent );
        }
    default:
        {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
    }
}

//! Installs the eventFilter(this) in the given view
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

//! Remove the eventFilter(this) in the given view
void medViewEventFilter::removeFromView(medAbstractView * view)
{
    if ( m_views.contains(view)) {
        QObject * filterObj = medViewEventFilter::objectToFilter(view);
        filterObj->removeEventFilter( this );
        m_views.remove(view);
        disconnect(view, SIGNAL(destroyed(QObject*)), this, SLOT(onViewDestroyed(QObject*)));
    }
}

//! Remove this from all views it has been installed on.
void medViewEventFilter::removeFromAllViews()
{
    while( !m_views.isEmpty() ) {
        this->removeFromView(*(m_views.begin()) );
    }
}

//! Which object to actually filter given the input view.
QObject * medViewEventFilter::objectToFilter( medAbstractView * view )
{
    return view->viewWidget();
}

void medViewEventFilter::onViewDestroyed( QObject* obj)
{
    // Cannot use dynamic casting because the object is being destroyed.
    // For same reason, we cannot call this->removeFromView.
    medAbstractView*view = static_cast<medAbstractView*>(obj);
    m_views.remove(view);
}












