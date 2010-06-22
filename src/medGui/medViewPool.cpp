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
  int                                      synchronize;
};


medViewPool::medViewPool(void) : d (new medViewPoolPrivate)
{
    d->synchronize = 0;
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
    connect (view, SIGNAL (propertySet(QString, QString)), this, SLOT (onViewPropertySet(QString, QString)));
    connect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    connect (view, SIGNAL (sync(bool)),                    this, SLOT (onViewSync(bool)));
  
    dtkAbstractView *refView = NULL;
    if (d->views.count()==1) {
        refView = view;
	refView->setProperty ("Daddy", "true");
    }
    else
        refView = this->daddy();
    
    if (d->synchronize && refView)
        refView->link (view);
}

void medViewPool::removeView (dtkAbstractView *view)
{
    if (!view || !d->views.contains (view))
        return;

    // look if a view is a daddy
    dtkAbstractView *refView = this->daddy();

    if (refView) {

      if (refView==view) { // we are daddy
	if (d->synchronize) { // we are synchronized
	  QList<dtkAbstractView *>::iterator it = d->views.begin();
	  for( ; it!=d->views.end(); it++) {
	    if ((*it)->property ("Daddy")=="false") {
	      (*it)->setProperty ("Daddy", "true");
	      break;
	    }
	  }
	}
      }
      else
	  refView->unlink (view);
    }

    disconnect (view, SIGNAL (propertySet(QString, QString)), this, SLOT (onViewPropertySet(QString, QString)));
    disconnect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    disconnect (view, SIGNAL (sync(bool)),                    this, SLOT (onViewSync(bool)));
    d->views.removeOne (view);
}

dtkAbstractView *medViewPool::daddy (void)
{
    dtkAbstractView *daddy = NULL;
    QList<dtkAbstractView *>::iterator it = d->views.begin();
    for( ; it!=d->views.end(); it++) {
        if ((*it)->property ("Daddy")=="true") {
	  daddy = (*it);
	  break;
	}
    }
    return daddy;
}

int medViewPool::synchronized()
{
    return d->synchronize;
}

void medViewPool::synchronize (void)
{
    d->synchronize = 1;
    if (d->views.count()==0)
        return;
    
    // look if a view is a daddy
    dtkAbstractView *refView = this->daddy();

    QList<dtkAbstractView *>::iterator it = d->views.begin();
    if (refView) {
        for (it; it!=d->views.end(); ++it) {
	    refView->link ( (*it) );
	}
    }
}

void medViewPool::desynchronize (void)
{
    dtkAbstractView *refView = this->daddy();

    if (refView) {
        QList<dtkAbstractView *>::iterator it = d->views.begin();
	for (it; it!=d->views.end(); ++it) {
	    refView->unlink ( (*it) );
	}
    }

    d->synchronize = 0;
}

void medViewPool::onViewDaddy (bool daddy)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {

      if (daddy) {
		  
		  dtkAbstractView *refView = this->daddy();
		  if (refView && d->synchronize) {
			  QList<dtkAbstractView *>::iterator it = d->views.begin();
			  for( ; it!=d->views.end(); it++)
				  refView->unlink((*it));
		  }
		  
	    QList<dtkAbstractView *>::iterator it = d->views.begin();
	    for( ; it!=d->views.end(); it++)
	        (*it)->setProperty ("Daddy", "false");
	    
	    view->setProperty ("Daddy", "true");
		  if (d->synchronize) {
			  it = d->views.begin();
			  for( ; it!=d->views.end(); it++)
				  view->link((*it));
		  }
      }
      else
	view->setProperty ("Daddy", "false");
    }
}

void medViewPool::onViewSync (bool sync)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
      
        if (sync) {
	  dtkAbstractView *refView = this->daddy();
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
	  }
	}
	else {

	  if ( d->viewData[view] ) {
	    view->setData (d->viewData[view]);
	    d->viewData[view] = NULL;
	    view->update();
	  }
	}
	
    }  
}

void medViewPool::onViewPropertySet (QString key, QString value)
{
    if (key=="Daddy")
        return;

	if (d->synchronize) {
    foreach (dtkAbstractView *lview, d->views) {
      if (lview!=this->sender()) {
        lview->blockSignals (true);
        lview->setProperty (key, value);
	    lview->blockSignals (false);
      }
    }
	}
}
