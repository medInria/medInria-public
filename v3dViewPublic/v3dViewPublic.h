// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////

#ifndef V3DVIEWPUBLIC_H
#define V3DVIEWPUBLIC_H

#include <medCore/medAbstractView.h>

#include "v3dViewPublicPluginExport.h"

class v3dViewPublicPrivate;
class vtkViewImage2D;
class vtkViewImage3D;
class vtkRenderer;
class vtkRenderWindowInteractor;

class V3DVIEWPUBLICPLUGIN_EXPORT v3dViewPublic : public medAbstractView
{
    Q_OBJECT

public:
             v3dViewPublic(void);
    virtual ~v3dViewPublic(void);

    virtual QString description(void) const;

    static bool registered(void);

public:
    void reset(void);
    void update(void);
	
    void   link(dtkAbstractView *other);
    void unlink(dtkAbstractView *other);
	
    void *view(void);
	
    void setData(dtkAbstractData *data);
    void *data (void);
	
    QList<dtkAbstractView *> linkedViews (void);
	
    // QWidget *receiverWidget(void);
    QWidget *widget(void);

    void close(void);
	
    // access method to internal members for v3dView**Interactor classes
    vtkViewImage2D *view2D(void);
    vtkViewImage3D *view3D(void);
    vtkRenderer    *renderer2D (void);
    vtkRenderer    *renderer3D (void);
    vtkRenderWindowInteractor *interactor2D (void);
    vtkRenderWindowInteractor *interactor3D (void);
	
public slots:
    void onPropertySet         (const QString &key, const QString &value);
    void onOrientationPropertySet           (const QString &value);
    void on3DModePropertySet                (const QString &value);
    void onRendererPropertySet              (const QString &value);
    void onPresetPropertySet                (const QString &value);
    void onShowScalarBarPropertySet         (const QString &value);
    void onLookupTablePropertySet           (const QString &value);
    void onShowAxisPropertySet              (const QString &value);
    void onShowRulerPropertySet             (const QString &value);
    void onShowAnnotationsPropertySet       (const QString &value);
    void onMouseInteractionPropertySet      (const QString &value);
    void onCroppingPropertySet              (const QString &value);
    void onDaddyPropertySet                 (const QString &value);
    void onWindowingLinkedPropertySet       (const QString &value);
    void onPositionLinkedPropertySet        (const QString &value);

    
public slots:
    void linkPosition  (dtkAbstractView *view, bool value);
    void linkCamera    (dtkAbstractView *view, bool value);
    void linkWindowing (dtkAbstractView *view, bool value);

    void onZSliderValueChanged (int);
    void onMousePressEvent(QMouseEvent *event);
    void switchToAxial(void);
    void switchToSagittal(void);
    void switchToCoronal(void);
    void switchToVR(void);
    void switchToVRGPU(void);
    void switchToMPR(void);
    void onMenuZoomTriggered (void);
    void onMenuWindowLevelTriggered (void);
    void onPlayButtonClicked (bool value);
    void onDimensionBoxChanged (const QString &value);
    

private:
    v3dViewPublicPrivate *d;
};

dtkAbstractView *createV3dViewPublic(void);

#endif
