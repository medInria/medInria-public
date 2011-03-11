#ifndef MEDTOOLBOXDIFFUSIONTENSORVIEW_H
#define MEDTOOLBOXDIFFUSIONTENSORVIEW_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxDiffusionTensorViewPrivate;

/**
 * @class medToolBoxDiffusionTensorView
 * @brief Toolbox with widgets to control settings of tensor image visualization.
 * A toolbox with widgets that allow the user to change values which are used by
 * later tensor visualizers.
 * For instance, users can change the glyph shape, or choose to calculate
 * more or less polygons for each glyph.
 */
class MEDGUI_EXPORT medToolBoxDiffusionTensorView : public medToolBox
{
	Q_OBJECT

public:
	medToolBoxDiffusionTensorView(QWidget *parent);
	~medToolBoxDiffusionTensorView();

signals:
    /** Emitted when a glyph shape is selected from the combobox */
    void glyphShapeChanged(const QString& shape);

    /** Emitted when a the sample rate change */
    void sampleRateChanged(int sampleRate);


public slots:
    void update (dtkAbstractView *view);

protected:
    medToolBoxDiffusionTensorViewPrivate *d;
};

#endif // MEDTOOLBOXDIFFUSIONTENSORVIEW_H
