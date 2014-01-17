/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractVtkViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dViewTensorInteractorPrivate;

class medAbstractData;
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
class V3DVIEWPLUGIN_EXPORT v3dViewTensorInteractor: public medAbstractVtkViewInteractor
{

    Q_OBJECT

public:
    v3dViewTensorInteractor();
    virtual ~v3dViewTensorInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;
    
    virtual bool isDataTypeHandled(QString dataType) const;

    static bool registered();

    virtual void setData(medAbstractData *data);
    medAbstractData *data();

    virtual void setView(dtkAbstractView *view);
    dtkAbstractView *view();

    virtual void enable();
    virtual void disable();

    enum GlyphShapeType {
        Lines,
        Disks,
        Arrows,
        Cubes,
        Cylinders,
        Ellipsoids,
        Superquadrics
    };

public slots:
    // Mandatory implementations from medVtkViewInteractor
    virtual void setOpacity(medAbstractData * data, double opacity);
    virtual double opacity(medAbstractData * data) const;

    virtual void setVisible(medAbstractData * data, bool visible);
    virtual bool isVisible(medAbstractData * data) const;
    
public slots:
    /** Change glyph shape */
    void setGlyphShape(int glyphShape);

    /** Modify sample rate */
    void setSampleRate(int sampleRate);

    /** Flip tensors along the X axis */
    void setFlipX(bool flip);

    /** Flip tensors along the Y axis */
    void setFlipY(bool flip);

    /** Flip tensors along the Z axis */
    void setFlipZ(bool flip);

    /** A new eigenvector for mapping the color mode is set */
    void setEigenVector(int eigenVector);

    /** Glyph resolution changed */
    void setGlyphResolution(int glyphResolution);

    /** Background color reverted */
    void setReverseBackgroundColor(bool isWhite);

    /** Scaling changed */
    void setScale(double scale);

    /** Hide or show axial slice */
    void setShowAxial(bool show);

    /** Hide or show coronal slice */
    void setShowCoronal(bool show);

    /** Hide or show sagittal slice */
    void setShowSagittal(bool show);

    /** Change position of the slices */
    void changePosition(const QVector3D& position, bool propagate);


    void setMajorScaling(int majorScalingExponent);

    void setMinorScaling(int minorScaling);

    void setScale(int minorScale, int majorScaleExponent);

protected:

    void computeBounds();
    void updateBounds(const double bounds[]);
    void setupParameters(dtkAbstractData *data);


private:
    v3dViewTensorInteractorPrivate *d;

};

dtkAbstractViewInteractor *createV3dViewTensorInteractor();


