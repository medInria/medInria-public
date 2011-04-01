#ifndef V3DVIEWTENSORINTERACTOR_H
#define V3DVIEWTENSORINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "v3dViewPublicPluginExport.h"

class v3dViewPublicTensorInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class V3DVIEWPUBLICPLUGIN_EXPORT v3dViewPublicTensorInteractor: public dtkAbstractViewInteractor
{

	Q_OBJECT

public:
	v3dViewPublicTensorInteractor();
	virtual ~v3dViewPublicTensorInteractor();

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

private:
    v3dViewPublicTensorInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewPublicTensorInteractor(void);

#endif
