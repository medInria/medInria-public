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

class medAbstractData;
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
    bool isDataTypeHandled(QString dataType) const;

    static bool registered();

    virtual void setData(medAbstractData *data);
    medAbstractData *data ();

    virtual void setView(dtkAbstractView *view);
    dtkAbstractView *view ();

    virtual void enable();
    virtual void disable();

    /** get the image size it is used to set med gui slider appropiate size*/
    virtual void imageSize(int* range);


    enum TesselationType
    {
        Icosahedron = 0,
        Dodecahedron,
        Octahedron,
        Hexahedron,
        Tetrahedron
    };

    enum TesselationBasis
    {
        SHMatrix = 0,
        SHMatrixMaxThesis,
        SHMatrixTournier,
        SHMatrixRshBasis
    };

public slots:

    /** Change glyph shape */
    void setTesselationType (TesselationType tesselationType);

    /** Change glyph shape */
    void setTesselationBasis (TesselationBasis tesselationBasis);

    /** Modify sample rate */
    void setSampleRate (int sampleRate);

    /** Flip SHs along the X axis */
    void setFlipX (const bool flipX);

    /** Flip SHs along the Y axis */
    void setFlipY (const bool flipY);

    /** Flip SHs along the Z axis */
    void setFlipZ (const bool flipZ);

    /** Turn on/off coloring of glyph with input scalar data or directions. If false, or input scalar data not present, then the
    scalars from the source object are passed through the filter.*/
    void setColorGlyphs (const bool ColorGlyph);

    void setNormalization (const bool Norma);

    //    /** A new eigenvector for mapping the color mode is set */
    //    void onEigenVectorPropertySet (int eigenVector);

    /** Glyph resolution changed */
    void setGlyphResolution (int glyphResolution);

    //    /** Background color reverted */
    //    void onReverseBackgroundColorPropertySet (bool isWhite);

    /** Scaling changed */
    void setScaling (double scale);

    /** Slice x changed */
    void setXSlice (int xSlice);

    /** Slice  y changed */
    void setYSlice (int ySlice);

    /** Slice z changed */
    void setZSlice (int zSlice);


    /** Hide or show axial slice */
    void setShowAxial (bool show);

    /** Hide or show coronal slice */
    void setShowCoronal(bool show);

    /** Hide or show sagittal slice */
    void setShowSagittal(bool show);

    /** Change the position of the slices */
    void setPosition(const QVector3D& position, bool propagate);

private:

    v3dViewSHInteractorPrivate* d;
};

dtkAbstractViewInteractor *createV3dViewSHInteractor();
