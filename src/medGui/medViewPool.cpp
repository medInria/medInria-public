/* medDropSite.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 17:41:43 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:15:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 18
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewPool.h"

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

class medViewPoolPrivate
{
public:
  QList<dtkAbstractView *>                 views;
  QMap<dtkAbstractView*, dtkAbstractData*> viewData;
  QHash<QString, QString>                  propertySet;
};


medViewPool::medViewPool(void) : d (new medViewPoolPrivate)
{
}

medViewPool::~medViewPool(void)
{
    delete d;
    d = NULL;
}

void medViewPool::appendView (dtkAbstractView *view)
{
    if (!view || d->views.contains (view))
        return;

    d->views.append (view);
    connect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    connect (view, SIGNAL (sync(bool)),                    this, SLOT (onViewSync(bool)));
    connect (view, SIGNAL (syncWL(bool)),                  this, SLOT (onViewSyncWL(bool)));
  
	/*
    dtkAbstractView *refView = NULL;
    if (d->views.count()==1) {
        refView = view;
	refView->setProperty ("Daddy", "true");
	connect (this, SIGNAL (linkwl (dtkAbstractView *, bool)), refView, SLOT (linkwl (dtkAbstractView *, bool)));
    }
    else
        refView = this->daddy();
	 */

    // set properties
    QHashIterator<QString, QString> it(d->propertySet);
    while (it.hasNext()) {
        it.next();
	view->setProperty (it.key(), it.value());
    }
}

void medViewPool::removeView (dtkAbstractView *view)
{
    if (!view || !d->views.contains (view))
        return;

    // look if a view is a daddy
    dtkAbstractView *refView = this->daddy();

    if (refView) {
        if (refView==view) { // we are daddy, we need to unlink, find a new daddy, and link again (if needed)
	  
	    // unlink
	    QList<dtkAbstractView *>::iterator it = d->views.begin();
	    for( ; it!=d->views.end(); it++) {
	        refView->unlink ((*it));
		emit linkwl ((*it), false);
	    }
	    
	    // change daddy
	    it = d->views.begin();
	    for( ; it!=d->views.end(); it++)
	        if ((*it)!=refView && (*it)->property ("Daddy")=="false") {
		    (*it)->setProperty ("Daddy", "true");
		    break;
		}

	    dtkAbstractView *oldDaddy = refView;
	    oldDaddy->setProperty ("Daddy", "false");
	    disconnect (this, SIGNAL (linkwl (dtkAbstractView *, bool)), oldDaddy, SLOT (linkwl (dtkAbstractView *, bool)));
		refView = this->daddy();
		if (refView) {
			connect (this, SIGNAL (linkwl (dtkAbstractView *, bool)), refView, SLOT (linkwl (dtkAbstractView *, bool)));
		}
	}
	else { // we are not daddy, just unlink
	    refView->unlink (view);
	    emit linkwl (view, false);
	}
    }
    
    disconnect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    disconnect (view, SIGNAL (sync(bool)),                    this, SLOT (onViewSync(bool)));
    disconnect (view, SIGNAL (syncWL(bool)),                  this, SLOT (onViewSyncWL(bool)));
    d->views.removeOne (view);
}

dtkAbstractView *medViewPool::daddy (void)
{
    QList<dtkAbstractView *>::iterator it = d->views.begin();
    for( ; it!=d->views.end(); it++) {
        if ((*it)->property ("Daddy")=="true")
	  return (*it);
    }
    return NULL;
}

void medViewPool::onViewDaddy (bool daddy)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
      
        if (daddy) { // view wants to become daddy
	  
	    dtkAbstractView *refView = this->daddy();
	    
	    if (refView) { // if daddy, then unlink first
	        QList<dtkAbstractView *>::iterator it = d->views.begin();
		for( ; it!=d->views.end(); it++) {
		    refView->unlink((*it));
		    emit linkwl ((*it), false);
		}
	    }

	    if (refView)
	        disconnect (this, SIGNAL (linkwl (dtkAbstractView *, bool)), refView, SLOT (linkwl (dtkAbstractView *, bool)));

	    // tell all views that they are not daddys
	    QList<dtkAbstractView *>::iterator it = d->views.begin();
	    for( ; it!=d->views.end(); it++)
	        (*it)->setProperty ("Daddy", "false");

	    // tell the sender it is now daddy
	    view->setProperty ("Daddy", "true");
	    connect (this, SIGNAL (linkwl (dtkAbstractView *, bool)), view, SLOT (linkwl (dtkAbstractView *, bool)));

	}
	else { // view does not want to become daddy
	  if (view==this->daddy()) { // unlink if needed
	        QList<dtkAbstractView *>::iterator it = d->views.begin();
		  for( ; it!=d->views.end(); it++) {
		      view->unlink((*it));
			  emit linkwl ((*it), false);
		  }
	    }
	  
	    view->setProperty ("Daddy", "false");
	}
    }
}

void medViewPool::onViewSync (bool sync)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
      
        if (sync) {
	  dtkAbstractView *refView = this->daddy();

	  if (refView==view) // do not register the view with itself
	      return;
	  
	  if (refView) {

	    dtkAbstractData *data1 = static_cast<dtkAbstractData *>(refView->data());
	    dtkAbstractData *data2 = static_cast<dtkAbstractData *>(view->data());

	    dtkAbstractProcess *process = dtkAbstractProcessFactory::instance()->create("itkProcessRegistration");
	    if (!process)
	        return;
	  
	    process->setInput (data1, 0);
	    process->setInput (data2, 1);
	    if (process->run()==0) {
	        dtkAbstractData *output = process->output();
	        d->viewData[view] = data2;
		view->setData (output);
		view->update();
	    }

	    refView->link (view);
	  }
	}
	else {

	  if ( d->viewData[view] ) {
	    view->setData (d->viewData[view]);
	    d->viewData[view] = NULL;
	    view->update();

	    if (dtkAbstractView *refView = this->daddy())
	        refView->unlink (view);
	  }
	}	
    }  
}

void medViewPool::onViewSyncWL (bool value)
{
  if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
      emit linkwl (view, value);
      view->update();
  }
}

void medViewPool::setViewProperty (QString key, QString value)
{
    if (key=="Daddy")
        return;

    d->propertySet[key] = value;
    
    foreach (dtkAbstractView *lview, d->views) {
        if (lview!=this->sender()) {
	    lview->blockSignals (true);
	    lview->setProperty (key, value);
	    lview->update();
	    lview->blockSignals (false);
	}
    }
}

int medViewPool::count (void)
{
    return d->views.count();
}
