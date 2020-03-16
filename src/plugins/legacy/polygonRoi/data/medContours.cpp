#include "medContours.h"

// medinria
#include <medAbstractDataFactory.h>

// qt
#include <QApplication>

// vtk
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class medContoursPrivate
{
public:
    vtkSmartPointer<vtkMetaDataSet> mesh;
    QVector<medTagContours> tagContoursSet;

    QList<QImage> thumbnails;
};

medContours::medContours()
    : medAbstractMeshData(),
      d (new medContoursPrivate())
{
    this->moveToThread(QApplication::instance()->thread());
    d->mesh = nullptr;
}

medContours::medContours(const medContours &other)
    : medAbstractMeshData(other),
      d (new medContoursPrivate)
{
    this->moveToThread(QApplication::instance()->thread());
    d->thumbnails = other.d->thumbnails;

    d->mesh = vtkSmartPointer<vtkMetaSurfaceMesh>::New();
    d->mesh->SetDataSet(vtkPolyData::New());

    d->mesh->GetDataSet()->DeepCopy(other.d->mesh->GetDataSet());

    d->tagContoursSet = other.d->tagContoursSet;
}

medContours::~medContours()
{
}

bool medContours::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<medContours>();
}

QVector<medTagContours> &medContours::getTagContoursSet()
{
    return d->tagContoursSet;
}

void medContours::setData(void* data)
{
    vtkMetaDataSet* mesh = vtkMetaDataSet::SafeDownCast((vtkObject*)data);
    if (!mesh)
    {
        qDebug()<< metaObject()->className() << ":: cannot cast data to correct data type";
        return;
    }

    if (mesh->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH)
    {
        qDebug()<< metaObject()->className() << ":: cannot cast data to correct data type "<<mesh->GetType();
        return;
    }

    d->mesh = mesh;

}

void medContours::setData(void* data, int channel)
{
    switch (channel)
    {
        case 0:
        {
            setData(data);
            break;
        }
        case 1:
        {
            auto ptr = reinterpret_cast<QVector<medTagContours>*>(data);
            d->tagContoursSet = *ptr;
            break;
        }
        default:
        {
            qDebug()<<metaObject()->className()<<" :: setData - unknown channel.";
        }
    }
}

void* medContours::data()
{
    return d->mesh.GetPointer();
}

void* medContours::data(int channel)
{
    switch (channel)
    {
        case 0:
        {
            return data();
        }
        case 1:
        {
            return &d->tagContoursSet;
        }
        default:
        {
            qDebug()<<metaObject()->className()<<":: data - unknown channel.";
        }
    }
    // avoid compiler warning
    return d->mesh;
}

void* medContours::output()
{
    return d->mesh.GetPointer();
}

medContours* medContours::clone()
{
    return new medContours(*this);
}
