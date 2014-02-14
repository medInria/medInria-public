/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "v3dViewPluginExport.h"
#include <medToolBox.h>

class medSHViewToolBoxPrivate;

/**
 * @class medSHViewToolBox
 * @brief Toolbox with widgets to control settings of Spherical Harmonics image visualization.
 * A toolbox with widgets that allow the user to change values which are used by
 * later SH visualizers.
 * For instance, users can change the scale, or choose to calculate
 * more or less polygons for each glyph.
 */
class V3DVIEWPLUGIN_EXPORT medSHViewToolBox : public medToolBox
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


    static bool registered();


public slots:

    void update(dtkAbstractView *view);

private slots:

    void setTesselationType (int tesselationType);
    void setTesselationBasis (int tesselationBasis);
    void setSampleRate (int sampleRate);

    void setXSlice (int xSlice);
    void setYSlice (int ySlice);
    void setZSlice (int zSlice);

    void setGlyphResolution (int glyphResolution);

    void flipX(const int state);
    void flipY(const int state);
    void flipZ(const int state);

    void setEnhance(const int state);

    void setScale(const double scale);
    void setScale(const double mantissa,const int exponent);

    /** Changes the Major scaling M of the glyph visualization m*10^M */
    void setMinorScaling(const int minorScale);

    /** Changes the minor scaling m of the glyph visualization  m*10^M*/
    void setMajorScaling(const int majorScaleExponent);

    void setShowAxial(const int checkBoxState);
    void setShowCoronal(const int checkBoxState);
    void setShowSagittal(const int checkBoxState);

protected:

    medSHViewToolBoxPrivate *d;
};
