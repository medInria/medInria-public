#ifndef V3DVIEWSHINTERACTOR_H
#define V3DVIEWSHINTERACTOR_H

#include <medCore/medShAbstractViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dViewShInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

/**
 * @class v3dViewShInteractor
 * @brief Extents a view by providing Spherical harmonics viewing/managing capabilities.
 *
 * An interactor is an extension to a view (v3dView in this case)
 * that provides additional functionality. This class extents the view by
 * adding Sh handling capabilities like visualization and Sh-specific
 * properties.
 */
class V3DVIEWPLUGIN_EXPORT v3dViewShInteractor: public medShAbstractViewInteractor
{

    Q_OBJECT

public:
    v3dViewShInteractor();
    virtual ~v3dViewShInteractor();

    virtual QString description(void) const;
    virtual QString identifier(void) const;
    virtual QStringList handled(void) const;

    static bool registered(void);

    virtual void setData(dtkAbstractData *data);
    dtkAbstractData *data (void);

    virtual void setView(dtkAbstractView *view);
    dtkAbstractView *view (void);

    virtual void enable(void);
    virtual void disable(void);

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

    /** Flip Shs along the X axis */
    void onFlipXPropertySet (const QString& flipX);

    /** Flip Shs along the Y axis */
    void onFlipYPropertySet (const QString& flipY);

    /** Flip Shs along the Z axis */
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
    v3dViewShInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewShInteractor(void);

#endif
