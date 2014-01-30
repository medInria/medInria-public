/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dDataFibers.h>

#include <medAbstractDataFactory.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkFiberDataSet.h>

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <QVTKWidget.h>

// /////////////////////////////////////////////////////////////////
// v3dDataFibersPrivate
// /////////////////////////////////////////////////////////////////

class v3dDataFibersPrivate
{
public:
    vtkSmartPointer<vtkFiberDataSet> data;
    QList<QImage>          thumbnails;
    QImage thumbnail;
};

// /////////////////////////////////////////////////////////////////
// v3dDataFibers
// /////////////////////////////////////////////////////////////////

v3dDataFibers::v3dDataFibers() : medAbstractData(), d(new v3dDataFibersPrivate)
{
    d->data = 0;
    d->thumbnail = QImage(":v3dData/icons/fibers.png");
    d->thumbnails.push_back (d->thumbnail);
}

v3dDataFibers::~v3dDataFibers()
{
    delete d;
    d = 0;
}

bool v3dDataFibers::registered()
{
    return medAbstractDataFactory::instance()->registerDataType("v3dDataFibers", createV3dDataFibers);
}

QString v3dDataFibers::description() const
{
    return tr("v3d fibers data") ;
}

QString v3dDataFibers::identifier() const
{
    return "v3dDataFibers";
}

void v3dDataFibers::setData(void *data)
{
  if (vtkFiberDataSet *dataset = vtkFiberDataSet::SafeDownCast (static_cast<vtkObject*>(data)))
      d->data = dataset;
}

void *v3dDataFibers::data()
{
    return d->data;
}

QImage & v3dDataFibers::thumbnail()
{
    /*
    if (d->thumbnails.count()==0)
        this->generateThumbnails();
    */
    return d->thumbnail;
}

QList<QImage> &v3dDataFibers::thumbnails()
{
    /*
    if (d->thumbnails.count()==0)
        generateThumbnails();
    */
    return d->thumbnails;
}

void v3dDataFibers::generateThumbnails() const
{
    d->thumbnails.clear();

    // we must use a parent to prevent infinte recursion of the QVTKWidget
    // see: http://vtk.1045678.n5.nabble.com/qtimageviewer-example-crashes-with-vtk-5-6-0-td1249263.html
    //
    QWidget *parent = new QWidget;

    QVTKWidget *widget = new QVTKWidget (parent);
    QVBoxLayout *l = new QVBoxLayout(parent);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(widget);


    vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
    vtkActor *actor = vtkActor::New();
    vtkRenderer *renderer = vtkRenderer::New();
    vtkRenderWindow *window = vtkRenderWindow::New();
    widget->SetRenderWindow(window);

    parent->setFixedSize(128,128);
    parent->show();

    mapper->SetInput (d->data->GetFibers());
    actor->SetMapper (mapper);
    renderer->AddViewProp(actor);
    window->SetSize (128,128);
    window->AddRenderer (renderer);
    //window->OffScreenRenderingOn();
    renderer->ResetCamera();

    //window->Render();
    parent->update();

    // make sure widget is shown and that openGL context is initialized
    // otherwise, an infinite loop happens when calling GetRGBACharPixelData
    qApp->processEvents();

    unsigned int w=128, h=128;
    d->thumbnail = QImage(w, h, QImage::Format_ARGB32);

    vtkUnsignedCharArray* pixels = vtkUnsignedCharArray::New();
    pixels->SetArray(d->thumbnail.bits(), w*h*4, 1);
    window->GetRGBACharPixelData(0, 0, w-1, h-1, 1, pixels);

    parent->hide();

    d->thumbnails.append (d->thumbnail);

    pixels->Delete();
    mapper->Delete();
    actor->Delete();
    renderer->Delete();
    window->Delete(); // crash if window is deleted
    parent->deleteLater();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractData *createV3dDataFibers()
{
    return new v3dDataFibers;
}

