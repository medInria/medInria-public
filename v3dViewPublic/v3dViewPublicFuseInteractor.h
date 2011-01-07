#ifndef V3DVIEWPUBLICFUSEINTERACTOR_H
#define V3DVIEWPUBLICFUSEINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "v3dViewPublicPluginExport.h"

class v3dViewPublicFuseInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class V3DVIEWPUBLICPLUGIN_EXPORT v3dViewPublicFuseInteractor: public dtkAbstractViewInteractor
{
	Q_OBJECT
	
public:
	         v3dViewPublicFuseInteractor();
	virtual ~v3dViewPublicFuseInteractor();
	
	virtual QString description(void) const;
	virtual QStringList handled(void) const;
	
	static bool registered(void);
	
	virtual void setData(dtkAbstractData *data, int channel);
	virtual void setView(dtkAbstractView *view);

	virtual void enable(void);
	virtual void disable(void);

public slots:
	virtual void onPropertySet                       (const QString& key, const QString& value);
	virtual void onFusionStylePropertySet            (const QString& value);
	virtual void onBlendAlphaValueSet                (int value);
	virtual void onBlendAlphaValueSet                (double value);
	virtual void onCheckerboardDivisionCountValueSet (int value);
	virtual void onCheckerboardDivisionCountValueSet (double value);

private:
    v3dViewPublicFuseInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewPublicFuseInteractor(void);

#endif
