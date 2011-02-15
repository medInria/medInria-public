#ifndef V3DVIEW4DINTERACTOR_H
#define V3DVIEW4DINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dView4DInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class vtkCollection;
class vtkMetaDataSetSequence;


class V3DVIEWPLUGIN_EXPORT v3dView4DInteractor: public dtkAbstractViewInteractor
{

    Q_OBJECT
    
public:
    v3dView4DInteractor();
    virtual ~v3dView4DInteractor();
    
    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    
    static bool registered(void);
    
    virtual void appendData(dtkAbstractData *data);
    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    virtual void enable(void);
    virtual void disable(void);

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );

    virtual void setCurrentTime (double time);
    virtual double getCurrentTime (void)
    { return this->currentTime; }
    
    
protected:
    virtual void updatePipeline ();

signals:
    void selectionValidated (const QString& name);

public slots:
    virtual void onPropertySet (const QString& key, const QString& value);
    virtual void onVisibilityPropertySet (const QString& value);
    virtual void onEdgeVisibilityPropertySet (const QString& value);
    virtual void onRenderingModePropertySet (const QString& value);

private:
    v3dView4DInteractorPrivate *d;

    vtkCollection* sequenceList;

    double currentTime;
};

dtkAbstractViewInteractor *createV3dView4DInteractor(void);

#endif // V3DVIEWMESHINTERACTOR_H
