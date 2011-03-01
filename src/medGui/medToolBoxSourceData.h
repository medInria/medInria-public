#ifndef MEDTOOLBOXSOURCEDATA_H
#define MEDTOOLBOXSOURCEDATA_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxSourceDataPrivate;
class medAbstractSourceDataPlugin;

/** 
 * @class medToolBoxSourceData
 * @brief Abstract base class for toolboxes of dynamic source plugins (e.g. Shanoir, PACS, ...)
 * This class is specialized in the source data plugins and instantiated by medToolBoxFactory
 **/

class MEDGUI_EXPORT medToolBoxSourceData : public medToolBox
{
	Q_OBJECT
	
public:
	medToolBoxSourceData(QWidget *parent = 0);
	virtual ~medToolBoxSourceData(void);
	
	/** Returns a pointer to the parent plugin (needs to be reimplemented) */
	virtual medAbstractSourceDataPlugin *plugin() = 0;
	
protected:
	/** Parent Qt widget */
	QWidget *parent(void);
	
private:
	medToolBoxSourceDataPrivate *d;
};

#endif
