#ifndef MEDTOOLBOXTENSOR_H
#define MEDTOOLBOXTENSOR_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxTensorPrivate;

class MEDGUI_EXPORT medToolBoxTensor : public medToolBox
{
	Q_OBJECT

public:
	medToolBoxTensor(QWidget *parent);
	~medToolBoxTensor();

signals:

public slots:
    void update (dtkAbstractView *view);

    void onGlyphShapeChanged(QString shape);

protected:
    medToolBoxTensorPrivate *d;
};

#endif // MEDTOOLBOXTENSOR_H
