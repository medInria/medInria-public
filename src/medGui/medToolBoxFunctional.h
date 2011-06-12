#ifndef MEDTOOLBOXFUNCTIONAL_H
#define MEDTOOLBOXFUNCTIONAL_H

#include "medGuiExport.h"
#include "medToolBox.h"
#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkAbstractView.h"

class medToolBoxFunctionalPrivate;

class MEDGUI_EXPORT medToolBoxFunctional : public medToolBox
{
	Q_OBJECT

public:
	medToolBoxFunctional(QWidget *parent = 0);
	~medToolBoxFunctional();

	dtkAbstractData *output(void) const;

signals:

public slots:
    void clear();

    void update (dtkAbstractView *view);

    void onViewAdded(dtkAbstractView *view);

    void onToolBoxChosen(const QString& tb);

private slots:


protected:
    medToolBoxFunctionalPrivate *d;
};

#endif // MEDTOOLBOXFUNCTIONAL_H
