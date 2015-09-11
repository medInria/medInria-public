/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPaintCommand.h>

#include <medAbstractImageView.h>
#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>

#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>

#include <QtCore>
#include <QVector3D>

#include <itkImage.h>
#include <itkLabelObject.h>
#include <itkLabelMapToLabelImageFilter.h>
#include <itkLabelMapToAttributeImageFilter.h>



class medPaintCommandPrivate
{
public:
    QVector3D lastVup;
    QVector3D lastVpn;
    double sampleSpacing[2];

    LabelObjectType::Pointer label;
    LabelMapType::Pointer labelMap;

    typedef itk::LabelMapToAttributeImageFilter< LabelMapType, MaskType > MapToImageFilter;
    MapToImageFilter::Pointer filter;
};


medPaintCommand::medPaintCommand(medPaintCommandOptions *options, QUndoCommand *parent)
    : medAbstractPaintCommand(options, parent), d(new medPaintCommandPrivate)
{
    d->lastVup = QVector3D();
    d->lastVpn = QVector3D();

    d->label = 0;

    this->setText("Paint");

    d->labelMap = medPaintCommandManager::instance()->labelMap(this->options()->itkMask);
}

medPaintCommand::~medPaintCommand()
{
    delete d;
    d = 0;
}


void medPaintCommand::paint()
{
    d->label = LabelObjectType::New();
    d->label->SetAttribute(this->options()->maskValue);

    foreach(QVector3D newPoint, this->options()->points)
    {
        typedef  MaskType::DirectionType::InternalMatrixType::element_type ElemType;
        itk::Point< ElemType > centerPoint;
        centerPoint.SetElement(0, newPoint.x());
        centerPoint.SetElement(1, newPoint.y());
        centerPoint.SetElement(2, newPoint.z());

        const QVector3D vup = this->options()->view->viewUp();
        const QVector3D vpn = this->options()->view->viewPlaneNormal();

        vnl_vector_fixed<ElemType, 3> vecVup(vup.x(), vup.y(), vup.z() );
        vnl_vector_fixed<ElemType, 3> vecVpn(vpn.x(), vpn.y(), vpn.z() );
        vnl_vector_fixed<ElemType, 3> vecRight = vnl_cross_3d(vecVup,vecVpn);

        if ( vup != d->lastVup || vpn != d->lastVpn ) {
            const MaskType::SpacingType & spacing = this->options()->itkMask->GetSpacing();

            // Rows of the direction matrix are the directions of the image i,j,k pixel directions.
            const MaskType::DirectionType & direction = this->options()->itkMask->GetDirection();

            // project spacing onto view.

            vnl_matrix_fixed<ElemType,2,3> projMatrix;
            projMatrix.set_row(0, vecRight );
            projMatrix.set_row(1, vecVup );

            projMatrix = projMatrix * direction.GetVnlMatrix(); // (direction.GetTranspose());

            double sampleSpacing[2];
            // Compute the total projection of each of the spacings onto the view plane x & y.
            for (int i = 0; i < 2; i++) //output axis
            {
                double s = 0;  // sum squares
                double r = 0;
                for (int j = 0; j < 3; j++)
                {
                    const double elem = projMatrix.get(i,j);
                    const double elem2 = elem*elem;
                    s += elem2*(spacing[j] >= 0 ? spacing[j] : -spacing[j]);
                    r += elem2;
                }
                s /= r;
                sampleSpacing[i] = s;
            }

            // Store result.
            std::copy( sampleSpacing, sampleSpacing + 2, d->sampleSpacing);

            //Oversample
            d->sampleSpacing[0] *= 0.5;
            d->sampleSpacing[1] *= 0.5;
            d->lastVup = vup;
            d->lastVpn = vpn;
        }

        const double radius2 = this->options()->radius*this->options()->radius;

        const int Nx = std::max( 1, (int)std::ceil(this->options()->radius/d->sampleSpacing[0]) );
        const int Ny = std::max( 1, (int)std::ceil(this->options()->radius/d->sampleSpacing[1]) );

        MaskType::PixelType pxValue = this->options()->maskValue;

        MaskType::IndexType index;
        itk::Point<ElemType,3> testPt;

        for ( int y(-Ny); y < Ny; ++y ) {
            double dy = y*d->sampleSpacing[1];
            for ( int x(-Nx); x < Nx; ++x ) {
                double dx = x*d->sampleSpacing[0];
                if ( dx*dx + dy*dy > radius2 )
                    continue;

                for ( int ic(0); ic<3; ++ic) {
                    testPt[ic] = centerPoint[ic] + dx * vecRight[ic] + dy * vecVup[ic];
                }

                bool isInside = this->options()->itkMask->TransformPhysicalPointToIndex( testPt, index );
                if ( isInside )
                {
                    this->options()->itkMask->SetPixel( index, pxValue );
                    d->label->AddIndex(index);
                }
            }
        }
        this->options()->itkMask->Modified();
        this->options()->itkMask->GetPixelContainer()->Modified();
        this->options()->itkMask->SetPipelineMTime(this->options()->itkMask->GetMTime());
    }
}

void medPaintCommand::undo()
{
    d->labelMap->RemoveLabelObject(d->label);

    typedef itk::LabelMapToAttributeImageFilter< LabelMapType, MaskType > MapToImageFilter;
    d->filter = MapToImageFilter::New();

    d->filter->SetInput(d->labelMap);
    d->filter->Update();

    this->options()->itkMask->Graft(d->filter->GetOutput());

    this->options()->view->render();
}

void medPaintCommand::redo()
{
    if(!d->label)
    {
        paint();
        d->labelMap->PushLabelObject(d->label);
    }
    else
    {
        d->labelMap->PushLabelObject(d->label);

        typedef itk::LabelMapToAttributeImageFilter< LabelMapType, MaskType > MapToImageFilter;
        d->filter = MapToImageFilter::New();

        d->filter->SetInput(d->labelMap);
        d->filter->Update();

        this->options()->itkMask->Graft(d->filter->GetOutput());
    }

    this->options()->view->render();
}
