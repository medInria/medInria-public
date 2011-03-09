#ifndef MEDTOOLBOXDIFFUSIONTENSORVIEW_H
#define MEDTOOLBOXDIFFUSIONTENSORVIEW_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxDiffusionTensorViewPrivate;

class MEDGUI_EXPORT medToolBoxDiffusionTensorView : public medToolBox
{
	Q_OBJECT

public:
	medToolBoxDiffusionTensorView(QWidget *parent);
	~medToolBoxDiffusionTensorView();

signals:

public slots:
    void update (dtkAbstractView *view);

    void onGlyphShapeChanged(QString shape);

protected:
    medToolBoxDiffusionTensorViewPrivate *d;
};

#endif // MEDTOOLBOXDIFFUSIONTENSORVIEW_H
