/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"
#include "medToolBox.h"

class medSHViewToolBoxPrivate;

/**
 * @class medSHViewToolBox
 * @brief Toolbox with widgets to control settings of Spherical Harmonics image visualization.
 * A toolbox with widgets that allow the user to change values which are used by
 * later SH visualizers.
 * For instance, users can change the scale, or choose to calculate
 * more or less polygons for each glyph.
 */
class MEDGUI_EXPORT medSHViewToolBox : public medToolBox
{
	Q_OBJECT

public:
        medSHViewToolBox(QWidget *parent);
        ~medSHViewToolBox();

        /** Returns the currently selected polyhedron that is going to be tesselated */
        QString tesselationType();

        /** Returns the currently selected spherical harmonics basis */
        QString tesselationBasis();

	/** Returns the currently selected sample rate */
	int sampleRate();

	/** Returns true is 'Flip X' checkbox is selected, false otherwise */
	bool isFlipX();

    /** Returns true is 'Flip Y' checkbox is selected, false otherwise */
    bool isFlipY();

    /** Returns true is 'Flip Z' checkbox is selected, false otherwise */
    bool isFlipZ();

    /** Returns true is MaxThesisFunc spherical harmonics checkbox is selected, false otherwise */
    bool isEnhanced();

//    /** Returns the currently selected eigen vector for color coding.
//     * Note that is not the eigen vector index which is being returned,
//     * hence the possible values are 1, 2 or 3.
//     * @return the currently selected eigen vector (either 1, 2 or 3)
//     */
//    int eigenVector();

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

    /** The SH toolbox widgets (slider size and steps) are updated accordingly with the SH image size by using the interactor*/
    void updateWithInteractor(dtkAbstractView *view);

signals:
    /** Emitted when a tesselation polyhedron type has is selected from the combobox */
    void tesselationTypeChanged(const QString& shape);

    /** Emitted when a spherical harmonics basis is selected from the combobox */
    void tesselationBasisChanged(const QString& basis);

    /** Emitted when the sample rate change */
    void sampleRateChanged(int sampleRate);

    /** Emitted when the user wants to flip the X axis */
    void flipX(const bool flipX);

    /** Emitted when the user wants to flip the Y axis */
    void flipY(const bool flipY);

    /** Emitted when the user wants to flip the Z axis */
    void flipZ(const bool flipZ);

    /** Emitted when the checks the normalize check box */
    void normalize(const bool normalize);

    /** Emitted when the glyph resolution change */
    void glyphResolutionChanged(int glyphResolution);

    void xSliceChanged(const int xSlice);
    void ySliceChanged(const int ySlice);
    void zSliceChanged(const int zSlice);

    /** Emitted when the minor scaling change */
    void scalingChanged(const double scale);

    /** Emitted when user wants to hide or show axial plane */
    void hideShowAxial(const bool show);

    /** Emitted when user wants to hide or show coronal plane */
    void hideShowCoronal(const bool show);

    /** Emitted when user wants to hide or show Sagittal plane */
    void hideShowSagittal(const bool show);

public slots:

    void update(dtkAbstractView *view);

private slots:

    // functions used to translate checkboxes from Qt::CheckState
    // to boolean values, and then emitting the signals
    void onFlipXCheckBoxStateChanged(const int state);
    void onFlipYCheckBoxStateChanged(const int state);
    void onFlipZCheckBoxStateChanged(const int state);

    void onEnhanceCheckBoxStateChanged(const int state);

    void onScaleChanged(const double scale);
    void onScaleChanged(const double mantissa,const int exponent);

    /** Changes the Major scaling M of the glyph visualization m*10^M */
    void onMinorScalingChanged(const int minorScale);

    /** Changes the minor scaling m of the glyph visualization  m*10^M*/
    void onMajorScalingChanged(const int majorScaleExponent);

    void onHideShowAxialChanged(const int checkBoxState);
    void onHideShowCoronalChanged(const int checkBoxState);
    void onHideShowSagittalChanged(const int checkBoxState);

protected:

    medSHViewToolBoxPrivate *d;
};
