#ifndef V3DVIEWMESHINTERACTOR_H
#define V3DVIEWMESHINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>
#include <medMeshAbstractViewInteractor.h>

#include <vtkPointSet.h>

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
    virtual QString identifier(void) const;
    virtual QStringList handled(void) const;

    static bool registered(void);

    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    virtual void enable(void);
    virtual void disable(void);

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );
    char* getLUTQuery (int meshLayer);
    void setOpacity (double value);
    double opacity (int meshLayer);
    bool visibility (int meshLayer);
    void setLayer(int meshLayer);
    int meshLayer(void);
    bool edgeVisibility (int meshLayer);
    QString* color (int meshLayer);
    QString* renderingType (int meshLayer);
    QString* attribute (int meshLayer);
    QString* lut (int meshLayer);
    void setAttribute (const QString& attribute, int meshLayer);
    void setScalarVisibility(bool val);
    bool isMeshOnly();
protected:
    virtual void updatePipeline (unsigned int meshLayer=0);
    void changeBounds ( vtkPointSet* pointSet);

signals:
    void selectionValidated (const QString& name);

public slots:
    virtual void onPropertySet (const QString& key, const QString& value);
    virtual void onVisibilityPropertySet (const QString& value);
    virtual void onEdgeVisibilityPropertySet (const QString& value);
    virtual void onRenderingModePropertySet (const QString& value);

    virtual void onLUTModePropertySet (const QString& value);
    virtual void onColorPropertySet ( const QColor& color);


private:
    v3dViewMeshInteractorPrivate *d;


};

dtkAbstractViewInteractor *createV3dViewMeshInteractor(void);

#endif // V3DVIEWMESHINTERACTOR_H
