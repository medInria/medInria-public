/* medViewerToolBoxFiltering.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:36:24 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 47
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXFILTERING_H
#define MEDTOOLBOXFILTERING_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxFilteringPrivate;
class dtkAbstractData;
class medToolBoxFilteringCustom;

class MEDGUI_EXPORT medToolBoxFiltering : public medToolBox
{
	Q_OBJECT
public:
	medToolBoxFiltering(QWidget *parent);
	~medToolBoxFiltering();
	
	dtkAbstractData* data(void);
	medToolBoxFilteringCustom* customToolbox(void);

signals:
	void processFinished();
	void dataSelected(dtkAbstractData *data);
 
public slots:
    
	void onToolBoxChosen(const QString&);
	void clear(void);
	void onObjectDropped(void);

    
protected:
	medToolBoxFilteringPrivate *d;
};

#endif
