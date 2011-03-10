#ifndef V3DVIEWTENSORINTERACTOR_H
#define V3DVIEWTENSORINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dViewTensorInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

/**
 * @class v3dViewTensorInteractor
 * @brief Extents a view by providing tensor viewing/managing capabilities.
 *
 * An interactor is an extension to a view (v3dView in this case)
 * that provides additional functionality. This class extents the view by
 * adding tensor handling capabilities like visualization and tensor-specific
 * properties.
 */
class V3DVIEWPLUGIN_EXPORT v3dViewTensorInteractor: public dtkAbstractViewInteractor
{

	Q_OBJECT

public:
	v3dViewTensorInteractor();
	virtual ~v3dViewTensorInteractor();

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

	void onGlyphShapePropertySet (const QString& value);

private:
    v3dViewTensorInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewTensorInteractor(void);

#endif
