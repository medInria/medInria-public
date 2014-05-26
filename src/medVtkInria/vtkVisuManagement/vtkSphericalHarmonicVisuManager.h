/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>
#include <vtkSphericalHarmonicSource.h>


class vtkSphericalHarmonicSource;
class vtkSphericalHarmonicGlyph;
class vtkExtractVOI;
class vtkPolyDataMapper;
class vtkActor;
class vtkImageData;

class MEDVTKINRIA_EXPORT vtkSphericalHarmonicVisuManager : public vtkObject {
public:

    static vtkSphericalHarmonicVisuManager *New();
    vtkTypeRevisionMacro(vtkSphericalHarmonicVisuManager, vtkObject);

    void SetGlyphScale (const float& scale);

    /** Get the Polyhedron type to be tesselated */

    void SetTesselationType (const int& type);

    /** Set the Polyhedron type to be tesselated */

    int GetTesselationType() const
    { return this->SHSource->GetTesselationType(); }

    /** Get the spherical harmonics basis to be used*/

    void SetTesselationBasis (const int& type);

    /** Set the spherical harmonics basis to be used */

    int GetTesselationBasis() const
    { return this->SHSource->GetTesselationBasis(); }

    void SetGlyphResolution (const int&);
    void SetOrder (const int&);

    void SetSampleRate (const int&,const int&, const int&);

    void FlipX (const int&);
    void FlipY (const int&);
    void FlipZ (const int&);
    void ColorGlyphs (const int&);
    void SetNormalization (const int& a);

    /** Set the Volume Of Interest (VOI). Consists in
      * 6 integers: xmin, xmax, ymin, ymax, zmin, zmax.*/

    void SetVOI(const int&,const int&,const int&,const int&,const int&,const int&);

    void SetInput (vtkImageData*);

    /** Get the object that computes the spherical harmonic glyph*/

    vtkGetObjectMacro (SHSource, vtkSphericalHarmonicSource);

    /** Get the object that controls the display the spherical harmonic glyph*/

    vtkGetObjectMacro (SHGlyph,  vtkSphericalHarmonicGlyph);

    /** Get the VOI to be displayed as spherical harmonic glyph*/

    vtkGetObjectMacro (VOI,      vtkExtractVOI);

    /** Get the normals used to display the spherical harmonic glyph*/

    vtkGetObjectMacro (Mapper,   vtkPolyDataMapper);

    vtkGetObjectMacro (Actor,    vtkActor);

    /** Set into the slice VisuManager the transformation matrix that is used
      * to display the glyphs according to the image coordinate system*/

    vtkSetObjectMacro (MatrixT, vtkMatrix4x4);

    /** Get from the slice VisuManager the transformation matrix that is used
      * to display the glyphs according to the image coordinate system*/

    vtkGetObjectMacro (MatrixT, vtkMatrix4x4);

protected:

    vtkSphericalHarmonicVisuManager();
    ~vtkSphericalHarmonicVisuManager();

private:

    vtkSphericalHarmonicSource* SHSource;
    vtkSphericalHarmonicGlyph*  SHGlyph;
    vtkExtractVOI*              VOI;
    vtkPolyDataMapper*          Mapper;
    vtkActor*                   Actor;

    vtkMatrix4x4*               MatrixT;
};
