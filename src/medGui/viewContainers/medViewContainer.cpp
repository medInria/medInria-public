/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medViewContainer.h"
#include "medViewContainer_p.h"
#include "medViewPool.h"
#include <medDataManager.h>
#include <medViewManager.h>
#include <medAbstractView.h>
#include <medDataIndex.h>

#include <QtGui>

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractData.h>


medViewContainer::medViewContainer ( QWidget *parent )
        : QFrame ( parent )
        , d ( new medViewContainerPrivate )
{
    d->layout = new QGridLayout ( this );
    d->layout->setContentsMargins ( 0, 0, 0, 0 );
    d->layout->setSpacing ( 3 );

    d->view = NULL;
    d->current = this;
    d->clicked = false;
    d->multiLayer = true;

    d->pool = new medViewPool ( this );

    medViewContainer *container = qobject_cast<medViewContainer *>(parent);
    if ( container != NULL ) {
        connect(this,      SIGNAL(dropped(const medDataIndex&)),
                container, SIGNAL(dropped(const medDataIndex&)));
        connect(this,      SIGNAL(focused(dtkAbstractView*)),
                container, SIGNAL(focused(dtkAbstractView*)));
    }

    this->setAcceptDrops ( true );
    this->setFocusPolicy ( Qt::ClickFocus );
    this->setMouseTracking ( true );
    this->setSizePolicy ( QSizePolicy::Expanding,QSizePolicy::Expanding );
}

medViewContainer::~medViewContainer()
{
    if ( d->view )
    {
        d->view->close();
    }

    delete d;

    d = NULL;
}


const medViewContainer *medViewContainer::current() const
{
    const medViewContainer * root = this->root();
    if ( root != this )
        return root->current();

    return d->current;
}

medViewContainer *medViewContainer::current()
{
    medViewContainer * root = this->root();
    if ( root != this )
        return root->current();

    return d->current;
}

bool medViewContainer::isClicked() const
{
    return d->clicked;
}

bool medViewContainer::isCurrent() const
{
    return const_cast< medViewContainer * > ( this )->current() == this;
}

bool medViewContainer::isRoot() const
{
    return this->parentContainer() == NULL;
}

bool medViewContainer::isLeaf() const
{
    return false;
}

bool medViewContainer::isEmpty() const
{
    return ( this->view() == NULL &&
             this->childContainers().isEmpty());
}

bool medViewContainer::isDaddy() const
{
    return ( this->view() != NULL &&
             this->view()->property ( "Daddy" ) == "true" );
}

medViewContainer * medViewContainer::parentContainer() const
{
    return qobject_cast< medViewContainer * >( this->parentWidget() );
}

const medViewContainer * medViewContainer::root() const
{
    const medViewContainer * parent = this->parentContainer();
    return ( parent != NULL ? parent->root() : this );
}

medViewContainer * medViewContainer::root()
{
    medViewContainer * parent = this->parentContainer();
    return ( parent != NULL ? parent->root() : this );
}

QList< medViewContainer * > medViewContainer::childContainers() const
{
    QList< medViewContainer * > containers;
    foreach ( QObject * child, this->children() ) {
        medViewContainer * c = qobject_cast<medViewContainer *>( child );
        if ( c != NULL ) {
            containers << c;
            containers << c->childContainers();
        }
    }

    return containers;
}

QList< medViewContainer * > medViewContainer::leaves ( bool excludeEmpty )
{
    QList< medViewContainer * > leaves;
    if ( this->isLeaf() )
    {
        if ( ! ( excludeEmpty && this->isEmpty() ) )
            leaves << this;
    }
    else                   // a leaf doesn't contain another container
        foreach ( medViewContainer * child, this->childContainers() )
        leaves << child->leaves ( excludeEmpty );

    return leaves;
}

void medViewContainer::split ( int rows, int cols )
{
    Q_UNUSED ( rows );
    Q_UNUSED ( cols );
}

dtkAbstractView *medViewContainer::view() const
{
    return d->view;
}

QList<dtkAbstractView *> medViewContainer::views() const
{
    QList<dtkAbstractView *> views;
    if ( d->view )
        views << d->view;

    return views;
}

medViewPool *medViewContainer::pool()
{
    return d->pool;
}

