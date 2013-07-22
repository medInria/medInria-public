/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCore/medSHAbstractViewInteractor.h>
#include <v3dViewPluginExport.h>

class v3dViewSHInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

/**
 * @class v3dViewSHInteractor
 * @brief Extents a view by providing Spherical harmonics viewing/managing capabilities.
 *
 * An interactor is an extension to a view (v3dView in this case)
 * that provides additional functionality. This class extents the view by
 * adding SH handling capabilities like visualization and SH-specific
 * properties.
 */
class V3DVIEWPLUGIN_EXPORT v3dViewSHInteractor: public medSHAbstractViewInteractor
{

    Q_OBJECT

public:
    v3dViewSHInteractor();
    virtual ~v3dViewSHInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setData(dtkAbstractData *data);
    dtkAbstractData *data ();

    virtual void setView(dtkAbstractView *view);
    dtkAbstractView *view ();

    virtual void enable();
    virtual void disable();

    /** get the image size it is used to set med gui slider appropiate size*/
    virtual void imageSize(int* range);

public slots:
    virtual void onPropertySet (const QString& key, const QString& value);

    /** Change glyph shape */
    void onTesselationTypePropertySet (const QString& tesselationType);

    /** Change glyph shape */
    void onTesselationBasisPropertySet (const QString& tesselationBasis);

    /** Modify sample rate */
    void onSampleRatePropertySet (int sampleRate);

    /** Flip SHs along the X axis */
    void onFlipXPropertySet (const QString& flipX);

    /** Flip SHs along the Y axis */
    void onFlipYPropertySet (const QString& flipY);

    /** Flip SHs along the Z axis */
    void onFlipZPropertySet (const QString& flipZ);

    /** Turn on/off coloring of glyph with input scalar data or directions. If false, or input scalar data not present, then the
    scalars from the source object are passed through the filter.*/
    void ColorGlyphsPropertySet (const QString& ColorGlyph);

    void NormalizationPropertySet (const QString& Norma);

    //    /** A new eigenvector for mapping the color mode is set */
    //    void onEigenVectorPropertySet (int eigenVector);

    /** Glyph resolution changed */
    void onGlyphResolutionPropertySet (int glyphResolution);

    //    /** Background color reverted */
    //    void onReverseBackgroundColorPropertySet (bool isWhite);

    /** Scaling changed */
    void onScalingPropertySet (double scale);

    /** Slice x changed */
    void onXSlicePropertySet (int xSlice);

    /** Slice  y changed */
    void onYSlicePropertySet (int ySlice);

    /** Slice z changed */
    void onZSlicePropertySet (int zSlice);


    /** Hide or show axial slice */
    void onHideShowAxialPropertySet(bool show);

    /** Hide or show coronal slice */
    void onHideShowCoronalPropertySet(bool show);

    /** Hide or show sagittal slice */
    void onHideShowSagittalPropertySet(bool show);

    /** Change the position of the slices */
    void onPositionChanged(const QVector3D& position, bool propagate);

private:

    v3dViewSHInteractorPrivate* d;
};

dtkAbstractViewInteractor *createV3dViewSHInteractor();
