#ifndef medSHViewToolBox_H
#define medSHViewToolBox_H

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
        QString tesselationType(void);

        /** Returns the currently selected spherical harmonics basis */
        QString tesselationBasis(void);

	/** Returns the currently selected sample rate */
	int sampleRate(void);

	/** Returns true is 'Flip X' checkbox is selected, false otherwise */
	bool isFlipX(void);

    /** Returns true is 'Flip Y' checkbox is selected, false otherwise */
    bool isFlipY(void);

    /** Returns true is 'Flip Z' checkbox is selected, false otherwise */
    bool isFlipZ(void);

    /** Returns true is MaxThesisFunc spherical harmonics checkbox is selected, false otherwise */
    bool isMaxThesisFunc(void);

//    /** Returns the currently selected eigen vector for color coding.
//     * Note that is not the eigen vector index which is being returned,
//     * hence the possible values are 1, 2 or 3.
//     * @return the currently selected eigen vector (either 1, 2 or 3)
//     */
//    int eigenVector(void);

    /** Returns the currently selected glyph resolution */
    int glyphResolution(void);

    /** Returns the current scale */
    double scale(void);

    /** Returns true is 'Hide/Show Axial' checkbox is selected, false otherwise */
    bool isShowAxial(void);

    /** Returns true is 'Hide/Show Coronal' checkbox is selected, false otherwise */
    bool isShowCoronal(void);

    /** Returns true is 'Hide/Show Sagittal' checkbox is selected, false otherwise */
    bool isShowSagittal(void);

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
    void flipX(bool flipX);

    /** Emitted when the user wants to flip the Y axis */
    void flipY(bool flipY);

    /** Emitted when the user wants to flip the Z axis */
    void flipZ(bool flipZ);

    /** Emitted when the checks the normalize check box */
    void normalize(bool normalize);

//    /** Emitted when a new eigen vector is selected */
//    void eigenVectorChanged(int eigenVector);

//    /** Emitted when a new eigen vector is selected */
//    void reverseBackgroundColor(bool reverseBackgroundColor);

    /** Emitted when the glyph resolution change */
    void glyphResolutionChanged(int glyphResolution);

    void xSliceChanged(int xSlice);
    void ySliceChanged(int ySlice);
    void zSliceChanged(int zSlice);

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

    void onNormalizeCheckBoxStateChanged(int state);


//    void onEigenVectorV1Toggled(bool isSelected);
//    void onEigenVectorV2Toggled(bool isSelected);
//    void onEigenVectorV3Toggled(bool isSelected);

//    void onReverseBackgroundColorChanged(int state);

    /** Changes the Major scaling M of the glyph visualization m*10^M */
    void onMinorScalingChanged(int minorScale);

    /** Changes the minor scaling m of the glyph visualization  m*10^M*/
    void onMajorScalingChanged(int majorScaleExponent);

    void onHideShowAxialChanged(int checkBoxState);
    void onHideShowCoronalChanged(int checkBoxState);
    void onHideShowSagittalChanged(int checkBoxState);

protected:
    medSHViewToolBoxPrivate *d;
};

#endif // medSHViewToolBox_H
