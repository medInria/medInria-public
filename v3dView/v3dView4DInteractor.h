#ifndef V3DVIEW4DINTERACTOR_H
#define V3DVIEW4DINTERACTOR_H

#include <medCore/med4DAbstractViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dView4DInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class vtkCollection;
class vtkMetaDataSetSequence;


class V3DVIEWPLUGIN_EXPORT v3dView4DInteractor: public med4DAbstractViewInteractor
{

    Q_OBJECT
    
public:
    v3dView4DInteractor();
    virtual ~v3dView4DInteractor();
    
    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    
    static bool registered(void);
    
    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    virtual void enable(void);
    virtual void disable(void);

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );

    void setCurrentTime (double time);
    double getCurrentTime (void)
    { return this->currentTime; }
    
    void sequencesRange (double* range);
    double sequencesMinTimeStep (void);

public slots:
    void onDataAdded (dtkAbstractData* data);
    
    
protected:
    virtual void updatePipeline ();

signals:

public slots:
    virtual void appendData(dtkAbstractData *data);
    
private:
    v3dView4DInteractorPrivate *d;

    double currentTime;
};

dtkAbstractViewInteractor *createV3dView4DInteractor(void);

#endif // V3DVIEWMESHINTERACTOR_H
