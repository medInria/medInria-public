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

    /** Emitted when the sample rate change */
    void sampleRateChanged(int sampleRate);

    /** Emitted when the user wants to flip the X axis */
    void flipX(bool flipX);

    /** Emitted when the user wants to flip the Y axis */
    void flipY(bool flipY);

    /** Emitted when the user wants to flip the Z axis */
    void flipZ(bool flipZ);

    /** Emitted when a new eigen vector is selected */
    void eigenVectorChanged(int eigenVector);

    /** Emitted when a new eigen vector is selected */
    void reverseBackgroundColor(bool reverseBackgroundColor);

    /** Emitted when the glyph resolution change */
    void glyphResolutionChanged(int glyphResolution);

    /** Emitted when the minor scaling change */
    void minorScalingChanged(int minorScaling);

    /** Emitted when the major scaling change */
    void majorScalingChanged(int majorScaling);

public slots:
    void update (dtkAbstractView *view);

private slots:
    // functions used to translate checkboxes from Qt::CheckState
    // to boolean values, and then emitting the signals
    void onFlipXCheckBoxStateChanged(int state);
    void onFlipYCheckBoxStateChanged(int state);
    void onFlipZCheckBoxStateChanged(int state);

    void onEigenVectorV1Toggled(bool isSelected);
    void onEigenVectorV2Toggled(bool isSelected);
    void onEigenVectorV3Toggled(bool isSelected);

    void onReverseBackgroundColorChanged(int state);

protected:
    medToolBoxDiffusionTensorViewPrivate *d;
};

#endif // MEDTOOLBOXDIFFUSIONTENSORVIEW_H
