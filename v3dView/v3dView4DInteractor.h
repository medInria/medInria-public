/* v3dView4DInteractor.h ---
 *
 * Author: Nicolas Toussaint

 * Change log:
 *
 */
#ifndef V3DVIEW4DINTERACTOR_H
#define V3DVIEW4DINTERACTOR_H

#include <med4DAbstractViewInteractor.h>
#include "v3dViewPluginExport.h"

class v3dView4DInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

/**
    \class v3dView4DInteractor
    \brief This important interactor handles the 4th dimension
    of data interacting with the viewing process.

    \see med4DAbstractViewInteractor medViewerToolBoxTime
    \author Nicolas Toussaint and Fatih Arslan
*/

class V3DVIEWPLUGIN_EXPORT v3dView4DInteractor: public med4DAbstractViewInteractor
{

    Q_OBJECT

public:
    v3dView4DInteractor();
    virtual ~v3dView4DInteractor();

    virtual QString description(void) const;
    virtual QString identifier(void) const;
    virtual QStringList handled(void) const;

    static bool registered(void);

    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    virtual void enable(void);
    virtual void disable(void);

    /** Override dtkAbstractViewInteractor method */
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );

    void setCurrentTime (double time);
    double getCurrentTime (void)
    { return this->currentTime; }
    /** Assigns ranfe array the minimum and maximum time of the sequence */
    void sequencesRange (double* range);
    /** Returns minimum time step for the sequence */
    double sequencesMinTimeStep (void);

public slots:
    void onDataAdded (dtkAbstractData* data);


protected:

signals:

public slots:
	//Add image to the sequence
    virtual void appendData(dtkAbstractData *data);

private:
    v3dView4DInteractorPrivate *d;

    double currentTime;
};

dtkAbstractViewInteractor *createV3dView4DInteractor(void);

#endif // V3DVIEWMESHINTERACTOR_H
