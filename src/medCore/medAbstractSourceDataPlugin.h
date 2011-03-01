#ifndef MEDABSTRACTSOURCEDATAPLUGIN_H
#define MEDABSTRACTSOURCEDATAPLUGIN_H

#include <dtkCore/dtkAbstractObject.h>
#include "medCoreExport.h"

class medAbstractSourceDataPluginPrivate;

/** 
 * @class medAbstractSourceDataPlugin
 * @brief Abstract base class for dynamic source plugins (e.g. Shanoir, PACS, ...)
 * This class adds specific methods to populate the browser area and import data.
 * A dynamic source data plugin derives from this class instead of dtkPlugin.
 **/

class MEDCORE_EXPORT medAbstractSourceDataPlugin : public dtkAbstractObject
{
	Q_OBJECT
	
public:
	medAbstractSourceDataPlugin(void);
	~medAbstractSourceDataPlugin(void);

	/** Returns the main widget (explorer area of the source data plugin, on the left side of the browser area) */
	virtual QWidget *widget() = 0;
	
	/** Returns the source selector widget (what's inside the source selector tab on the right side of the browser area) */
	virtual QWidget *sourceSelectorWidget() = 0;
	
	/** Returns the tab name for the source selector panel */
	virtual QString tabName() = 0;

signals:
  /** A source data plugin emits a signal when it succesfully imported a data */
	void importedSuccess();
	
public slots:
	/** Actually does the import of the selected data */
	virtual void onImportData(void) = 0;
	
private:
	medAbstractSourceDataPluginPrivate *d;
};

#endif
