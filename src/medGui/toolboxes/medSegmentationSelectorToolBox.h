/* medSegmentationSelectorToolBox.h ---
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

#ifndef medSegmentationSelectorToolBox_H
#define medSegmentationSelectorToolBox_H

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractProcess;
class medAbstractDataImage;
class medAbstractViewCoordinates;
class medDataIndex;
class medProgressionStack;
class medSegmentationSelectorToolBoxPrivate;
class medWorkspace;
class medViewEventFilter;

//! Segmentation toolbox
class MEDGUI_EXPORT medSegmentationSelectorToolBox : public medToolBox
{
    Q_OBJECT;

public:
    struct MaskPixelValues { enum E { Unset = 0, Foreground = 1, Background = 2 };  };

     medSegmentationSelectorToolBox(medWorkspace * workspace, QWidget *parent = 0);
    ~medSegmentationSelectorToolBox(void);

    //! Get/Set the currently running process.
    dtkAbstractProcess * process();
    void setProcess(dtkAbstractProcess* process);

    //! Get the progress stack.
    medProgressionStack * progressionStack();

    //! Request that the given process is run.
    void run( dtkAbstractProcess* alg );

    //! Obtain the view coordinates for the given view
    static medAbstractViewCoordinates * viewCoordinates( dtkAbstractView * view );

    //! Get the data for the given view.
    static dtkAbstractData * viewData( dtkAbstractView * view );

    //! Get the description for the algorithm with the given name.
    QString localizedNameForAlgorithm( const QString & algName ) const;

    //! Add / remove an event filter to a view.
    void addViewEventFilter(medViewEventFilter * filter);
    void removeViewEventFilter(medViewEventFilter * filter);

signals:
    //! Emitted when a toolbox is chosen from the combo box.
    void toolBoxChosen(const QByteArray & algName);

    //! Emitted when an error should be shown.
    void showError (const QString&,unsigned int timeout);

    //! Emitted when info should be shown.
    void showInfo(const QString&,unsigned int timeout);

public slots:
    // Override base class
    void update (dtkAbstractView *view);

    //! called when an algorithm is added (to a factory for instance)
    void onAlgorithmAdded( const QString & algName);

    //! Callback from combo box.
    void onToolBoxChosen(int);

    //! The given segmentation algorithm is selected.
    void onToolBoxChosen(const QByteArray & id);

    //! Reset selection
    void clear(void);

    //! Callbacks from running a process.
    void onSuccess(QObject * sender);
    void onFailure( QObject * sender );
    void onCancelled( QObject * sender );

protected:
    void initializeAlgorithms();
    void initializeAnnotations();
    void activateViews( bool isActivate = true );

    //! Add the minimum metadata to the output if not present
    void setOutputMetadata(const dtkAbstractData * inputData, dtkAbstractData * outputData);

public:
    void setAlgorithmParameterWidget( QWidget * widget );

private:
    medSegmentationSelectorToolBoxPrivate *d;
};

#endif
