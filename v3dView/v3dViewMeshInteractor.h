#ifndef V3DVIEWMESHINTERACTOR_H
#define V3DVIEWMESHINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dViewMeshInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class V3DVIEWPLUGIN_EXPORT v3dViewMeshInteractor: public dtkAbstractViewInteractor
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

signals:
    void selectionValidated (QString name);

public slots:
    virtual void onPropertySet (QString key, QString value);
    virtual void onVisibilityPropertySet (QString value);
    virtual void onBoxVisibilityPropertySet (QString value);
    virtual void onRenderingModePropertySet (QString value);
    virtual void onGPUModePropertySet (QString value);
    virtual void onColorModePropertySet (QString value);
    virtual void onBoxBooleanOperationPropertySet (QString value);
    virtual void onProjectionPropertySet (QString value);
    virtual void onRadiusSet (int value);

    virtual void onSelectionTagged (void);
    virtual void onSelectionReset (void);
    virtual void onSelectionValidated (void);    
     
private:
    v3dViewMeshInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewMeshInteractor(void);

#endif // V3DVIEWMESHINTERACTOR_H
