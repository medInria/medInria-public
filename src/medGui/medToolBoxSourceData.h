#ifndef MEDTOOLBOXSOURCEDATA_H
#define MEDTOOLBOXSOURCEDATA_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxSourceDataPrivate;
class medAbstractSourceDataPlugin;

class MEDGUI_EXPORT medToolBoxSourceData : public medToolBox
{
	Q_OBJECT
	
public:
	medToolBoxSourceData(QWidget *parent = 0);
	virtual ~medToolBoxSourceData(void);
	
	virtual medAbstractSourceDataPlugin *plugin() = 0;
	
signals:
	void import();
	
protected:
	QWidget *parent(void);
	
private:
	medToolBoxSourceDataPrivate *d;
};

#endif
