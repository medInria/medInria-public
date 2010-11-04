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
#include <medCore/medMessageController.h>

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include "medCore/medMessageController.h"

class medViewPoolPrivate
{
public:
  QList<dtkAbstractView *>                 views;
  QMap<dtkAbstractView*, dtkAbstractData*> viewData;
  QHash<QString, QString>                  propertySet;
};


medViewPool::medViewPool(void) : d (new medViewPoolPrivate)
{
    connect(this,SIGNAL(showInfo(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(QObject*,const QString&,unsigned int)));
    connect(this,SIGNAL(showError(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(QObject*,const QString&,unsigned int)));
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

    if (d->views.count()==0) { // view will become daddy      
      view->setProperty ("Daddy", "true");
      connect (this, SIGNAL (linkPosition  (dtkAbstractView *, bool)), view, SLOT (linkPosition  (dtkAbstractView *, bool)));
      connect (this, SIGNAL (linkCamera    (dtkAbstractView *, bool)), view, SLOT (linkCamera    (dtkAbstractView *, bool)));
      connect (this, SIGNAL (linkWindowing (dtkAbstractView *, bool)), view, SLOT (linkWindowing (dtkAbstractView *, bool)));
    }
    
    d->views.append (view);

    connect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    connect (view, SIGNAL (syncPosition(bool)),            this, SLOT (onViewSyncPosition(bool)));
    connect (view, SIGNAL (syncCamera(bool)),              this, SLOT (onViewSyncCamera(bool)));
    connect (view, SIGNAL (syncWindowing(bool)),           this, SLOT (onViewSyncWindowing(bool)));
    connect (view, SIGNAL (reg(bool)),                     this, SLOT (onViewReg(bool)));
  
    // set properties
    QHashIterator<QString, QString> it(d->propertySet);
    while (it.hasNext()) {
        it.next();
	view->setProperty (it.key(), it.value());
    }

    if (dtkAbstractView *refView = this->daddy())
      if (refView != view)
	refView->link (view);
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
		emit linkPosition  ((*it), false);
		emit linkCamera    ((*it), false);
		emit linkWindowing ((*it), false);
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
	    disconnect (this, SIGNAL (linkPosition  (dtkAbstractView *, bool)), oldDaddy, SLOT (linkPosition  (dtkAbstractView *, bool)));
	    disconnect (this, SIGNAL (linkCamera    (dtkAbstractView *, bool)), oldDaddy, SLOT (linkCamera    (dtkAbstractView *, bool)));
	    disconnect (this, SIGNAL (linkWindowing (dtkAbstractView *, bool)), oldDaddy, SLOT (linkWindowing (dtkAbstractView *, bool)));
	    refView = this->daddy();
	    if (refView) {
	        connect (this, SIGNAL (linkPosition  (dtkAbstractView *, bool)), refView, SLOT (linkPosition  (dtkAbstractView *, bool)));
	        connect (this, SIGNAL (linkCamera    (dtkAbstractView *, bool)), refView, SLOT (linkCamera    (dtkAbstractView *, bool)));
	        connect (this, SIGNAL (linkWindowing (dtkAbstractView *, bool)), refView, SLOT (linkWindowing (dtkAbstractView *, bool)));
	    }
	}
	else { // we are not daddy, just unlink
	    refView->unlink (view);
	    emit linkPosition  (view, false);
	    emit linkCamera    (view, false);
	    emit linkWindowing (view, false);	    
	}
    }
    
    disconnect (view, SIGNAL (becomeDaddy(bool)),             this, SLOT (onViewDaddy(bool)));
    disconnect (view, SIGNAL (syncPosition(bool)),            this, SLOT (onViewSyncPosition(bool)));
    disconnect (view, SIGNAL (syncCamera(bool)),              this, SLOT (onViewSyncCamera(bool)));
    disconnect (view, SIGNAL (syncWindowing(bool)),           this, SLOT (onViewSyncWindowing(bool)));
    disconnect (view, SIGNAL (reg(bool)),                     this, SLOT (onViewReg(bool)));

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
		    emit linkPosition  ((*it), false);
		    emit linkCamera    ((*it), false);
		    emit linkWindowing ((*it), false);
		}
	    }

	    if (refView) {
	        disconnect (this, SIGNAL (linkPosition  (dtkAbstractView *, bool)), refView, SLOT (linkPosition  (dtkAbstractView *, bool)));
		disconnect (this, SIGNAL (linkCamera    (dtkAbstractView *, bool)), refView, SLOT (linkCamera    (dtkAbstractView *, bool)));
	        disconnect (this, SIGNAL (linkWindowing (dtkAbstractView *, bool)), refView, SLOT (linkWindowing (dtkAbstractView *, bool)));
	    }

	    // tell all views that they are not daddys
	    QList<dtkAbstractView *>::iterator it = d->views.begin();
	    for( ; it!=d->views.end(); it++)
	        (*it)->setProperty ("Daddy", "false");

	    // tell the sender it is now daddy
	    view->setProperty ("Daddy", "true");

		// restore the previous data (if any)
	    if ( d->viewData[view] ) {
	        view->setData (d->viewData[view]);
		d->viewData[view] = NULL;
		view->update();
	    }
	    connect (this, SIGNAL (linkPosition  (dtkAbstractView *, bool)), view, SLOT (linkPosition  (dtkAbstractView *, bool)));
	    connect (this, SIGNAL (linkCamera    (dtkAbstractView *, bool)), view, SLOT (linkCamera    (dtkAbstractView *, bool)));
	    connect (this, SIGNAL (linkWindowing (dtkAbstractView *, bool)), view, SLOT (linkWindowing (dtkAbstractView *, bool)));

	}
	else { // view does not want to become daddy
	    if (view==this->daddy()) {
	        QList<dtkAbstractView *>::iterator it = d->views.begin();
		    for( ; it!=d->views.end(); it++) {
		        view->unlink((*it));
			emit linkPosition  ((*it), false);
			emit linkCamera    ((*it), false);
			emit linkWindowing ((*it), false);			
		    }
	    }
	  
	    view->setProperty ("Daddy", "false");
	}
    }
}

void medViewPool::onViewSyncPosition (bool value)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
      
        if (view->property ("Daddy")=="false") {
	    emit linkPosition (view, value);
	    view->update();
	}
	
    }
}

void medViewPool::onViewSyncCamera (bool value)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
      
        if (view->property ("Daddy")=="false") {
	    emit linkCamera (view, value);
	    view->update();
	}
	
    }
}

void medViewPool::onViewSyncWindowing (bool value)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
      
        if (view->property ("Daddy")=="false") {
	    emit linkWindowing (view, value);
	    view->update();
	}
	
    }
}

void medViewPool::onViewReg(bool value)
{
	if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
      
        if (value) {
	    dtkAbstractView *refView = this->daddy();

	    if (refView==view) // do not register the view with itself
	        return;
	  
	    if (refView) {

	        dtkAbstractData *data1 = static_cast<dtkAbstractData *>(refView->data());
		dtkAbstractData *data2 = static_cast<dtkAbstractData *>(view->data());

		if (data1!=data2) {// do not register the same data

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
                        emit showInfo (this, tr ("Automatic registration successful"),3000);
		    }
                    else {
                        emit showError(this, tr  ("Automatic registration failed"),3000);
                    }
		}

	    }
	}
	else {

	    // restore the previous data (if any)
	    if ( d->viewData[view] ) {
	        view->setData (d->viewData[view]);
		d->viewData[view] = NULL;
		view->update();
	    }
	}
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
