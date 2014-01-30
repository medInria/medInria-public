/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dDataImage.h>

#include <medAbstractDataFactory.h>

//#include <itkAnalyzeImageIOFactory.h>
//#include <itkVTKImageIOFactory.h>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMetaImageData.h>

// /////////////////////////////////////////////////////////////////
// v3dDataImagePrivate
// /////////////////////////////////////////////////////////////////

class v3dDataImagePrivate
{
public:
    vtkSmartPointer<vtkImageData> image;
};

// /////////////////////////////////////////////////////////////////
// v3dDataImage
// /////////////////////////////////////////////////////////////////

v3dDataImage::v3dDataImage() : medAbstractDataImage(), d(new v3dDataImagePrivate)
{
    d->image = NULL;
}

v3dDataImage::~v3dDataImage()
{
    delete d;
    d = 0;
}

bool v3dDataImage::registered()
{
    return medAbstractDataFactory::instance()->registerDataType("v3dDataImage", createV3dDataImage);
}

QString v3dDataImage::description() const
{
    return tr("vtk 3d image data");
}

QString v3dDataImage::identifier() const
{
    return "v3dDataImage";
}

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

bool v3dDataImage::read(const QString& file)
{
    if(dtkAbstractData::read(file))
        return true;

    vtkMetaImageData* metaimage = vtkMetaImageData::New();

    try
    {
        metaimage->Read(file.toAscii().constData());
    }
    catch (vtkErrorCode::ErrorIds &)
    {
        qDebug() << "Error: cannot open file" << file.toAscii().constData();
    }

    d->image = metaimage->GetImageData();

    if(!d->image)
        return false;

    return true;
}

bool v3dDataImage::write(const QString& file)
{
    if(dtkAbstractData::write(file))
        return true;

    return false;
}

void v3dDataImage::setData(void *data)
{
    if(vtkImageData *image = dynamic_cast<vtkImageData*>((static_cast<vtkObject*>(data))))
        d->image = image;
}

void *v3dDataImage::data()
{
    return d->image;
}

void *v3dDataImage::output()
{
    return d->image;
}

int v3dDataImage::xDimension()
{
  if (!d->image)
    return -1;

  int *dimensions = d->image->GetDimensions();
  return dimensions[0];
}

int v3dDataImage::yDimension()
{
  if (!d->image)
    return -1;

  int *dimensions = d->image->GetDimensions();
  return dimensions[1];
}

int v3dDataImage::zDimension()
{
  if (!d->image)
    return -1;

  int *dimensions = d->image->GetDimensions();
  return dimensions[2];
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractData *createV3dDataImage()
{
    return new v3dDataImage;
}

