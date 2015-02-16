/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageViewInteractor.h>
#include <medItkSHImageExport.h>

class medVtkViewItkSHImageInteractorPrivate;

class medAbstractData;
class dtkAbstractView;

/**
 * @class medVtkViewItkSHImageInteractor
 * @brief Extents a view by providing Spherical harmonics viewing/managing capabilities.
 *
 * An interactor is an extension to a view (v3dView in this case)
 * that provides additional functionality. This class extents the view by
 * adding SH handling capabilities like visualization and SH-specific
 * properties.
 */
class MEDITKSHIMAGEPLUGIN_EXPORT medVtkViewItkSHImageInteractor: public medAbstractImageViewInteractor
{

    Q_OBJECT

public:
    medVtkViewItkSHImageInteractor(medAbstractView* parent);
    virtual ~medVtkViewItkSHImageInteractor();

public:

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

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setInputData(medAbstractData * data);

    virtual QWidget* buildLayerWidget();
    virtual QWidget* buildToolBoxWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();

    void removeData();

    /** get the image size it is used to set med gui slider appropiate size*/
    virtual void imageSize(int* range);

public slots:

    /** Change glyph shape */
    void setTesselationType (QString tesselationType);

    /** Change glyph shape */
    void setTesselationBasis (QString tesselationBasis);

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
    void setScale (double scale);

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
    void setPosition(const QVector3D& position);


    void setMajorScaling(int majorScalingExponent);
    void setMinorScaling(int minorScaling);
    void setScale(int minorScale, int majorScaleExponent);


    void setOpacity(double opacity);
    void setVisibility(bool visibility);
    void setWindowLevel(QHash<QString, QVariant>);

    virtual void setUpViewForThumbnail();

    virtual void updateWidgets();

    virtual void moveToSlice  (int slice);

protected:
    void setupParameters();

private:
    static QStringList dataHandled();
    void update();

private slots:
    void updateSlicingParam();

private:

    medVtkViewItkSHImageInteractorPrivate* d;
};
