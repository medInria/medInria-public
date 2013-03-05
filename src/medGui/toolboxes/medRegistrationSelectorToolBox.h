/* medRegistrationSelectorToolBox.h ---
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

#ifndef medRegistrationSelectorToolBox_H
#define medRegistrationSelectorToolBox_H

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractProcess;
class medAbstractDataImage;
class medDataIndex;
class medRegistrationSelectorToolBoxPrivate;

class MEDGUI_EXPORT medRegistrationSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medRegistrationSelectorToolBox(QWidget *parent = 0);
    ~medRegistrationSelectorToolBox();

    dtkAbstractView *fixedView();
    dtkAbstractView *movingView();

    medAbstractDataImage *fixedData();
    medAbstractDataImage *movingData();

    void setFuseView(dtkAbstractView *view);
    dtkAbstractView *fuseView();

    dtkAbstractProcess * process();
    void setProcess(dtkAbstractProcess* process);

signals:
    void setupLayoutCompare();
    void setupLayoutFuse();
    void showError (QObject*,const QString&,unsigned int timeout);
    void showInfo(QObject*,const QString&,unsigned int timeout);

public slots:
    void onMovingImageDropped(const medDataIndex& index);
    void onFixedImageDropped(const medDataIndex& index);

//    void onBlendModeSet(bool value);
//    void onCheckerboardModeSet(bool value);

    void onToolBoxChosen(int index);
    void clear();
    void onSaveImage();
    void onSaveTrans();
    void onSuccess();

    void synchroniseWindowLevel(QObject * sender = NULL);
    void synchronisePosition(const QVector3D &position);

private:
    medRegistrationSelectorToolBoxPrivate *d;
};

#endif
