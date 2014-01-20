/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medVtkInriaExport.h"

#include <vtkObject.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>

#include <vtkSphericalHarmonicVisuManager.h>

class MEDVTKINRIA_EXPORT vtkSphericalHarmonicManager: public vtkObject {
public:

    static vtkSphericalHarmonicManager *New();
    vtkTypeRevisionMacro(vtkSphericalHarmonicManager, vtkObject);

    void SetRenderWindowInteractor(vtkRenderWindowInteractor*, vtkRenderer* ren=0);
    vtkGetObjectMacro(RenderWindowInteractor, vtkRenderWindowInteractor);

    vtkSetObjectMacro(Input, vtkImageData);
    vtkGetObjectMacro(Input, vtkImageData);

    /** Get/set the transformation matrix that is used to display the glyphs
     *  according to the image coordinate system */
    vtkSetObjectMacro(MatrixT,vtkMatrix4x4);
    vtkGetObjectMacro(MatrixT, vtkMatrix4x4);

    vtkGetObjectMacro(SHVisuManagerAxial,    vtkSphericalHarmonicVisuManager);
    vtkGetObjectMacro(SHVisuManagerSagittal, vtkSphericalHarmonicVisuManager);
    vtkGetObjectMacro(SHVisuManagerCoronal,  vtkSphericalHarmonicVisuManager);

    /** Set a scaling factor for the glyphs. */
    void SetGlyphScale(const float& f);

    /** Set the sample rate 1 over n Spherical Harmonic's will be
     *  displayed.*/
    void SetSampleRate(const int&,const int&,const int&);

    /** Set the glyph resolution */
    void SetGlyphResolution(const int res);

    /** Set the Polyhedron type to be tesselated */
    void SetTesselationType(const int& type);

    /** Set the spherical harmonics basis to be used */
    void SetTesselationBasis(const int& type);

    /** Set the spherical harmonics order to be used */
    void SetOrder(const int order);

    /** Flip Spherical Harmonic's along the X axis */
    void FlipX(const bool a);

    /** Flip Spherical Harmonic's along the Y axis */
    void FlipY(const bool a);

    /** Flip Spherical Harmonic's along the Z axis */
    void FlipZ(const bool a);

    /** Turn on/off coloring of glyph with input scalar data or directions.
     *  If false, or input scalar data not present, then the
     *  scalars from the source object are passed through the filter.*/
    void ColorGlyphs(const bool a);

    /** Turn on/off Normalizaiton.*/
    void Normalization(const bool a);

    /** get the image size it is used to set med gui slider appropiate size*/
    void GetSphericalHarmonicDimensions(int * dims);

    /** Generate the glyph and add them to the RenderWindowInteractor */
    void Update();

    /** Remove the actors from the renderer. */
    void Initialize();

    /** Set the current position in voxel coordinates. */
    void SetCurrentPosition(const int&, const int&, const int&);

    /** Set the current position in voxel coordinates. */
    void SetCurrentPosition(const int pos[3]);

    /** Get the current position. */
    const int* GetCurrentPosition() const { return this->CurrentPosition; }

    /** Get the current position. */
    void GetCurrentPosition(int pos[3]) const {
        for(int i=0;i<3;++i)
            pos[i] = this->CurrentPosition[i];
    }

    /** Reset the position to the center of the dataset. */
    void ResetPosition();

    /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
    void SetAxialSliceVisibility(const int);

    /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
    void SetSagittalSliceVisibility(const int);

    /** Set the Axial/Coronal/Sagittal slice visibility on or off. */
    void SetCoronalSliceVisibility(const int);

protected:

    vtkSphericalHarmonicManager();
    ~vtkSphericalHarmonicManager();

private:

    static int clamp(const int value,const int bound) {
        return (value>bound-1) ? (bound-1) : value;
    }

    vtkRenderWindowInteractor* RenderWindowInteractor;

    vtkImageData* Input;
    vtkMatrix4x4* MatrixT;

    vtkRenderer* Renderer;

    vtkSphericalHarmonicVisuManager* SHVisuManagerAxial;
    vtkSphericalHarmonicVisuManager* SHVisuManagerSagittal;
    vtkSphericalHarmonicVisuManager* SHVisuManagerCoronal;

    int CurrentPosition[3];
};
