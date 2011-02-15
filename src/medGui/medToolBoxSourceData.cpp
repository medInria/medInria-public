#include "medToolBoxSourceData.h"

class medToolBoxSourceDataPrivate
{
public:
	QWidget *parent;
	medAbstractSourceDataPlugin *plugin;
};

medToolBoxSourceData::medToolBoxSourceData(QWidget *parent) : medToolBox(parent), d(new medToolBoxSourceDataPrivate)
{
	d->parent = parent;
	d->plugin = NULL;
}

medToolBoxSourceData::~medToolBoxSourceData(void)
{
	if (d)
		delete d;
	
	d = NULL;
}

QWidget *medToolBoxSourceData::parent(void)
{
	return d->parent;
}
