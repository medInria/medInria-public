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

#include <itkImage.h>

#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>

#include <QtCore>
#include <QVector3D>

class medPaintCommandPrivate
{
public:
    QVector<QVector3D> points;
    medAbstractImageView * view;
    double radius;

    QVector3D lastVup;
    QVector3D lastVpn;

    typedef itk::Image<unsigned char, 3> MaskType;
    MaskType::Pointer itkMask;

    double sampleSpacing[2];

    medPaintCommand::medPaintMode mode;

    unsigned int maskValue;

};

medPaintCommand::medPaintCommand(QVector<QVector3D> points, medAbstractImageView * view, double radius, MaskType::Pointer itkMask, unsigned int maskValue, medPaintMode mode,  QUndoCommand *parent)
    :d(new medPaintCommandPrivate)
{
    d->points = points;
    d->view = view;
    d->radius = radius;
    d->itkMask = itkMask;

    d->lastVup = QVector3D();
    d->lastVpn = QVector3D();

    d->maskValue = maskValue;

    d->mode = mode;

    this->setText("Paint");
}


void medPaintCommand::paint(medPaintMode mode)
{
    //this->updateFromGuiItems();

//    if ( !m_imageData ) {
//        this->setData(view->layerData(view->currentLayer()));
//    }
//    if (!m_imageData) {
//        dtkWarn() << "Could not set data";
//        return;
//    }

    foreach(QVector3D newPoint, d->points)
    {
        typedef  MaskType::DirectionType::InternalMatrixType::element_type ElemType;
        itk::Point< ElemType > centerPoint;
        centerPoint.SetElement(0, newPoint.x());
        centerPoint.SetElement(1, newPoint.y());
        centerPoint.SetElement(2, newPoint.z());

        const QVector3D vup = d->view->viewUp();
        const QVector3D vpn = d->view->viewPlaneNormal();

        vnl_vector_fixed<ElemType, 3> vecVup(vup.x(), vup.y(), vup.z() );
        vnl_vector_fixed<ElemType, 3> vecVpn(vpn.x(), vpn.y(), vpn.z() );
        vnl_vector_fixed<ElemType, 3> vecRight = vnl_cross_3d(vecVup,vecVpn);

        if ( vup != d->lastVup || vpn != d->lastVpn ) {
            const MaskType::SpacingType & spacing = d->itkMask->GetSpacing();

            // Rows of the direction matrix are the directions of the image i,j,k pixel directions.
            const MaskType::DirectionType & direction = d->itkMask->GetDirection();

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

        const double radius2 = d->radius*d->radius;

        const int Nx = std::max( 1, (int)std::ceil(d->radius/d->sampleSpacing[0]) );
        const int Ny = std::max( 1, (int)std::ceil(d->radius/d->sampleSpacing[1]) );

        MaskType::PixelType pxValue;
        switch ( mode )
        {
        case PAINT :
            pxValue = d->maskValue;
            break;
        default:
            //erasing: mask pixel value to 0
            pxValue = 0;
            break;
        }

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

                bool isInside = d->itkMask->TransformPhysicalPointToIndex( testPt, index );
                if ( isInside ) {
                    d->itkMask->SetPixel( index, pxValue );
                }
            }
        }
        d->itkMask->Modified();
        d->itkMask->GetPixelContainer()->Modified();
        d->itkMask->SetPipelineMTime(d->itkMask->GetMTime());
    }
}


void medPaintCommand::undo()
{
    if(d->mode == PAINT)
        paint(ERASE);
    else if(d->mode == ERASE)
        paint(PAINT);

    d->view->render();
}

void medPaintCommand::redo()
{
    paint(d->mode);

    d->view->render();
}


bool medPaintCommand::mergeWith(const QUndoCommand *command)
{
    qDebug() << "mergeWith";

}
