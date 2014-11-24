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
    QVector3D lastVup;
    QVector3D lastVpn;
    double sampleSpacing[2];
};

medPaintCommand::medPaintCommand(medPaintCommandOptions *options, QUndoCommand *parent)
    : medAbstractPaintCommand(options, parent), d(new medPaintCommandPrivate)
{
    d->lastVup = QVector3D();
    d->lastVpn = QVector3D();

    this->setText("Paint");
}


void medPaintCommand::paint(unsigned int maskValue)
{
    //this->updateFromGuiItems();

//    if ( !m_imageData ) {
//        this->setData(view->layerData(view->currentLayer()));
//    }
//    if (!m_imageData) {
//        dtkWarn() << "Could not set data";
//        return;
//    }

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

        MaskType::PixelType pxValue = maskValue;

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
                if ( isInside ) {
                    this->options()->itkMask->SetPixel( index, pxValue );
                }
            }
        }
        this->options()->itkMask->Modified();
        this->options()->itkMask->GetPixelContainer()->Modified();
        this->options()->itkMask->SetPipelineMTime(this->options()->itkMask->GetMTime());
    }
}

void medPaintCommand::paint()
{
    this->paint(this->options()->maskValue);
}

void medPaintCommand::unpaint()
{
    qDebug() << "medPaintCommand::unpaint()";
    this->paint(0);
}
