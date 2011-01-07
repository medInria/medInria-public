#ifndef V3DVIEWPUBLICMESHINTERACTOR_H
#define V3DVIEWPUBLICMESHINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "v3dViewPublicPluginExport.h"

class v3dViewPublicMeshInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class V3DVIEWPUBLICPLUGIN_EXPORT v3dViewPublicMeshInteractor: public dtkAbstractViewInteractor
{

    Q_OBJECT
    
public:
    v3dViewPublicMeshInteractor();
    virtual ~v3dViewPublicMeshInteractor();
    
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
    virtual void updatePipeline ();

signals:
    void selectionValidated (const QString& name);

public slots:
    virtual void onPropertySet (const QString& key, const QString& value);
    virtual void onVisibilityPropertySet (const QString& value);
    virtual void onEdgeVisibilityPropertySet (const QString& value);
    virtual void onRenderingModePropertySet (const QString& value);

private:
    v3dViewPublicMeshInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewPublicMeshInteractor(void);

#endif // V3DVIEWMESHINTERACTOR_H
