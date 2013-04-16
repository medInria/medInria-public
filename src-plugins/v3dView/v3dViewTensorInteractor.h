/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dViewTensorInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

/**
 * @class v3dViewTensorInteractor
 * @brief Extents a view by providing tensor viewing/managing capabilities.
 *
 * An interactor is an extension to a view (v3dView in this case)
 * that provides additional functionality. This class extents the view by
 * adding tensor handling capabilities like visualization and tensor-specific
 * properties.
 */
class V3DVIEWPLUGIN_EXPORT v3dViewTensorInteractor: public dtkAbstractViewInteractor
{

    Q_OBJECT

public:
    v3dViewTensorInteractor();
    virtual ~v3dViewTensorInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setData(dtkAbstractData *data);
    dtkAbstractData *data();

    virtual void setView(dtkAbstractView *view);
    dtkAbstractView *view();

    virtual void enable();
    virtual void disable();

public slots:
    virtual void onPropertySet (const QString& key, const QString& value);

    /** Change glyph shape */
    void onGlyphShapePropertySet (const QString& glyphShape);

    /** Modify sample rate */
    void onSampleRatePropertySet (int sampleRate);

    /** Flip tensors along the X axis */
    void onFlipXPropertySet (const QString& flipX);

    /** Flip tensors along the Y axis */
    void onFlipYPropertySet (const QString& flipY);

    /** Flip tensors along the Z axis */
    void onFlipZPropertySet (const QString& flipZ);

    /** A new eigenvector for mapping the color mode is set */
    void onEigenVectorPropertySet (int eigenVector);

    /** Glyph resolution changed */
    void onGlyphResolutionPropertySet (int glyphResolution);

    /** Background color reverted */
    void onReverseBackgroundColorPropertySet (bool isWhite);

    /** Scaling changed */
    void onScalingPropertySet (double scale);

    /** Hide or show axial slice */
    void onHideShowAxialPropertySet(bool show);

    /** Hide or show coronal slice */
    void onHideShowCoronalPropertySet(bool show);

    /** Hide or show sagittal slice */
    void onHideShowSagittalPropertySet(bool show);

    /** Change position of the slices */
    void onPositionChanged(const QVector3D& position, bool propagate);

private:
    v3dViewTensorInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewTensorInteractor();


