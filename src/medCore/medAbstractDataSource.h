#ifndef MEDABSTRACTDATASOURCE_H
#define MEDABSTRACTDATASOURCE_H

#include <dtkCore/dtkAbstractObject.h>
#include "medCoreExport.h"

class medAbstractDataSourcePrivate;
class medToolBox;

/** 
 * @class medAbstractDataSource
 * @brief Abstract base class for dynamic source plugins (e.g. Shanoir, PACS, ...)
 * This class adds specific methods to populate the browser area and import data.
 * A dynamic source data plugin derives from this class instead of dtkPlugin.
 **/

class MEDCORE_EXPORT medAbstractDataSource : public dtkAbstractObject
{
	Q_OBJECT
	
public:
	medAbstractDataSource(void);
	~medAbstractDataSource(void);

	/** Returns the main widget (explorer area of the source data plugin, on the left side of the browser area) */
	virtual QWidget *widget() = 0;
	
	/** Returns the source selector widget (what's inside the source selector tab on the right side of the browser area) */
	virtual QWidget *sourceSelectorWidget() = 0;
	
	/** Returns the tab name for the source selector panel */
	virtual QString tabName() = 0;
  
  /** Returns the list of toolboxes owned by the source data plugin */
  virtual unsigned int getNumberOfAdditionalToolBoxes() = 0;
  
  /** Returns the list of toolboxes owned by the source data plugin */
  virtual medToolBox * getAdditionalToolBox(unsigned int i) = 0;

signals:
  /** A source data emits a signal when it succesfully got a data and is ready for the browser to import it*/
	void dataImport(QString dataName);
  
  /** A source data emits a signal when it failed to get a data so that the browser does the appropriate thing */
	void getDataFailed(QString dataName);
	
public slots:
	/** Prepares the data for import by the database in medinria. For example, this can 
   *  download the data from a server andthen will call dataImport to send it to the database importer */
	virtual void onImportData(void) = 0;
	
private:
	medAbstractDataSourcePrivate *d;
};

#endif