void medViewContainer::setView ( dtkAbstractView *view )
{
    if (!isLeaf())
    {
        //go down to the actual currently selected container to set the view.
        current()->setView(view);
        return;
    }
    if (view==d->view)
        return;

    // clear connection of previous view
    if ( d->view )
    {
        disconnect ( view, SIGNAL ( changeDaddy ( bool ) ), this, SLOT ( onDaddyChanged ( bool ) ) );
        d->view->close();
        d->view = 0;
    }

    d->view = view;

    if ( d->view )
    {
        // pass properties to the view
        QHash<QString,QString>::iterator it = d->viewProperties.begin();
        while ( it!=d->viewProperties.end() )
        {
            view->setProperty ( it.key(), it.value() );
            ++it;
        }
        connect (view, SIGNAL(changeDaddy(bool)), this, SLOT(onDaddyChanged(bool)));
    }
    setFocus(Qt::MouseFocusReason);
}

void medViewContainer::onViewFocused ( bool value )
{
//    qDebug()<<"medViewerContainer::onViewFocused";

    if ( !value )
        return;
    if ( this ->acceptDrops()) // excluding containers that don't accept inputs (e.g Filtering result container)
        this->setCurrent ( this );

    if (!current() || !current()->view())
    { //focusing on an empty container, reset the toolboxes.
//        qDebug()<< "focusing on empty container";
        emit focused(NULL);
        return;
    }
    if (dtkAbstractView *view = current()->view())
    {
//        qDebug() << "focusing on view"<<view;
        emit focused(view);
    }
    this->update();
}

void medViewContainer::onContainerClicked()
{
    d->clicked = false;
    this->recomputeStyleSheet();
}

void medViewContainer::setCurrent ( medViewContainer *container )
{
    medViewContainer * parent =
        qobject_cast<medViewContainer *>( this->parentWidget() );
    if ( parent != NULL )
        parent->setCurrent ( container );
    else
        d->current = container;

}

void medViewContainer::recomputeStyleSheet()
{
    this->setStyleSheet ( qApp->styleSheet() );
}

void medViewContainer::dragEnterEvent ( QDragEnterEvent *event )
{
    event->acceptProposedAction();
}

void medViewContainer::dragMoveEvent ( QDragMoveEvent *event )
{
    event->acceptProposedAction();
}

void medViewContainer::dragLeaveEvent ( QDragLeaveEvent *event )
{
    event->accept();
}

void medViewContainer::dropEvent ( QDropEvent *event )
{
    const QMimeData *mimeData = event->mimeData();

    medDataIndex index = medDataIndex::readMimeData ( mimeData );
    if ( index.isValid() )
    {
        open ( index );
        
        emit dropped ( index );
    }

    event->acceptProposedAction();
      
}

void medViewContainer::focusInEvent ( QFocusEvent *event )
{
    Q_UNUSED ( event );

    medViewContainer * former = this->current();

    d->clicked = true;
    this->onViewFocused( true );
//    qDebug()<< this->isDaddy() << isEmpty() << isLeaf() << isClicked();
    this->recomputeStyleSheet();

    if ( former )
        former->update();

    emit clicked();
}

void medViewContainer::focusOutEvent ( QFocusEvent *event )
{
    Q_UNUSED(event);
    d->clicked = false;
    this->recomputeStyleSheet();
}

void medViewContainer::paintEvent ( QPaintEvent *event )
{
    if ( this->layout()->count() > 1 )
        return;

    // QFrame::paintEvent(event);

    // QColor borderColor = Qt::darkGray;
    // if ( this->current() == this )
    //     borderColor.setRgb( 0x9a, 0xb3, 0xd5 );

    // qreal borderWidth = 1;

    QPainter painter;
    painter.begin ( this );

    // painter.setPen( QPen( borderColor, borderWidth ) );
    // // painter.setBrush(QColor(0x38, 0x38, 0x38));
    // painter.setBrush( Qt::transparent );
    // painter.drawRect(this->rect().adjusted(0, 0, -1, -1));

    // if ( this->view() != NULL &&
    //      this->view()->property( "Daddy" ) == "true" ) {
    //     // painter.setPen( Qt::red );
    //     painter.setPen(
    //         QPen( QColor( 255, 0, 0, 127 ), borderWidth, Qt::DotLine ) );
    //     painter.setBrush( Qt::transparent );
    //     painter.drawRect( this->rect().adjusted( 0, 0, -1, -1 ) );
    // }

    if ( !this->view() && ( d->viewProperties.count() ||
                            !d->viewInfo.isEmpty() ) )
    {
        painter.setPen ( Qt::white );
        QFont font = painter.font();
        font.setPointSize ( 18 );
        painter.setFont ( font );
        QString text;

        //Add View Info:
        if ( !d->viewInfo.isEmpty() )
            //    Debug()<< "viewInfo" << d->viewInfo;
            text += d->viewInfo + "\n";

        QList<QString> keys = d->viewProperties.keys();
        foreach ( QString key, keys )
        text += d->viewProperties[key] + "\n";
        painter.drawText ( event->rect(), Qt::AlignCenter, text );
    }

    painter.end();
}

