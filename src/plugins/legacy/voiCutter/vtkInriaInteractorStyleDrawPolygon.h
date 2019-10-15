/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractView.h>
#include <vector>      // For returning Polygon Points
#include <vtkInteractorStyle.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVector.h> // For Polygon Points


class vtkInriaInteractorStyleDrawPolygon : public vtkInteractorStyle
{
public:
    static vtkInriaInteractorStyleDrawPolygon *New();
    vtkTypeMacro(vtkInriaInteractorStyleDrawPolygon, vtkInteractorStyle)
    void PrintSelf(ostream& os, vtkIndent indent);

    /**
     * Event: handle cursor entering in the window
     */
    virtual void OnEnter();
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();

    std::vector<vtkVector2i> GetPolygonPoints();
    void ResetPolygon();

    void SetView(medAbstractView *view);

protected:
    vtkInriaInteractorStyleDrawPolygon();
    ~vtkInriaInteractorStyleDrawPolygon();

    virtual void DrawPolygon();

    int StartPosition[2];
    int EndPosition[2];
    int On;
    medAbstractView *view;

    vtkUnsignedCharArray *PixelArray;

private:
    vtkInriaInteractorStyleDrawPolygon(const vtkInriaInteractorStyleDrawPolygon&) = delete;
    void operator=(const vtkInriaInteractorStyleDrawPolygon&) = delete;

    template<typename A, int Size>
    vtkVector<A,Size> substraction(const vtkVector<A, Size> &v1,const vtkVector<A, Size> &v2)
    {
        vtkVector<A, Size> ret;
        for (int i = 0; i < Size; ++i)
            ret[i] = v1[i] - v2[i];
        
        return ret;
    }

    template<typename A, int Size>
    A SquaredNorm(const vtkVector<A, Size> &v) const
    {
        A result = 0;
        for (int i = 0; i < Size; ++i)
        {
            result += v[i] * v[i];
        }
        return result;
    }

    class vtkInternal;
    vtkInternal *Internal;
};
