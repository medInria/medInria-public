//   Authors:		Maxime Descoteaux, Jaime Garcia Guevara, Theodore Papadopoulo.
//
//   Description:  scale and orient glyph according to spherical harmonic representation   
//   vtkSphericalHarmonicGlyph is a filter that copies a SH representation (specified
//   as polygonal data) to every input point. 
//
//   Copyright (c) 2007-2011, INRIA Sophia Antipolis, France, groups Odyssee, Athena.
//   Please see the copyright notice included in this distribution for full details.

#ifndef VTKSPHERICALHARMONICGLYPH_HPP
#define VTKSPHERICALHARMONICGLYPH_HPP

#include <vtkPolyDataAlgorithm.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>

#include <vtkSphericalHarmonicSource.h>

enum { COLOR_BY_SCALARS, COLOR_BY_DIRECTIONS };

#include "SphericalFunctionSHVisualizationExport.h"

class SPHERICALFUNCTIONSHVISUALIZATION_EXPORT vtkSphericalHarmonicGlyph: public vtkPolyDataAlgorithm {
public:

    vtkTypeRevisionMacro(vtkSphericalHarmonicGlyph,vtkPolyDataAlgorithm);
    void PrintSelf(ostream& os,vtkIndent indent);

    // Construct object with scale factor 1.0. Glyphs are colored with input scalar data, RBG map or actual spherical values.

    static vtkSphericalHarmonicGlyph* New();

    // Specify the geometry to copy to each point. Old style. See SetSourceConnection.

    void SetSource(vtkPolyData* source) { this->SetInput(1,source); }
    vtkPolyData *GetSource();

    // Specify the Spherical Harmonic Source that the glyph needs to draw a deform spherical function at every point.

    void SetSphericalHarmonicSource(vtkSphericalHarmonicSource* shs) { this->SphericalHarmonicSource = shs; }

    //vtkSetObjectMacro (SphericalHarmonicSource,vtkSphericalHarmonicSource);

    static const char* GetSphericalHarmonicCoefficientsArrayName() { return "sh_coeff"; }

    // Name of scalar arrays used to color
    //
    // Anisotropy Measure: Single scalar for the whole spherical function
    // RGB: Every point (x,y,z) of the spherical function is colored with rgb = {x,y,z}
    // SH values: Every point of the spherical function colored by its value

    static const char* GetAnisotropyMeasureArrayName()       { return "aniso";     }
    static const char* GetRGBArrayName()                     { return "rgb";       }
    static const char* GetSphericalHarmonicValuesArrayName() { return "sh_values"; }

    // Specify a source object at a specified table location. New style.
    // Source connection is stored in port 1. This method is equivalent to SetInputConnection(1,id,outputPort).

    void SetSourceConnection(int id,vtkAlgorithmOutput* algOutput);
    void SetSourceConnection(vtkAlgorithmOutput* algOutput) { this->SetSourceConnection(0,algOutput); }

    // Specify scale factor to scale object by. (Scale factor always affects output even if scaling is off.)

    vtkSetMacro(ScaleFactor,double);
    vtkGetMacro(ScaleFactor,double);

    // Turn on/off coloring of glyph with input scalar data or directions. If false, or input scalar data not present, then the
    // scalars from the source object are passed through the filter.

    vtkSetMacro(ColorGlyphs,bool);
    vtkGetMacro(ColorGlyphs,bool);
    vtkBooleanMacro(ColorGlyphs,bool);

    vtkSetObjectMacro(TMatrix,vtkMatrix4x4);
    vtkGetObjectMacro(TMatrix,vtkMatrix4x4);

    // Set the color mode to be used for the glyphs. This can be set to use the input scalars (default) or to use the directions at the
    // point.  If ThreeGlyphs is set and the eigenvalues are chosen for coloring then each glyph is colored by the corresponding
    // directions and if not set the color corresponding to the largest direction is chosen.  The recognized values are:
    // COLOR_BY_SCALARS = 0 (default)
    // COLOR_BY_EIGENVALUES = 1

    vtkSetClampMacro(ColorMode,int,COLOR_BY_SCALARS,COLOR_BY_DIRECTIONS);
    vtkGetMacro(ColorMode, int);
    void SetColorModeToScalars()    { this->SetColorMode(COLOR_BY_SCALARS);    }
    void SetColorModeToDirections() { this->SetColorMode(COLOR_BY_DIRECTIONS); }

protected:

    vtkSphericalHarmonicGlyph();
    ~vtkSphericalHarmonicGlyph() { }

    virtual int RequestData(vtkInformation*,vtkInformationVector**,vtkInformationVector*);
    virtual int FillInputPortInformation(int port,vtkInformation *info);

    // Scale factor to use to scale geometry

    double ScaleFactor;

    // Boolean controls coloring with input scalar data

    bool ColorGlyphs;

    // The coloring mode to use for the glyphs.

    int ColorMode;
    vtkSphericalHarmonicSource* SphericalHarmonicSource;
    vtkMatrix4x4*  TMatrix;


private:

    vtkSphericalHarmonicGlyph(const vtkSphericalHarmonicGlyph&);  // Not implemented.
    void operator=(const vtkSphericalHarmonicGlyph&);  // Not implemented.
};

#endif  // ! VTKSPHERICALHARMONICGLYPH_HPP
