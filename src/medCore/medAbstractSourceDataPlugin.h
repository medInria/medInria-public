#ifndef MEDABSTRACTSOURCEDATAPLUGIN_H
#define MEDABSTRACTSOURCEDATAPLUGIN_H

#include <dtkCore/dtkPlugin.h>
#include "medCoreExport.h"

class medAbstractSourceDataPluginPrivate;

class MEDCORE_EXPORT medAbstractSourceDataPlugin : public dtkPlugin
{
	Q_OBJECT
	
public:
	medAbstractSourceDataPlugin(QObject *parent = 0);
	~medAbstractSourceDataPlugin(void);

	QWidget *widget() = 0;
	
public slots:
	void onImportData(void) = 0;
	
private:
	medAbstractSourceDataPluginPrivate *d;
};

#endif
