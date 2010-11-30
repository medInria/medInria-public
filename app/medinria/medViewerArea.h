/* medViewerArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:42:58 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:14:33 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 114
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERAREA_H
#define MEDVIEWERAREA_H

#include <QtGui>

class dtkAbstractView;

class medDataIndex;
class medToolBox;
class medViewContainer;
class medViewerAreaStack;
class medViewerAreaPrivate;

class medViewerArea : public QWidget
{
    Q_OBJECT

public:
     medViewerArea(QWidget *parent = 0);
    ~medViewerArea(void);

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

public slots:
    void setup(void);

    void split(int rows, int cols);

    void open(const medDataIndex& index);
    void open(const QString& file);

    void switchToPatient(int index);
    void switchToContainer(int index);
    void switchToContainerPreset(int index);

    void addToolBox(medToolBox *toolbox);
    void removeToolBox(medToolBox *toolbox);

protected slots:
    void onViewFocused(dtkAbstractView *view);

protected:
    medViewerAreaStack *currentStack(void);
    medViewContainer   *currentContainer(void);
    medViewContainer   *currentContainerFocused(void);

protected slots:

//! @name "View Settings" @{

    void setupForegroundLookupTable(QString table);
    // void setupBackgroundLookupTable(QString table);
    void setupAxisVisibility(bool visible);
    void setupScalarBarVisibility(bool visible);
    void setupRulerVisibility(bool visible);
    void setupAnnotationsVisibility(bool visible);
    void setup3DMode (QString mode);
    void setup3DVRMode (QString mode);
    void setupLUTPreset (QString table);
    void setup3DLOD (int value);
    void setupWindowing (bool checked);
    void setupZooming (bool checked);
    void setupSlicing (bool checked);
    void setupMeasuring (bool checked);
    void setupCropping (bool checked);

    void bringUpTransferFunction(bool checked);
    void updateTransferFunction();

//  @}

//! @name "Registration Settings" @{

    void setupLayoutCompare(void);
    void setupLayoutFuse(void);

//  @}

private:
    friend class medMessageController;

private:
    medViewerAreaPrivate *d;
};

#endif // MEDVIEWERAREA_H
