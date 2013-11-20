#pragma once

#include <itkImage.h>
#include <vtkImageView2D.h>

#include <medAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

class itkThumbnailGenerator : public QObject
{
    Q_OBJECT
    
public:
    itkThumbnailGenerator(medAbstractData *data, unsigned int dimension);
    
    QImage &thumbnail();
    
private slots:
    void generateThumbnail();
    
private:
    template <class T> void SetViewInput(itk::ImageBase <3> *img, vtkImageView2D *view)
    {
        typedef itk::Image<T,3> ImageType;
        
        typename ImageType::Pointer castPtr = dynamic_cast <ImageType *> (img);
        
        if (!castPtr)
            return;
        
        view->SetITKInput(castPtr);
    }
    
    template <class T> void SetViewInput4(itk::ImageBase <4> *img, vtkImageView2D *view)
    {
        typedef itk::Image<T,4> ImageType;
        
        typename ImageType::Pointer castPtr = dynamic_cast <ImageType *> (img);
        
        if (!castPtr)
            return;
        
        view->SetITKInput4(castPtr);
        view->SetTimeIndex(0);
    }
    
    QImage m_Thumbnail;
    dtkSmartPointer <medAbstractData> m_InputData;
    unsigned int m_ImageDimension;
    unsigned int m_XYDim;
};
