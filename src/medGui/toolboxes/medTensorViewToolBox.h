/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"
#include "medToolBox.h"

class medTensorViewToolBoxPrivate;

/**
 * @class medTensorViewToolBox
 * @brief Toolbox with widgets to control settings of tensor image visualization.
 * A toolbox with widgets that allow the user to change values which are used by
 * later tensor visualizers.
 * For instance, users can change the glyph shape, or choose to calculate
 * more or less polygons for each glyph.
 */
class MEDGUI_EXPORT medTensorViewToolBox : public medToolBox
{
	Q_OBJECT

public:
	medTensorViewToolBox(QWidget *parent);
	~medTensorViewToolBox();

	/** Returns the currently selected glyph shape */
	QString glyphShape();

	/** Returns the currently selected sample rate */
	int sampleRate();

	/** Returns true is 'Flip X' checkbox is selected, false otherwise */
	bool isFlipX();

    /** Returns true is 'Flip Y' checkbox is selected, false otherwise */
    bool isFlipY();

    /** Returns true is 'Flip Z' checkbox is selected, false otherwise */
    bool isFlipZ();

    /** Returns the currently selected eigen vector for color coding.
     * Note that is not the eigen vector index which is being returned,
     * hence the possible values are 1, 2 or 3.
     * @return the currently selected eigen vector (either 1, 2 or 3)
     */
    int eigenVector();

    /** Returns the currently selected glyph resolution */
    int glyphResolution();

    /** Returns the current scale */
    double scale();

    /** Returns true is 'Hide/Show Axial' checkbox is selected, false otherwise */
    bool isShowAxial();

    /** Returns true is 'Hide/Show Coronal' checkbox is selected, false otherwise */
    bool isShowCoronal();

    /** Returns true is 'Hide/Show Sagittal' checkbox is selected, false otherwise */
    bool isShowSagittal();

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
    void scalingChanged(double scale);

    /** Emitted when user wants to hide or show axial plane */
    void hideShowAxial(bool show);

    /** Emitted when user wants to hide or show coronal plane */
    void hideShowCoronal(bool show);

    /** Emitted when user wants to hide or show Sagittal plane */
    void hideShowSagittal(bool show);

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

    void onMinorScalingChanged(int minorScale);
    void onMajorScalingChanged(int majorScaleExponent);

    void onHideShowAxialChanged(int checkBoxState);
    void onHideShowCoronalChanged(int checkBoxState);
    void onHideShowSagittalChanged(int checkBoxState);

protected:
    medTensorViewToolBoxPrivate *d;
};


