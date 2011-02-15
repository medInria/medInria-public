#ifndef MEDABSTRACTSOURCEDATAPLUGIN_H
#define MEDABSTRACTSOURCEDATAPLUGIN_H

#include <dtkCore/dtkAbstractObject.h>
#include "medCoreExport.h"

class medAbstractSourceDataPluginPrivate;

class MEDCORE_EXPORT medAbstractSourceDataPlugin : public dtkAbstractObject
{
	Q_OBJECT
	
public:
	medAbstractSourceDataPlugin(void);
	~medAbstractSourceDataPlugin(void);

	virtual QWidget *widget() = 0;
	virtual QWidget *sourceSelectorWidget() = 0;
	virtual QString tabName() = 0;

signals:	
	void importedSuccess();
	
public slots:
	virtual void onImportData(void) = 0;
	
private:
	medAbstractSourceDataPluginPrivate *d;
};

#endif
