/* medViewContainerCustom.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 11:01:46 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 16:28:13 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 67
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewContainer_p.h"
#include "medViewContainerCustom.h"
#include "medViewPool.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <medCore/medAbstractView.h>
#include <medCore/medViewManager.h>

class medViewContainerCustomPrivate
{
public:
    QList<medViewContainerCustom*> children;
    int rowMax;
    int columnMax;
    int preset;
};

medViewContainerCustom::medViewContainerCustom (QWidget *parent) : medViewContainer(parent), d2 (new medViewContainerCustomPrivate)
{
    d2->rowMax    = 5;
    d2->columnMax = 5;
    d2->preset = medViewContainerCustom::A;
}

medViewContainerCustom::~medViewContainerCustom()
{
    delete d2;
    d2 = NULL;
}

medViewContainer::Type medViewContainerCustom::type(void)
{
    return medViewContainer::Custom;
}

void medViewContainerCustom::split(int rows, int cols)
{
    if (d->view)
        return;
    
    this->clear();
    
    for(int i = 0 ; i < rows ; i++) {
        d->layout->setRowStretch(i, 0);
        for(int j = 0 ; j < cols ; j++) {
            medViewContainerCustom *container = new medViewContainerCustom(this);
	    connect (container, SIGNAL(viewAdded(dtkAbstractView*)),   this, SIGNAL (viewAdded(dtkAbstractView*)));
	    connect (container, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
            d2->children.append (container);
            d->layout->addWidget(container, i, j);
            d->layout->setColumnStretch(j, 0);
        }
    }
    
    // in split, the preset is no valid anymore
    d2->preset = 0;

    this->setCurrent(NULL);
}

void medViewContainerCustom::setPreset(int preset)
{
    if (d2->preset == preset)
        return;

    d2->preset = preset;
  
    this->clear();
    
    medViewContainerCustom *custom1;
    medViewContainerCustom *custom2;
    medViewContainerCustom *custom3;
    medViewContainerCustom *custom4;

    switch(preset) {
    case B:
        custom1 = new medViewContainerCustom(this);
	custom2 = new medViewContainerCustom(this);
	connect (custom1, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom1, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	d2->children.append (custom1);
	d2->children.append (custom2);
        d->layout->addWidget(custom1, 0, 0);
        d->layout->addWidget(custom2, 1, 0);
	d->layout->setRowStretch(0, 0);
	d->layout->setRowStretch(1, 0);						
        break;
    case C:
        custom1 = new medViewContainerCustom(this);
        custom1->split(2, 1);
        custom2 = new medViewContainerCustom(this);
	connect (custom1, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom1, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	d2->children.append (custom1);
	d2->children.append (custom2);
        d->layout->addWidget(custom1, 0, 0);
        d->layout->addWidget(custom2, 0, 1);
        d->layout->setColumnStretch(0, 1);
        d->layout->setColumnStretch(1, 2);
        break;
    case D:
        custom1 = new medViewContainerCustom(this);
        custom1->split(3, 1);
        custom2 = new medViewContainerCustom(this);
	connect (custom1, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom1, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	d2->children.append (custom1);
	d2->children.append (custom2);
        d->layout->addWidget(custom1, 0, 0);
        d->layout->addWidget(custom2, 0, 1);
        d->layout->setColumnStretch(0, 1);
        d->layout->setColumnStretch(1, 2);
        break;
    case E:
        custom1 = new medViewContainerCustom(this);
	custom2 = new medViewContainerCustom(this);
	custom3 = new medViewContainerCustom(this);
	custom4 = new medViewContainerCustom(this);

	connect (custom1, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom3, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom4, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom1, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	connect (custom3, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	connect (custom4, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));

	custom1->setViewProperty ("Orientation", "Axial");
	custom2->setViewProperty ("Orientation", "Sagittal");
	custom3->setViewProperty ("Orientation", "Coronal");
	custom4->setViewProperty ("Orientation", "3D");

	d2->children.append (custom1);
	d2->children.append (custom2);
	d2->children.append (custom3);
	d2->children.append (custom4);

	d->layout->addWidget(custom1, 0, 0);
	d->layout->addWidget(custom2, 0, 1);
	d->layout->addWidget(custom3, 1, 0);
	d->layout->addWidget(custom4, 1, 1);
	d->layout->setColumnStretch(0, 0);
	d->layout->setColumnStretch(1, 0);			
	d->layout->setRowStretch(0, 0);
	d->layout->setRowStretch(1, 0);
        break;
	
    case A:
	default:
        custom1 = new medViewContainerCustom(this);
        custom2 = new medViewContainerCustom(this);
	connect (custom1, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewAdded(dtkAbstractView*)), this, SIGNAL (viewAdded(dtkAbstractView*)));
	connect (custom1, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	connect (custom2, SIGNAL(viewRemoved(dtkAbstractView*)), this, SIGNAL (viewRemoved(dtkAbstractView*)));
	d2->children.append (custom1);
	d2->children.append (custom2);
        d->layout->addWidget(custom1, 0, 0);
        d->layout->addWidget(custom2, 0, 1);
	d->layout->setColumnStretch(0, 0);
	d->layout->setColumnStretch(1, 0);			
        break;

    };

    this->setCurrent(NULL);
}

void medViewContainerCustom::setView(dtkAbstractView *view)
{ 
    if (d2->children.count()==0) {
      if (view!=d->view) {
	if (d->layout->count())
	  d->layout->removeItem(d->layout->itemAt(0));

	if (d->view)
	  this->onViewClosing();

	dtkAbstractView *cloneView = dtkAbstractViewFactory::instance()->create (view->description());
	cloneView->setData ( static_cast<dtkAbstractData*>(view->data()) );
	cloneView->reset();
	
	medViewContainer::setView (cloneView);

	d->layout->setContentsMargins(1, 1, 1, 1);    
	d->layout->addWidget(cloneView->widget(), 0, 0);
	
	d->view = cloneView;
	// d->view->reset();
	
	this->synchronize_2 (cloneView);
	
	connect (cloneView, SIGNAL (closing()), this, SLOT (onViewClosing()));
	connect (cloneView, SIGNAL (fullScreen(bool)), this, SLOT (onViewFullScreen(bool)));

	emit viewAdded (cloneView);
      }
    }
    else {
      foreach (medViewContainerCustom *container, d2->children)
	container->setView (view);
    }    
}

dtkAbstractView *medViewContainerCustom::view (void) const
{
    return d->view;
}

QList<dtkAbstractView *> medViewContainerCustom::views (void) const
{
    QList<dtkAbstractView *> views;
    if (d2->children.count()==0) {
        if (d->view)
	  views << d->view;
    }
    else {
      foreach (medViewContainerCustom *container, d2->children)
	  views << container->views();
    }
    return views;
}

void medViewContainerCustom::synchronize_2 (dtkAbstractView *view)
{
    if (medViewContainerCustom *parent = dynamic_cast<medViewContainerCustom*>(this->parent())) {
        parent->synchronize_2(view);
    }
    else { // top level medViewContainerCustom
        if (medAbstractView *medView = dynamic_cast<medAbstractView*> (view) )
            d->pool->appendView (medView);
	connect (view, SIGNAL (becomeDaddy(bool)), this, SLOT (repaint()));
    }
}

void medViewContainerCustom::desynchronize_2 (dtkAbstractView *view)
{
    if (medViewContainerCustom *parent = dynamic_cast<medViewContainerCustom*>(this->parent())) {
        parent->desynchronize_2(view);
    }
    else { // top level medViewContainerCustom
        if (medAbstractView *medView = dynamic_cast<medAbstractView*> (view) )
            d->pool->removeView (medView);
	disconnect (view, SIGNAL (becomeDaddy(bool)), this, SLOT (repaint()));
    }
}

void medViewContainerCustom::onViewClosing (void)
{
    if (d->view) {
        this->onViewFullScreen2 (false, d->view); // in case view is full screen
        d->layout->removeWidget (d->view->widget());
        this->desynchronize_2 (d->view);
        disconnect (d->view, SIGNAL (closing()), this, SLOT (onViewClosing()));
	disconnect (d->view, SIGNAL (fullScreen(bool)), this, SLOT (onViewFullScreen(bool)));

	emit viewRemoved (d->view);
	
        d->view->close();
        
        d->view = NULL;
    }
}

void medViewContainerCustom::onViewFullScreen (bool value)
{
    if (medViewContainerCustom *parent = dynamic_cast<medViewContainerCustom*>(this->parent())) {
        parent->onViewFullScreen2 (value, dynamic_cast<dtkAbstractView *>(this->sender()) );
    }
    else { // top level medViewContainerCustom
        this->fullScreen (value, dynamic_cast<dtkAbstractView *>(this->sender()));
    }
}

void medViewContainerCustom::onViewFullScreen2 (bool value, dtkAbstractView *view)
{
    if (medViewContainerCustom *parent = dynamic_cast<medViewContainerCustom*>(this->parent())) {
        parent->onViewFullScreen2 (value, view );
    }
    else { // top level medViewContainerCustom
        this->fullScreen (value, view);
    }
}

void medViewContainerCustom::fullScreen (bool value, dtkAbstractView *view)
{
  if (d2->children.count()==0) { // no children = end widget
      if (!d->view ||(d->view && d->view!=view)) {
	  if (value)
	    this->hide();
	  else
	    this->show();
      }
  }

  foreach (medViewContainerCustom *custom, d2->children)
      custom->fullScreen (value, view);
}

void medViewContainerCustom::dragEnterEvent(QDragEnterEvent *event)
{
    this->setAttribute(Qt::WA_UpdatesDisabled, true);

    medViewContainer::dragEnterEvent(event);
}

void medViewContainerCustom::dragMoveEvent(QDragMoveEvent *event)
{
    medViewContainer::dragMoveEvent(event);
}

void medViewContainerCustom::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->setAttribute(Qt::WA_UpdatesDisabled, false);

    medViewContainer::dragLeaveEvent(event);
}

void medViewContainerCustom::dropEvent(QDropEvent *event)
{
    this->setCurrent(this);

    this->setAttribute(Qt::WA_UpdatesDisabled, false);

    medViewContainer::dropEvent(event);
}

void medViewContainerCustom::focusInEvent(QFocusEvent *event)
{
    medViewContainer::focusInEvent(event);
}

void medViewContainerCustom::focusOutEvent(QFocusEvent *event)
{
    medViewContainer::focusOutEvent(event);
}

void medViewContainerCustom::clear (void)
{
    if (d->view)
        this->onViewClosing();
  
    foreach (medViewContainerCustom *custom, d2->children) {
        custom->clear();
	delete custom;
	d->layout->removeWidget (custom);
    }
    d2->children.clear();

    for(int i=0; i<d2->rowMax; i++)
        d->layout->setRowStretch (i, 0);
    
    for(int i=0; i<d2->columnMax; i++)
        d->layout->setColumnStretch (i, 0);
}
