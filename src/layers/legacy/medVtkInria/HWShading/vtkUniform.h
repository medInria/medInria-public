/*============================================================================
The Hardware Shading (HWShading) module is protected by the
following copyright:
Copyright (c) 2007 Biomedical Image Analysis (BMIA) - Department of
Biomedical Engineering - Eindhoven University of Technology.
All rights reserved. See Copyright.txt for details.
The HWShading implementation was originally written by Tim Peeters (BMIA - TUe)
and published at the "11th International Fall Workshop on Vision, Modeling,
and Visualization 2006" (VMV'06):
"Visualization of the Fibrous Structure of the Heart", by T. Peeters et al.
See http://timpeeters.com/research/vmv2006 for more information.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

This file is a modified version inspired from the original file by
the medInria team.
============================================================================*/

#pragma once

#include <medVtkInriaExport.h>
#include <vtkShaderUniform.h>

template <unsigned N>
struct Vec {

    Vec(const float v[N]) {
        for (unsigned i=0;i<N;++i)
            Value[i] = v[i];
    }

    operator float*() {
        return Value;
    }

    void operator=(const float& v) {
        for (unsigned i=0;i<N;++i)
            Value[i] = v;
    }

    float& operator[](const int i)       { return Value[i]; }
    float  operator[](const int i) const { return Value[i]; }

    float Value[N];
};

template <unsigned N>
vtkOStreamWrapper& operator<<(vtkOStreamWrapper& os,const Vec<N>& V) {
    for (unsigned i=0;i<N;++i)
        os << V[i] << ' ';
    return os;
}

template <typename T>
class MEDVTKINRIA_EXPORT vtkUniform: public vtkShaderUniform {
public:

    //  Not canonical, but the VTK use of macros obliges this...

    static vtkUniform* New() { return new vtkUniform; }

    vtkTypeMacro(vtkUniform, vtkShaderUniform);

    vtkSetMacro(Value, T);
    vtkGetMacro(Value, T);

protected:

    vtkUniform(): Value(0) { }
    ~vtkUniform() { }

    virtual void SetGlUniformSpecific() {
        glUniform1f(Location, Value);
    }

private:
    T Value;
};

template <unsigned N>
class MEDVTKINRIA_EXPORT vtkUniform<Vec<N> >: public vtkShaderUniform {
public:

    //  Not canonical, but the VTK use of macros obliges this...

    static vtkUniform* New() { return new vtkUniform; }

    vtkTypeMacro(vtkUniform<Vec<N> >, vtkShaderUniform);

    vtkSetVectorMacro(Value,float,(int)N);
    vtkGetVectorMacro(Value,float,(int)N);

protected:

    vtkUniform(): Value(0) { }
    ~vtkUniform() { }

    virtual void SetGlUniformSpecific();

private:

    Vec<N> Value;
};

template<>
inline
void vtkUniform<Vec<2> >::SetGlUniformSpecific() {
    vtkDebugMacro(<< "Calling glUniform2f(" << Location << ", " << Value[0] << ", " << Value[1] << ").");
    glUniform2f(Location,Value[0],Value[1]);
}

template<>
inline
void vtkUniform<Vec<3> >::SetGlUniformSpecific() {
    vtkDebugMacro(<< "Calling glUniform3f(" << Location << ", " << Value[0] << ", " << Value[1] << ", " << Value[2] << ").");
    glUniform3f(Location,Value[0],Value[1],Value[2]);
}

template<>
inline
void vtkUniform<Vec<4> >::SetGlUniformSpecific() {
    vtkDebugMacro(<< "Calling glUniform4f(" << Location << ", " << Value[0] << ", " << Value[1] << ", " << Value[2] << ", " << Value[3] << ").");
    glUniform4f(Location,Value[0],Value[1],Value[2],Value[3]);
}

