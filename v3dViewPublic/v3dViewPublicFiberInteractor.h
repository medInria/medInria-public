#ifndef V3DVIEWFIBERINTERACTOR_H
#define V3DVIEWFIBERINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "v3dViewPublicPluginExport.h"

class v3dViewPublicFiberInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class V3DVIEWPUBLICPLUGIN_EXPORT v3dViewPublicFiberInteractor: public dtkAbstractViewInteractor
{

	Q_OBJECT
	
public:
	v3dViewPublicFiberInteractor();
	virtual ~v3dViewPublicFiberInteractor();
	
	virtual QString description(void) const;
	virtual QStringList handled(void) const;
	
	static bool registered(void);
	
	virtual void setData(dtkAbstractData *data);
	dtkAbstractData *data (void);
		
	virtual void setView(dtkAbstractView *view);
	dtkAbstractView *view (void);

	virtual void enable(void);
	virtual void disable(void);

public slots:
	virtual void onPropertySet (const QString& key, const QString& value);
	
	virtual void onVisibilityPropertySet (const QString& value);
	virtual void onBoxVisibilityPropertySet (const QString& value);
	virtual void onRenderingModePropertySet (const QString& value);
	virtual void onGPUModePropertySet (const QString& value);
	virtual void onColorModePropertySet (const QString& value);
	virtual void onBoxBooleanOperationPropertySet (const QString& value);
	virtual void onProjectionPropertySet (const QString& value);
	virtual void onRadiusSet (int value);

	virtual void onSelectionTagged (void);
	virtual void onSelectionReset (void);
	virtual void onSelectionValidated (QString name);
	
	/*
	  virtual void onBundleClicked (QListWidgetItem* item);
	  virtual void onBundleVisibilityClicked (bool value);
	  virtual void onBundleColorModeClicked (bool value);
	  virtual void onShowAllClicked (bool value);
	  virtual void onDeleteAllClicked (void);
	  virtual void onBundleDeleteClicked (void);
	  virtual void onBundleColorClicked (void);
	  virtual void onBundleNameChanged (void);
	*/	
private:
    v3dViewPublicFiberInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewPublicFiberInteractor(void);

#endif
