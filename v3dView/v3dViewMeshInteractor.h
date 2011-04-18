#ifndef V3DVIEWMESHINTERACTOR_H
#define V3DVIEWMESHINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>
#include <medCore/medMeshAbstractViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dViewMeshInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;
class vtkImageActor;

class V3DVIEWPLUGIN_EXPORT v3dViewMeshInteractor: public medMeshAbstractViewInteractor
{

    Q_OBJECT
    
public:
    v3dViewMeshInteractor();
    virtual ~v3dViewMeshInteractor();
    
    virtual QString description(void) const;
    virtual QStringList handled(void) const;
    
    static bool registered(void);
    
    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    virtual void enable(void);
    virtual void disable(void);

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );
    
    

protected:
    virtual void updatePipeline (unsigned int meshLayer=0);

signals:
    void selectionValidated (const QString& name);

public slots:
    virtual void onPropertySet (const QString& key, const QString& value);
    virtual void onVisibilityPropertySet (const QString& value, int meshLayer);
    virtual void onEdgeVisibilityPropertySet (const QString& value, int meshLayer);
    virtual void onRenderingModePropertySet (const QString& value, int meshLayer);
    virtual void onOpacityModePropertySet (double value, int meshLayer);
    virtual void onLUTModePropertySet (const QString& value, int meshLayer);
    

private:
    v3dViewMeshInteractorPrivate *d;


};

dtkAbstractViewInteractor *createV3dViewMeshInteractor(void);

#endif // V3DVIEWMESHINTERACTOR_H
