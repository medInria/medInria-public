/* medToolBoxSegmentation.h ---
 * 
 * Author: John Stark
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

#ifndef MEDTOOLBOXSEGMENTATION_H
#define MEDTOOLBOXSEGMENTATION_H

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractProcess;
class medAbstractDataImage;
class medAbstractViewScene;
class medDataIndex;
class medProgressionStack;
class medToolBoxSegmentationPrivate;
class medViewerConfiguration;

class MEDGUI_EXPORT medToolBoxSegmentation : public medToolBox
{
    Q_OBJECT;

public:
     medToolBoxSegmentation(medViewerConfiguration * config, QWidget *parent = 0);
    ~medToolBoxSegmentation(void);
	
    dtkAbstractProcess * process();
    void setProcess(dtkAbstractProcess* process);

    medProgressionStack * progressionStack();

    void run( dtkAbstractProcess* alg );
    bool canUndo() const;
    void undo();

    static medAbstractViewScene * viewScene( dtkAbstractView * view );
    static dtkAbstractData * viewData( dtkAbstractView * view );

    QString localizedNameForAlgorithm( const QString & algName ) const;

    void addViewEventFilter(QObject * filter);
    void removeViewEventFilter(QObject * filter);

signals:
    void toolBoxChosen(const QByteArray & algName);

    void showError (QObject*,const QString&,unsigned int timeout);
    void showInfo(QObject*,const QString&,unsigned int timeout);

public slots:
    // Override
    void update (dtkAbstractView *view);
   
    void onAlgorithmAdded( const QString & algName);

    void onToolBoxChosen(int);
    void onToolBoxChosen(const QByteArray & id);
    void clear(void);

    void onSuccess(QObject * sender);
    void onFailure( QObject * sender );
    void onCancelled( QObject * sender );

protected:
    void initializeAlgorithms();
    void initializeAnnotations();
    void activateViews( bool isActivate = true );

public:
    void setAlgorithmParameterWidget( QWidget * widget );

private:
    medToolBoxSegmentationPrivate *d;
};

#endif
