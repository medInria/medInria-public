/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <vtkCommand.h>
#include <vtkInriaInteractorStyleDrawPolygon.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

vtkStandardNewMacro(vtkInriaInteractorStyleDrawPolygon)

//-----------------------------------------------------------------------------
class vtkInriaInteractorStyleDrawPolygon::vtkInternal
{
public:
    std::vector<vtkVector2i> points;

    void AddPoint(const vtkVector2i &point)
    {
        this->points.push_back(point);
    }

    void AddPoint(int x, int y)
    {
        this->AddPoint(vtkVector2i(x, y));
    }

    vtkVector2i GetPoint(vtkIdType index) const
    {
        return this->points[index];
    }

    vtkIdType GetNumberOfPoints() const
    {
        return this->points.size();
    }

    void Clear()
    {
        this->points.clear();
    }

    void DrawPixels(const vtkVector2i& StartPos,
                    const vtkVector2i& EndPos, unsigned char *pixels, int *size)
    {
        int x1 = StartPos.GetX(), x2 = EndPos.GetX();
        int y1 = StartPos.GetY(), y2 = EndPos.GetY();

        double x = x2 - x1;
        double y = y2 - y1;
        double length = sqrt( x*x + y*y );
        if(length == 0)
        {
            return;
        }
        double addx = x / length;
        double addy = y / length;

        x = x1;
        y = y1;
        int row, col;
        for(double i = 0; i < length; i += 1)
        {
            col = static_cast<int>(x);
            row = static_cast<int>(y);
            pixels[3*(row*size[0]+col)] = 0;
            pixels[3*(row*size[0]+col)+1] = 255;
            pixels[3*(row*size[0]+col)+2] = 0;
            x += addx;
            y += addy;
        }
    }
};

//----------------------------------------------------------------------------
vtkInriaInteractorStyleDrawPolygon::vtkInriaInteractorStyleDrawPolygon()
{
    this->Internal = new vtkInternal();
    this->StartPosition[0] = this->StartPosition[1] = 0;
    this->EndPosition[0]   = this->EndPosition[1]   = 0;
    this->On = 0;
    this->PixelArray = vtkUnsignedCharArray::New();
}

//----------------------------------------------------------------------------
vtkInriaInteractorStyleDrawPolygon::~vtkInriaInteractorStyleDrawPolygon()
{
    this->PixelArray->Delete();
    delete this->Internal;
}

//----------------------------------------------------------------------------
std::vector<vtkVector2i> vtkInriaInteractorStyleDrawPolygon::GetPolygonPoints()
{
    return this->Internal->points;
}

void vtkInriaInteractorStyleDrawPolygon::OnEnter()
{
    // Polygons do not stay hidden if the user hides and shows the window
    vtkRenderWindow *renWin = this->Interactor->GetRenderWindow();
    int *size = renWin->GetSize();
    renWin->GetPixelData(0, 0, size[0]-1, size[1]-1, 1, this->PixelArray);
    DrawPolygon();
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStyleDrawPolygon::OnLeftButtonDown()
{
    if (this->Interactor)
    {
        if (!this->Interactor->GetControlKey()) // Users are not allowed to use CTRL-click
        {
            if (!this->On)
            {
                this->On = 1;
                vtkRenderWindow *renWin = this->Interactor->GetRenderWindow();

                this->StartPosition[0] = this->Interactor->GetEventPosition()[0];
                this->StartPosition[1] = this->Interactor->GetEventPosition()[1];
                this->EndPosition[0] = this->StartPosition[0];
                this->EndPosition[1] = this->StartPosition[1];

                this->PixelArray->Initialize();
                this->PixelArray->SetNumberOfComponents(3);
                int *size = renWin->GetSize();
                this->PixelArray->SetNumberOfTuples(size[0] * size[1]);

                renWin->GetPixelData(0, 0, size[0]-1, size[1]-1, 1, this->PixelArray);

                this->Internal->Clear();
                this->Internal->AddPoint(this->StartPosition[0], this->StartPosition[1]);
                this->InvokeEvent(vtkCommand::StartInteractionEvent);
            }
            else
            {
                this->EndPosition[0] = this->Interactor->GetEventPosition()[0];
                this->EndPosition[1] = this->Interactor->GetEventPosition()[1];
                int *size = this->Interactor->GetRenderWindow()->GetSize();
                if (this->EndPosition[0] > (size[0]-1))
                {
                    this->EndPosition[0] = size[0]-1;
                }
                if (this->EndPosition[0] < 0)
                {
                    this->EndPosition[0] = 0;
                }
                if (this->EndPosition[1] > (size[1]-1))
                {
                    this->EndPosition[1] = size[1]-1;
                }
                if (this->EndPosition[1] < 0)
                {
                    this->EndPosition[1] = 0;
                }

                vtkVector2i lastPoint =
                        this->Internal->GetPoint(
                            this->Internal->GetNumberOfPoints() - 1);
                vtkVector2i newPoint(this->EndPosition[0], this->EndPosition[1]);

                if(SquaredNorm((substraction(lastPoint, newPoint))) > 100)
                {
                    this->Internal->AddPoint(newPoint);
                    this->DrawPolygon();
                }
            }
        }
    }
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStyleDrawPolygon::OnLeftButtonUp()
{
    if (this->Interactor && this->On)
    {
        this->InvokeEvent(vtkCommand::SelectionChangedEvent);
        view->viewWidget()->repaint();
    }
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStyleDrawPolygon::DrawPolygon()
{

    vtkNew<vtkUnsignedCharArray> tmpPixelArray;
    tmpPixelArray->DeepCopy(this->PixelArray);
    unsigned char *pixels = tmpPixelArray->GetPointer(0);
    int *size = this->Interactor->GetRenderWindow()->GetSize();

    // Draw each line segment
    for(vtkIdType i = 0; i < this->Internal->GetNumberOfPoints() - 1; i++)
    {
        const vtkVector2i &a = this->Internal->GetPoint(i);
        const vtkVector2i &b = this->Internal->GetPoint(i+1);

        this->Internal->DrawPixels(a, b, pixels, size);
    }

    // Draw a line from the end to the start
    if(this->Internal->GetNumberOfPoints() >= 3)
    {
        const vtkVector2i &start = this->Internal->GetPoint(0);
        const vtkVector2i &end = this->Internal->GetPoint(this->Internal->GetNumberOfPoints() - 1);

        this->Internal->DrawPixels(start, end, pixels, size);
    }

    this->Interactor->GetRenderWindow()->SetPixelData(0, 0, size[0]-1, size[1]-1, pixels, 1);
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStyleDrawPolygon::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    os << indent << "On: " << this->On << endl;
    os << indent << "StartPosition: " << this->StartPosition[0] << "," << this->StartPosition[1] << endl;
    os << indent << "EndPosition: "   << this->EndPosition[0]   << "," << this->EndPosition[1] << endl;
}

void vtkInriaInteractorStyleDrawPolygon::ResetPolygon()
{
    this->On = 0;
    this->Internal->Clear();
}

//----------------------------------------------------------------------------
void vtkInriaInteractorStyleDrawPolygon::SetView(medAbstractView *view)
{
    this->view = view;
}
