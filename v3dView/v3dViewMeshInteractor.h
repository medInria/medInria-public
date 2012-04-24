#ifndef V3DVIEWMESHINTERACTOR_H
#define V3DVIEWMESHINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>
#include <medMeshAbstractViewInteractor.h>

#include <vtkPointSet.h>

#include "v3dViewPluginExport.h"


class v3dViewMeshInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;
class vtkLookupTable;

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

    virtual void setData(dtkAbstractData * data);
    virtual void setView(dtkAbstractView * view);

    virtual void enable(void);
    virtual void disable(void);

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );

    void setOpacity(int meshLayer, double value);
    double opacity(int meshLayer) const;

    void setVisibility(int meshLayer, bool visible);
    bool visibility(int meshLayer) const;

    void setEdgeVisibility(int meshLayer, bool visible);
    bool edgeVisibility(int meshLayer) const;

    void setColor(int meshLayer, QColor color);
    QColor color(int meshLayer) const;

    void setRenderingType(int meshLayer, const QString & type);
    QString renderingType(int meshLayer) const;

    void setAttribute(int meshLayer, const QString & attribute);
    QString attribute(int meshLayer) const;

    QStringList getAllAttributes(int meshLayer) const;

    void setLut(int meshLayer, const QString & lutName);
    QString lut(int meshLayer) const;

    QStringList getAllLUTs() const;

protected:
    void updatePipeline (unsigned int meshLayer = 0);
    void changeBounds ( vtkPointSet* pointSet);
    void setLut(int meshLayer, vtkLookupTable * lut);

private:
    v3dViewMeshInteractorPrivate * d;
};

dtkAbstractViewInteractor * createV3dViewMeshInteractor(void);

#endif // V3DVIEWMESHINTERACTOR_H
