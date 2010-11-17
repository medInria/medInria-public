/* medToolBoxRegistration.h --- 
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

#ifndef MEDTOOLBOXREGISTRATION_H
#define MEDTOOLBOXREGISTRATION_H

#include "medGuiExport.h"
#include "medToolBox.h"

class dtkAbstractDataImage;
class dtkAbstractView;

class medDataIndex;

class medToolBoxRegistrationPrivate;

class MEDGUI_EXPORT medToolBoxRegistration : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxRegistration(QWidget *parent = 0);
    ~medToolBoxRegistration(void);
	
    dtkAbstractView *fixedView(void);
    dtkAbstractView *movingView(void);
    dtkAbstractView *fuseView(void);

    dtkAbstractDataImage *fixedData(void);
    dtkAbstractDataImage *movingData(void);

signals:
    void addToolBox(medToolBox *toolbox);
    void removeToolBox(medToolBox *toolbox);
    void setupLayoutCompare(void);
    void setupLayoutFuse(void);

public slots:
    void onMovingImageDropped(void);
    void onFixedImageDropped(void);
    
    void onBlendModeSet(bool value);
    void onCheckerboardModeSet(bool value);
	
    void onToolBoxChosen(const QString&);

private:
    medToolBoxRegistrationPrivate *d;
};

#endif
