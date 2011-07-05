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

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractDataImage;
class dtkAbstractView;
class dtkAbstractProcess;
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

    dtkAbstractDataImage *fixedData(void);
    dtkAbstractDataImage *movingData(void);
    
    void setFuseView(dtkAbstractView *view);
    dtkAbstractView *fuseView(void);

    dtkAbstractProcess * process();
    void setProcess(dtkAbstractProcess* process);

signals:
    void setupLayoutCompare(void);
    void setupLayoutFuse(void);
    void showError (QObject*,const QString&,unsigned int timeout);
    void showInfo(QObject*,const QString&,unsigned int timeout);

public slots:
    void onMovingImageDropped(const medDataIndex& index);
    void onFixedImageDropped(const medDataIndex& index);
    
//    void onBlendModeSet(bool value);
//    void onCheckerboardModeSet(bool value);

    void onToolBoxChosen(const QString&);
    void clear(void);
    void onSaveImage(void);
    void onSaveTrans(void);
    void onSuccess(void);
private:
    medToolBoxRegistrationPrivate *d;
};

#endif
