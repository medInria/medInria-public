/* medToolBoxRegistration.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar  4 10:44:47 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 29
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXREGISTRATION_H
#define MEDTOOLBOXREGISTRATION_H

#include "medToolBox.h"

class medToolBoxRegistrationPrivate;
class dtkAbstractView;
class medDataIndex;

class MEDGUI_EXPORT medToolBoxRegistration : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxRegistration(QWidget *parent = 0);
    ~medToolBoxRegistration(void);
	
    dtkAbstractView *fuseView(void);

signals:
    void setupLayoutCompare(void);
    void setupLayoutFuse(void);

public slots:
    void run(void);

    void onMovingImageDropped (void);
    void onFixedImageDropped (void);
    
    void onBlendModeSet(bool value);
    void onCheckerboardModeSet(bool value);
	
private:
    medToolBoxRegistrationPrivate *d;
};

#endif
