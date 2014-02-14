/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainer.h>
#include <medViewContainer_p.h>
#include <medDataManager.h>
#include <medViewManager.h>
#include <medAbstractView.h>
#include <medAbstractImageView.h>
#include <medAbstractData.h>
#include <medDataIndex.h>

#include <QtGui>

#include <medImageViewFactory.h>

#include <medAbstractData.h>


medViewContainer::medViewContainer ( QWidget *parent )
        : QFrame ( parent )
        , d ( new medViewContainerPrivate )
{
    d->layout = new QGridLayout ( this );
    d->layout->setContentsMargins ( 0, 0, 0, 0 );
    d->layout->setSpacing ( 0 );

    d->view = NULL;
    d->current = this;
    d->selected = false;
    d->multiLayer = true;

    medViewContainer *container = qobject_cast<medViewContainer *>(parent);
    if ( container != NULL ) {
        connect(this,      SIGNAL(imageSet(const medDataIndex&)),
                container, SIGNAL(imageSet(const medDataIndex&)));
        connect(this,      SIGNAL(focused(medAbstractView*)),
                container, SIGNAL(focused(medAbstractView*)));
    }

    this->setAcceptDrops ( true );
    this->setFocusPolicy ( Qt::ClickFocus );
    this->setMouseTracking ( true );
    this->setSizePolicy ( QSizePolicy::Expanding,QSizePolicy::Expanding );

}

medViewContainer::~medViewContainer()
{
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

bool medViewContainer::isSelected() const
{
    return d->selected;
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

medAbstractView *medViewContainer::view() const
{
    return d->view;
}

QList<medAbstractView *> medViewContainer::views() const
{
    QList<medAbstractView *> views;
    if ( d->view )
        views << d->view;

    return views;
}


void medViewContainer::setView(medAbstractView *view )
{
    if (view == d->view)
        return;

    if (!isLeaf())
    {
        //go down to the actual currently selected container to set the view.
        current()->setView(view);
        return;
    }

    // clear connection of previous view
    if (d->view)
    {
        // disconnect all conenctions from this class to the view
        disconnect(d->view, 0, this, 0 );
        delete d->view;
        d->view = NULL;
    }

    d->view = view;
    d->view->setParent(this);

    d->layout->setContentsMargins(0, 0, 0, 0);
    d->layout->addWidget(view->widget(), 0, 0);
//    d->view->widget()->show();

    this->recomputeStyleSheet();
    emit viewAdded (view);

    setFocus(Qt::MouseFocusReason);
}

void medViewContainer::select()
{
    qDebug() << "SELECT !!!";

    if(!d->view)
    {
        qDebug() << "no view !!!!";
        return;
    }

    d->selected = true;

    medViewManager::instance()->addToSelection(d->view);
    this->recomputeStyleSheet();

    emit focused(d->view);
    emit selected();
}

void medViewContainer::unselect()
{
    if(QApplication::keyboardModifiers() != Qt::ControlModifier)
    {
        d->selected = false;
        this->recomputeStyleSheet();

        medAbstractView *medView = dynamic_cast<medAbstractView*>(this->view());
        medViewManager::instance()->removeFromSelection(medView);
    }
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
    this->style()->unpolish(this);
    this->style()->polish(this);
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
    }

    event->acceptProposedAction();
      
}

void medViewContainer::focusInEvent ( QFocusEvent *event )
{
    Q_UNUSED ( event );

    this->select();
}

void medViewContainer::focusOutEvent ( QFocusEvent *event )
{
    Q_UNUSED(event);
}

void medViewContainer::paintEvent ( QPaintEvent *event )
{
//    if ( this->layout()->count() > 1 )
//        return;

//    QPainter painter;
//    painter.begin ( this );

//    if (!this->view()
//            && (d->viewProperties.count()
//                || !d->viewInfo.isEmpty()))
//    {
//        painter.setPen (Qt::white);
//        QFont font = painter.font();
//        font.setPointSize (18);
//        painter.setFont (font);
//        QString text;

//        //Add View Info:
//        if ( !d->viewInfo.isEmpty() )
//            //    Debug()<< "viewInfo" << d->viewInfo;
//            text += d->viewInfo + "\n";

//        QList<QString> keys = d->viewProperties.keys();
//        foreach ( QString key, keys )
//        text += d->viewProperties[key] + "\n";
//        painter.drawText ( event->rect(), Qt::AlignCenter, text );
//    }

//    painter.end();
    QFrame::paintEvent(event);
}

void medViewContainer::onViewFullScreen ( bool value )
{
    Q_UNUSED ( value );
}

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

    if(!index.isValidForSeries())
        return res;

    if(!this->acceptDrops())
        return res;

    if( this != this->current())
    {
        // opening should be done by current container
        res = this->current()->open(index);
        return res;
    }

    dtkSmartPointer<medAbstractData> data;
    data = medDataManager::instance()->data(index);
    res = this->open(data);

    return res;
}

bool medViewContainer::open(medAbstractData* data)
{
    if (data == NULL)
        return false;

    medAbstractImageView* view = qobject_cast<medAbstractImageView*>(this->view());

    bool newView = !(view && this->multiLayer());

    if(newView)
    {
        //container empty, or multi with no extendable view
        view = medImageViewFactory::instance()->createView("medVtkView", this);
        if(!view)
        {
            qWarning() << "medViewContainer: Unable to create a medVtkView";
            return false;
        }
        this->setView(view);
        medViewManager::instance()->insert(data->dataIndex(), view);
        connect(view, SIGNAL(destroyed()), this, SLOT(removeInternView()));
    }

    view->addLayer(data);
    // in order to update the toolboxes, when a new data type has been added for example
    setFocus(Qt::MouseFocusReason);
    emit imageSet(data->dataIndex());
    return true;
}



void medViewContainer::keyPressEvent(QKeyEvent * event)
{
    if( event->key() == Qt::Key_Control)
    {
        this->root()->setMultiSelection(true);
    }
}

void medViewContainer::keyReleaseEvent(QKeyEvent * event)
{
    if(event->matches(QKeySequence::SelectAll))
    {
        foreach(medViewContainer* container, this->root()->childContainers())
        {
            if(container != this)
              container->select();
        }
    }

  else this->root()->setMultiSelection(false);
}

void medViewContainer::setMultiSelection(bool enabled)
{
    d->multiSelectionEnabled = enabled;
}

bool medViewContainer::multiSelection()
{
    return d->multiSelectionEnabled;
}

void medViewContainer::removeInternView()
{
    qDebug()<<"set view to null in container";
    d->view = NULL;
}
