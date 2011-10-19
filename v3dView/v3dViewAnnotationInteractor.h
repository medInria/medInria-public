#ifndef V3DVIEWANNOTATIONINTERACTOR_H
#define V3DVIEWANNOTATIONINTERACTOR_H

#include "medAbstractAnnotationViewInteractor.h"

#include "v3dViewPluginExport.h"

#include <vtkAbstractWidget.h>

class v3dViewAnnotationInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;
class v3dViewAnnIntSeedPointHelper;

class v3dView;

class V3DVIEWPLUGIN_EXPORT v3dViewAnnotationInteractor: public medAbstractAnnotationViewInteractor
{
    typedef medAbstractAnnotationViewInteractor BaseClass;

    Q_OBJECT

public:
    v3dViewAnnotationInteractor();
    virtual ~v3dViewAnnotationInteractor();

    static QString s_identifier();

    virtual QString description(void) const;
    virtual QString identifier(void) const;
    virtual QStringList handled(void) const;

    static bool registered(void);

    virtual void enable(void);
    virtual void disable(void);

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );

protected:

    void onPropertySet(const QString& key, const QString& value);

    void onVisibilityPropertySet (const QString& value);

    v3dView * getV3dView();

    void initialize(medAbstractView * view, medAbstractData* data);

public slots:
    virtual void onDataModified(medAnnotationData* data);

signals:

public slots:

    virtual bool onAddAnnotation( medAnnotationData * annItem );
    virtual void onRemoveAnnotation( medAnnotationData * annItem );

protected:
    typedef vtkSmartPointer <vtkAbstractWidget>  WidgetSmartPointer;
    enum EAnnotation { SeedPoint };
    struct ActorInfo {
        WidgetSmartPointer actor2d;
        WidgetSmartPointer actor3d;
        EAnnotation AnnotationId;
    };

    typedef std::map< medAnnotationData*, ActorInfo> ActorMap;

    friend class v3dViewAnnotationInteractorPrivate;
    friend class v3dViewAnnIntSeedPointHelper;

    ActorMap & getActorMap();

    bool findActorMapForWidget(vtkAbstractWidget * w, ActorMap::iterator & it);
private:
    v3dViewAnnotationInteractorPrivate *d;


};
#endif // V3DVIEWANNOTATIONINTERACTOR_H
