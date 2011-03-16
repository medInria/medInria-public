#ifndef MEDTOOLBOXSOURCEDATA_H
#define MEDTOOLBOXSOURCEDATA_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxSourceDataPrivate;

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
		
protected:
	/** Parent Qt widget */
	QWidget *parent(void);
	
private:
	medToolBoxSourceDataPrivate *d;
};

#endif
