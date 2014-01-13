#include <itkThumbnailGenerator.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkUnsignedCharArray.h>
#include <QVTKWidget.h>

#include <medDatabaseThumbnailHelper.h>

itkThumbnailGenerator::itkThumbnailGenerator(medAbstractData *data, unsigned int dimension)
{
    m_XDim = medDatabaseThumbnailHelper::width;
    m_YDim = medDatabaseThumbnailHelper::height;
    m_Thumbnail = QImage(m_XDim,m_YDim,QImage::Format_RGB32);
    m_Thumbnail.fill(0);
    m_InputData = 0;
    
    if (data->identifier().contains("itkDataImage"))
        m_InputData = data;
    
    m_ImageDimension = dimension;
    
    this->moveToThread(QApplication::instance()->thread());
}

QImage &itkThumbnailGenerator::thumbnail()
{
    if (QThread::currentThread() != QApplication::instance()->thread())
        QMetaObject::invokeMethod(this, "generateThumbnail", Qt::BlockingQueuedConnection);
    else
        generateThumbnail();

    return m_Thumbnail;
}

void itkThumbnailGenerator::generateThumbnail()
{
    if (!m_InputData)
        return;

    vtkSmartPointer <vtkRenderer> renderer2d = vtkRenderer::New();
    
    vtkSmartPointer <vtkImageView2D> view2d = vtkImageView2D::New();
    view2d->SetRenderer (renderer2d);
    view2d->SetBackground ( 0.0, 0.0, 0.0 );
    view2d->CursorFollowMouseOff();
    view2d->ShowImageAxisOff();
    view2d->ShowScalarBarOff();
    view2d->ShowAnnotationsOff();
    view2d->ShowRulerWidgetOff();
    
    vtkSmartPointer <vtkRenderWindow> renWin = vtkRenderWindow::New();
    renWin->SetOffScreenRendering(1);
    view2d->SetRenderWindow (renWin);
    
    if (m_ImageDimension == 3)
    {
        typedef itk::ImageBase<3> ImageType;
        
        ImageType::Pointer imgPtr = dynamic_cast <ImageType *> ((itk::Object *)m_InputData->data());
        
        if (!(this->SetViewInput<double> (imgPtr,view2d) ||
              this->SetViewInput<float> (imgPtr,view2d) ||
              this->SetViewInput<int> (imgPtr,view2d) ||
              this->SetViewInput<unsigned int> (imgPtr,view2d) ||
              this->SetViewInput<short> (imgPtr,view2d) ||
              this->SetViewInput<unsigned short> (imgPtr,view2d) ||
              this->SetViewInput<long> (imgPtr,view2d) ||
              this->SetViewInput<unsigned long> (imgPtr,view2d) ||
              this->SetViewInput<char> (imgPtr,view2d) ||
              this->SetViewInput<unsigned char> (imgPtr,view2d) ||
              this->SetViewInput<itk::RGBPixel<unsigned char> > (imgPtr,view2d) ||
              this->SetViewInput<itk::RGBAPixel<unsigned char> > (imgPtr,view2d)))
        {
            return;
        }
    }
    else
    {
        typedef itk::ImageBase<4> ImageType;
        
        ImageType::Pointer imgPtr = dynamic_cast <ImageType *> ((itk::Object *)m_InputData->data());
        
        if (!(this->SetViewInput4<double> (imgPtr,view2d) ||
              this->SetViewInput4<float> (imgPtr,view2d) ||
              this->SetViewInput4<int> (imgPtr,view2d) ||
              this->SetViewInput4<unsigned int> (imgPtr,view2d) ||
              this->SetViewInput4<short> (imgPtr,view2d) ||
              this->SetViewInput4<unsigned short> (imgPtr,view2d) ||
              this->SetViewInput4<long> (imgPtr,view2d) ||
              this->SetViewInput4<unsigned long> (imgPtr,view2d) ||
              this->SetViewInput4<char> (imgPtr,view2d) ||
              this->SetViewInput4<unsigned char> (imgPtr,view2d)))
        {
            return;
        }
    }
    
    
    QVTKWidget *vtkWidget = new QVTKWidget;
    vtkWidget->resize(m_XDim,m_YDim);
    vtkWidget->SetRenderWindow(renWin);

    view2d->Reset();
    view2d->Render();
    
    vtkSmartPointer <vtkUnsignedCharArray> pixels = vtkUnsignedCharArray::New();
    pixels->SetArray(m_Thumbnail.bits(), m_XDim*m_YDim*4, 1);
    renWin->GetRGBACharPixelData(0, 0, m_XDim-1, m_YDim-1, 1, pixels);
    
    m_Thumbnail = m_Thumbnail.mirrored(false,true);
    
    delete vtkWidget;
}
