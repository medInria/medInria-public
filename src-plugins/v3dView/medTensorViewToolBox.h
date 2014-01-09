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
class V3DVIEWPLUGIN_EXPORT medTensorViewToolBox : public medToolBox
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

    static bool registered();

public slots:
    void update (dtkAbstractView *view);

private slots:

    void setGlyphShape(int glyphShape);
    void setSampleRate(int SampleRate);
    void setGlyphResolution(int glyphResolution);

    void setFlipX(int state);
    void setFlipY(int state);
    void setFlipZ(int state);

    void setEigenVectorV1(bool isSelected);
    void setEigenVectorV2(bool isSelected);
    void setEigenVectorV3(bool isSelected);

    void onReverseBackgroundColorChanged(int state);

    void setMinorScaling(int minorScale);
    void setMajorScaling(int majorScaleExponent);

    void setShowAxial(int checkBoxState);
    void setShowCoronal(int checkBoxState);
    void setShowSagittal(int checkBoxState);

protected:
    medTensorViewToolBoxPrivate *d;
};


