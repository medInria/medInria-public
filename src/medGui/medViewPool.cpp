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

class medViewPoolPrivate
{
public:
  QList<dtkAbstractView *> views;
  int                      synchronize;
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
  
    dtkAbstractView *refView = NULL;
    if (d->views.count()==1) {
        refView = view;
	refView->setProperty ("Daddy", "true");
    }
    else {  
        QList<dtkAbstractView *>::iterator it = d->views.begin();
	for( ; it!=d->views.end(); it++) {
	    if ((*it)->property ("Daddy")=="true") {
	        refView = (*it);
		break;
	    }
	}
    }
    
    if (d->synchronize && refView)
        refView->link (view);
}

void medViewPool::removeView (dtkAbstractView *view)
{
    if (!view || !d->views.contains (view))
        return;

    // look if a view is a daddy
    dtkAbstractView *refView = NULL;
    QList<dtkAbstractView *>::iterator it = d->views.begin();
    for( ; it!=d->views.end(); it++) {
        if ((*it)->property ("Daddy")=="true") {
	  refView = (*it);
	  break;
	}
    }

    if (refView)
        refView->unlink (view);

    disconnect (view, SIGNAL (propertySet(QString, QString)), this, SLOT (onViewPropertySet(QString, QString)));
    d->views.removeOne (view);
}

void medViewPool::synchronize (void)
{
    d->synchronize = 1;
    if (d->views.count()==0)
        return;
    
    // look if a view is a daddy
    dtkAbstractView *refView = NULL;
    QList<dtkAbstractView *>::iterator it = d->views.begin();
    for( ; it!=d->views.end(); it++) {
        if ((*it)->property ("Daddy")=="true") {
	  refView = (*it);
	  break;
	}
    }

    it = d->views.begin();
    if (refView) {
        for (it; it!=d->views.end(); ++it) {
	    refView->link ( (*it) );
	}
    }
}

void medViewPool::desynchronize (void)
{
    dtkAbstractView *refView = NULL;
    QList<dtkAbstractView *>::iterator it = d->views.begin();
    for( ; it!=d->views.end(); it++) {
        if ((*it)->property ("Daddy")=="true") {
	  refView = (*it);
	  break;
	}
    }

    if (refView) {
        QList<dtkAbstractView *>::iterator it = d->views.begin();
	for (it; it!=d->views.end(); ++it) {
	    refView->unlink ( (*it) );
	}
    }

    d->synchronize = 0;
}


void medViewPool::onViewPropertySet (QString key, QString value)
{
    if (key=="Daddy")
        return;

    foreach (dtkAbstractView *lview, d->views) {
      if (lview!=this->sender()) {
        lview->blockSignals (true);
        lview->setProperty (key, value);
	lview->blockSignals (false);
      }
    }
}