void medViewContainer::setViewProperty ( const QString &key, const QString &value )
{
    d->viewProperties[key] = value;
}

QString medViewContainer::viewProperty ( const QString &key ) const
{
    if ( d->viewProperties.contains ( key ) )
        return d->viewProperties[key];

    return QString();
}

void medViewContainer::onViewFullScreen ( bool value )
{
    Q_UNUSED ( value );
}

void medViewContainer::onDaddyChanged ( bool state )
{
    Q_UNUSED ( state );
    this->recomputeStyleSheet();
}

// void medViewContainer::clear()
// {
//     if ( d->view )
//     {
// // //         this->fu( false, d->view ); // in case view is full screen
//         d->layout->removeWidget ( d->view->widget() );
// // // //         this->desynchronize_2 ( d->view );
// //         disconnect ( d->view, SIGNAL ( closing() ),         this, SLOT ( onViewClosing() ) );
// // //         disconnect ( d->view, SIGNAL ( fullScreen ( bool ) ),  this, SLOT ( onViewFullScreen ( bool ) ) );
// // //         disconnect ( d->view, SIGNAL ( changeDaddy ( bool ) ), this, SLOT ( onDaddyChanged ( bool ) ) );
// //
//         emit viewRemoved ( d->view );
// //
//         d->view->close();
//         d->view = NULL;
//     }
//
//     foreach ( medViewContainer *container, this->childContainers() )
//     {
//         d->layout->removeWidget ( container );
//         container->deleteLater(); // safer than delete container
//     }
// }

void medViewContainer::setInfo ( const QString& info )
{
    d->viewInfo = info;
}

QString medViewContainer::info()
{
    return d->viewInfo;
}

void medViewContainer::setMultiLayer(bool enable)
{
    d->multiLayer = enable;
}

bool medViewContainer::multiLayer( void )
{
    return d->multiLayer;
}

bool medViewContainer::open(const medDataIndex& index)
{
    bool res = false;
    
    if(!index.isValid())
        return false;
        
    if( index.isValidForSeries() )
    {
        dtkSmartPointer<dtkAbstractData> data;

        data = medDataManager::instance()->data(index);
        
        res = open(data);   
        
        if(res)
        {
            // add the view to the viewManager
            dtkSmartPointer<medAbstractView> view = qobject_cast<medAbstractView*>(this->view());
            medViewManager::instance()->insert(index, view);
        }
    }
    
    return res;
}

bool medViewContainer::open(dtkAbstractData * data)
{
    if ( data == NULL )
        return false;


    dtkSmartPointer<medAbstractView> view = qobject_cast<medAbstractView*>(this->view());
    bool newView = view.isNull();

    if( newView)
    {
        //container empty, or multi with no extendable view
        view = qobject_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->createSmartPointer("v3dView"));
        connect (this, SIGNAL(sliceSelected(int)), view, SLOT(setSlider(int)));
    }

    if( view.isNull() ) {
        qWarning() << "medViewContainer: Unable to create a v3dView";
        return false;
    }

    // set the data to the view
    if (!this->multiLayer())
    {
        view->removeOverlay(0);
        view->setSharedDataPointer(data,0);
        newView = true;
    }
    else
    {
        view->setSharedDataPointer(data);
    }

    //set the view to the current container
    this->setView(view);
    //only call reset if the view is a new one or with only one layer.
    if (newView)
    {
        qDebug() << "medViewContainer: Reset view";
        view->reset();
    }
    view->update();

    return true;
}

