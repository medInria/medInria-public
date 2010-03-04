/* medViewerArea.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:42:58 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Mar  4 11:02:14 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 45
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

class medViewContainer;

// /////////////////////////////////////////////////////////////////
// medViewerAreaStack
// /////////////////////////////////////////////////////////////////

class medViewerAreaStackPrivate;

class medViewerAreaStack : public QStackedWidget
{
    Q_OBJECT

public:
     medViewerAreaStack(QWidget *parent = 0);
    ~medViewerAreaStack(void);

    medViewContainer *current(void);

    void setPatientId(int id);

signals:
    void focused(dtkAbstractView *view);

private:
    medViewerAreaStackPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medViewerArea
// /////////////////////////////////////////////////////////////////

class medViewerAreaPrivate;

class medViewerArea : public QWidget
{
    Q_OBJECT

public:
     medViewerArea(QWidget *parent = 0);
    ~medViewerArea(void);

    void setup(QStatusBar *status);
    void setdw(QStatusBar *status);

    void setPatientIndex(int id);
    void   setStudyIndex(int id);
    void  setSeriesIndex(int id);
    void   setImageIndex(int id);

public slots:
    void setup(void);
    void split(int rows, int cols);

    void onPatientIndexChanged(int index);
    void   onStudyIndexChanged(int index);
    void  onSeriesIndexChanged(int index);
    void   onImageIndexChanged(int index);

    void onViewFocused(dtkAbstractView *view);

protected slots: // layout settings
    void setStackIndex(int index);

protected slots: // view settings
    void setupForegroundLookupTable(QString table);
    void setupBackgroundLookupTable(QString table);
    void setupAxisVisibility(bool visible);
    void setupScalarBarVisibility(bool visible);
    void setup3DMode (QString table);
    void setupLUTPreset (QString table);
    void setup3DLOD (int value);
    void setupWindowing (bool checked);
    void setupZooming (bool checked);
    void setupSlicing (bool checked);
    void setupMeasuring (bool checked);
    void setupCropping (bool checked);

protected slots: // registration settings
    void setupLayoutCompare(void);
    void setupLayoutFuse(void);

private:
    medViewerAreaPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medViewerConfiguration
// /////////////////////////////////////////////////////////////////

class medToolBox;
class medViewerConfigurationPrivate;

class medViewerConfiguration : public QObject
{
    Q_OBJECT

public:
     medViewerConfiguration(void);
    ~medViewerConfiguration(void);

    void setup(void);
    void setdw(void);

    void attach(medToolBox *toolbox, bool visible);

private:
    medViewerConfigurationPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medViewerConfigurator
// /////////////////////////////////////////////////////////////////

class medViewerConfiguratorPrivate;

class medViewerConfigurator : public QObject
{
    Q_OBJECT

public:
    static medViewerConfigurator *instance(void);

    void addConfiguration(QString name, medViewerConfiguration *configuration);

public slots:
    void setConfiguration(QString name);

protected:
     medViewerConfigurator(void);
    ~medViewerConfigurator(void);

protected:
    static medViewerConfigurator *s_instance;

private:
    medViewerConfiguratorPrivate *d;
};

#endif // MEDVIEWERAREA_H
