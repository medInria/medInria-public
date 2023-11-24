#ifndef __itkLocalCriteriaOptimizer_txx
#define __itkLocalCriteriaOptimizer_txx 


#include "itkLocalCriteriaOptimizer.h"
#include "itkVectorNeighborhoodOperatorImageFilter.h"
#include "itkGaussianOperator.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkGradientImageFilter.h"
#include <itkStatisticsImageFilter.h>
#include "itkImageDuplicator.h"
#include "itkMultiplyImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkDerivativeImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkScalarImageToHistogramGenerator.h"

namespace itk
{



template<class TImageType1,class TImageType2>
itk::SmartPointer<TImageType1> Operation(itk::SmartPointer<TImageType1> Image1, itk::SmartPointer<TImageType2> Image2, int  operation)
{
  typedef typename itk::ImageRegionIterator <TImageType1> IteratorType;

    
  typename TImageType1::IndexType IndexV;

  typename TImageType1::Pointer output = TImageType1::New();
  output->SetRegions(Image1->GetBufferedRegion());
  output->SetOrigin(Image1->GetOrigin());
  output->SetSpacing(Image1->GetSpacing());
  output->SetDirection(Image1->GetDirection());
  output->Allocate();

  IteratorType It = IteratorType( output, Image1->GetBufferedRegion());

  for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
      IndexV=It.GetIndex();	
      if (operation==0)
	It.Set(Image1->GetPixel(IndexV)+Image2->GetPixel(IndexV));
      if (operation==1)
        It.Set(Image1->GetPixel(IndexV)-Image2->GetPixel(IndexV));
      if (operation==2)
        It.Set(Image1->GetPixel(IndexV)*Image2->GetPixel(IndexV));
   	
    }

  return(output);
}

template<class TImageType1,class TImageType2>
itk::SmartPointer<TImageType1> OperationV(itk::SmartPointer<TImageType1> Image1, itk::SmartPointer<TImageType2> Image2, int  operation,int dim)
{
  typedef typename itk::ImageRegionIterator <TImageType1> IteratorType;

    
  typename TImageType1::IndexType IndexV;

  typename TImageType1::Pointer output = TImageType1::New();
  output->SetRegions(Image1->GetBufferedRegion());
  output->SetOrigin(Image1->GetOrigin());
  output->SetSpacing(Image1->GetSpacing());
  output->SetDirection(Image1->GetDirection());
  output->Allocate();

  IteratorType It = IteratorType( output, Image1->GetBufferedRegion());

  typename TImageType1::PixelType vector;
  for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
      IndexV=It.GetIndex();	
      if (operation==0)
        {
	for (int i=0;i<dim;++i)
	  vector[i]=Image1->GetPixel(IndexV)[i]+Image2->GetPixel(IndexV)[i];
        It.Set(vector);
        }
      if (operation==1)
        {
	for (int i=0;i<dim;++i)
	  vector[i]=Image1->GetPixel(IndexV)[i]-Image2->GetPixel(IndexV)[i];
        It.Set(vector);
        }      
       if (operation==2)
        {
	for (int i=0;i<dim;++i)
	  vector[i]=Image1->GetPixel(IndexV)[i]*Image2->GetPixel(IndexV)[i];
        It.Set(vector);
        }    
      }

  return(output);
}

template<class TImageType1,class TImageType2>
itk::SmartPointer<TImageType1> DivisionV(itk::SmartPointer<TImageType1> Image1, itk::SmartPointer<TImageType2> Image2, int dim)
{
  typedef typename itk::ImageRegionIterator <TImageType1> IteratorType;

    
  typename TImageType1::IndexType IndexV;

  typename TImageType1::Pointer output = TImageType1::New();
  output->SetRegions(Image1->GetBufferedRegion());
  output->SetOrigin(Image1->GetOrigin());
  output->SetSpacing(Image1->GetSpacing());
  output->SetDirection(Image1->GetDirection());
  output->Allocate();

  IteratorType It = IteratorType( output, Image1->GetBufferedRegion());
  
  typename TImageType1::PixelType vector;
  for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
      IndexV=It.GetIndex();	
      for (int i=0;i<dim;++i)
        vector[i]=Image1->GetPixel(IndexV)[i]/Image2->GetPixel(IndexV);
        It.Set(vector);
          
    }

  return(output);
}


template<class TImageType1,class TImageType2>
itk::SmartPointer<TImageType1> Division(itk::SmartPointer<TImageType1> Image1, itk::SmartPointer<TImageType2> Image2)
{
  typedef typename itk::ImageRegionIterator <TImageType1> IteratorType;

    
  typename TImageType1::IndexType IndexV;

  typename TImageType1::Pointer output = TImageType1::New();
  output->SetRegions(Image1->GetBufferedRegion());
  output->SetOrigin(Image1->GetOrigin());
  output->SetSpacing(Image1->GetSpacing());
  output->SetDirection(Image1->GetDirection());
  output->Allocate();

  IteratorType It = IteratorType( output, Image1->GetBufferedRegion());
  

  for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
      IndexV=It.GetIndex();	
      It.Set(Image1->GetPixel(IndexV)/Image2->GetPixel(IndexV)  );
    }

  return(output);
}



template<class TImageType1>
itk::SmartPointer<TImageType1> Sqrt(itk::SmartPointer<TImageType1> Image1)
{
  typedef typename itk::ImageRegionIterator <TImageType1> IteratorType;

    
  typename TImageType1::IndexType IndexV;

  typename TImageType1::Pointer output = TImageType1::New();
  output->SetRegions(Image1->GetBufferedRegion());
  output->SetOrigin(Image1->GetOrigin());
  output->SetSpacing(Image1->GetSpacing());
  output->SetDirection(Image1->GetDirection());
  output->Allocate();

  IteratorType It = IteratorType( output, Image1->GetBufferedRegion());

  for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
      IndexV=It.GetIndex();
      if (Image1->GetPixel(IndexV)>0)	
          It.Set(sqrt(Image1->GetPixel(IndexV)));
      else
          It.Set(0);
    }

  return(output);
}


template<class TImageType>
itk::SmartPointer<TImageType> SmoothGivenField(itk::SmartPointer<TImageType> InputImage,double Sigma[3])
{
typedef itk::ImageDuplicator< TImageType > DuplicatorType;
 typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
 
 duplicator->SetInputImage(InputImage);
 duplicator->Update();


 typedef  itk::RecursiveGaussianImageFilter <TImageType,TImageType> RGFType;
 typename RGFType::Pointer DGF0=RGFType::New();
 typename RGFType::Pointer DGF1=RGFType::New();
 typename RGFType::Pointer DGF2=RGFType::New();

 DGF0->SetZeroOrder();
 DGF1->SetZeroOrder();
 DGF2->SetZeroOrder();

  DGF0->SetSigma( Sigma[0] );
  DGF1->SetSigma( Sigma[1] );
  DGF2->SetSigma( Sigma[2] );

 DGF0->SetDirection(0);
 DGF1->SetDirection(1);
 DGF2->SetDirection(2);

  DGF0->SetInput(duplicator->GetOutput());
  DGF0->Update();
  DGF1->SetInput(DGF0->GetOutput());
  DGF1->Update();
  DGF2->SetInput(DGF1->GetOutput());
  DGF2->Update();

  return(DGF2->GetOutput());
}

template<class TImageType1>
itk::SmartPointer<TImageType1> BoundarySmoothing(itk::SmartPointer<TImageType1> Image, bool BoundaryCheck)
{
  if (BoundaryCheck)
    {
     typedef typename itk::RescaleIntensityImageFilter< TImageType1,TImageType1 > ScalerFilterType;
     typename ScalerFilterType::Pointer scaler = ScalerFilterType::New();
     scaler->SetOutputMinimum(0 );
     scaler->SetOutputMaximum( 1 );
     scaler->SetInput(Image);
     scaler->Update();

     typedef typename itk::ImageRegionIterator <TImageType1> IteratorType;

    
     typename TImageType1::IndexType IndexV;

     typename TImageType1::Pointer output = TImageType1::New();
     output->SetRegions(Image->GetBufferedRegion());
     output->SetOrigin(Image->GetOrigin());
     output->SetSpacing(Image->GetSpacing());
     output->SetDirection(Image->GetDirection());
     output->Allocate();

     IteratorType It = IteratorType( output, Image->GetBufferedRegion());
  
     double min=10e10,a,m;
     for( It.GoToBegin(); !It.IsAtEnd(); ++It )
       {
         IndexV=It.GetIndex();
         a=Image->GetPixel(IndexV);
         m=scaler->GetOutput()->GetPixel(IndexV);
         if (m>0.07)
           {
            It.Set(a);
            if (min>a) min=a;
           }
         else
           It.Set(0);
       }

     for( It.GoToBegin(); !It.IsAtEnd(); ++It )
       {
         IndexV=It.GetIndex();
         if (It.Get()==0)
         It.Set(min);
       }
    return(output);
   }
  else return(Image);
/* typedef typename itk::Statistics::ScalarImageToHistogramGenerator<
                                 TImageType1 > HistogramGeneratorType;

 typename  HistogramGeneratorType::Pointer histogramGenerator =
                                        HistogramGeneratorType::New(); 

  histogramGenerator->SetInput( Image );

  histogramGenerator->SetNumberOfBins( 256 );
  histogramGenerator->SetMarginalScale( 10.0 );

  histogramGenerator->SetHistogramMin( -0.5 );
  histogramGenerator->SetHistogramMax( 255.5 ); 

  histogramGenerator->Compute();

 typedef typename HistogramGeneratorType::HistogramType HistogramType;

 const HistogramType * histogram = histogramGenerator->GetOutput();
 
 
 typedef typename itk::ImageRegionIterator <TImageType1> IteratorType;

    
 typename TImageType1::IndexType IndexV;

 typename TImageType1::Pointer output = TImageType1::New();
 output->SetRegions(Image->GetBufferedRegion());
 output->SetOrigin(Image->GetOrigin());
 output->SetSpacing(Image->GetSpacing());
 output->SetDirection(Image->GetDirection());
 output->Allocate();

 IteratorType It = IteratorType( output, Image->GetBufferedRegion());
  
 double min=10e10,a,m;

 for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
      IndexV=It.GetIndex();	
      a=Image->GetPixel(IndexV);
      if (a>histogram->Quantile(0,0.6))
        {
         It.Set(a);
         if (min>a) min=a;
        }
      else 
        It.Set(0);	
    }

  for( It.GoToBegin(); !It.IsAtEnd(); ++It )
    {
      IndexV=It.GetIndex();
      if (It.Get()==0)
	It.Set(min);  
    }
  return(output);
*/

}


template<class TFixedImage,class TMovingImage, class TDeformationField> 
LocalCriteriaOptimizer<TFixedImage,TMovingImage,TDeformationField>
::LocalCriteriaOptimizer()
{
for (int i=0;i<FixedImageDimension;++i)   m_Sigma[i]=10;

typename DefaultInterpolatorType::Pointer interp = DefaultInterpolatorType::New();
m_MovingImageInterpolator = static_cast<InterpolatorType*>(interp.GetPointer() );

m_MovingImageWarper = WarperType::New();
m_MovingImageWarper->SetInterpolator( m_MovingImageInterpolator );
m_MovingImageWarper->SetEdgePaddingValue( NumericTraits<MovingImagePixelType>::max() );

m_FixedImageWarper = WarperType::New();
m_FixedImageWarper->SetInterpolator( m_MovingImageInterpolator );
m_FixedImageWarper->SetEdgePaddingValue( NumericTraits<MovingImagePixelType>::max() );


  this->SetMovingImage(NULL);
  this->SetFixedImage(NULL);
  m_FixedImageSpacing.Fill( 1.0 );
  m_FixedImageOrigin.Fill( 0.0 );
  m_FixedImageDirection.SetIdentity();

  m_MovingImageSpacing.Fill( 1.0 );
  m_MovingImageOrigin.Fill( 0.0 );
  m_MovingImageDirection.SetIdentity();

  m_Metric = NumericTraits<double>::max();
  m_LCC = 0.0;
  m_NumberOfPixelsProcessed = 0L;
  m_RMSChange = NumericTraits<double>::max();
  m_SumOfSquaredChange = 0.0;

  m_TimeStep=1.0;

  m_UseMask = false;

  m_BoundaryCheck  = true;
}



template<class TFixedImage,class TMovingImage, class TDeformationField> 
typename LocalCriteriaOptimizer<TFixedImage,TMovingImage,TDeformationField>
::VectorType
LocalCriteriaOptimizer<TFixedImage,TMovingImage,TDeformationField>
::ComputeUpdate(const NeighborhoodType &it, void * gd,
                const FloatOffsetType& itkNotUsed(offset) )
{

  GlobalDataStruct *globalData = (GlobalDataStruct *)gd;

  VectorType update;



IndexType indexV=it.GetIndex();


for ( int i = 0; i < FixedImageDimension; ++i)
    update[i]=1/(pow(m_SmoothedSimGrad->GetPixel(indexV).GetNorm(),2)+
    (m_SigmaI)/ (pow(m_SimilarityImage->GetPixel(indexV),2)) )*m_SmoothedSimGrad->GetPixel(indexV)[i];


if ( globalData )
 {
      globalData->m_LCC +=pow(m_SimilarityImage->GetPixel(indexV),2)  ;
      globalData->m_NumberOfPixelsProcessed += 1;
      globalData->m_SumOfSquaredChange += update.GetSquaredNorm();
    }
return(update);

}


template<class TFixedImage,class TMovingImage, class TDeformationField>
void
LocalCriteriaOptimizer<TFixedImage,TMovingImage,TDeformationField>
::SetBoundaryCheck(bool flag)
{
   this->m_BoundaryCheck=flag;
}


template<class TFixedImage,class TMovingImage, class TDeformationField>
void
LocalCriteriaOptimizer<TFixedImage,TMovingImage,TDeformationField>
::InitializeIteration(void)
{


  if( !this->GetMovingImage() || !this->GetFixedImage()
      || !m_MovingImageInterpolator )
    {
    itkExceptionMacro(
       << "MovingImage, FixedImage and/or Interpolator not set" );
    }

  m_Metric = NumericTraits<double>::max();
  m_LCC = 0.0;
  m_NumberOfPixelsProcessed = 0L;
  m_RMSChange = NumericTraits<double>::max();
  m_SumOfSquaredChange = 0.0;

    // cache fixed image information
  m_FixedImageOrigin  = this->GetFixedImage()->GetOrigin();
  m_FixedImageSpacing = this->GetFixedImage()->GetSpacing();
  m_FixedImageDirection = this->GetFixedImage()->GetDirection();

  m_MovingImageOrigin  = this->GetMovingImage()->GetOrigin();
  m_MovingImageSpacing = this->GetMovingImage()->GetSpacing();
  m_MovingImageDirection = this->GetMovingImage()->GetDirection();
 
  // Compute warped moving image
  m_MovingImageWarper->SetOutputOrigin( this->m_FixedImageOrigin );
  m_MovingImageWarper->SetOutputSpacing( this->m_FixedImageSpacing );
  m_MovingImageWarper->SetOutputDirection( this->m_FixedImageDirection );
  m_MovingImageWarper->SetInput( this->GetMovingImage() );
  m_MovingImageWarper->SetEdgePaddingValue( 0 );
  m_MovingImageWarper->SetDisplacementField( this->GetDisplacementField() );
  m_MovingImageWarper->GetOutput()->SetRequestedRegion( this->GetDisplacementField()->GetRequestedRegion() );
  m_MovingImageWarper->Update();
 
  m_FixedImageWarper->SetOutputOrigin( this->m_FixedImageOrigin );
  m_FixedImageWarper->SetOutputSpacing( this->m_FixedImageSpacing );
  m_FixedImageWarper->SetOutputDirection( this->m_FixedImageDirection );
  m_FixedImageWarper->SetInput( this->GetFixedImage() );
  m_FixedImageWarper->SetEdgePaddingValue( 0 );
  m_FixedImageWarper->SetDisplacementField( this->GetInverseDeformationField() );
  m_FixedImageWarper->GetOutput()->SetRequestedRegion( this->GetInverseDeformationField()->GetRequestedRegion() );
  m_FixedImageWarper->Update();

  if (m_UseMask==true)
   {
      m_MaskImageWarper = WarperType::New();
      m_MaskImageWarper->SetInterpolator( m_MovingImageInterpolator );
      m_MaskImageWarper->SetOutputOrigin( this->m_MovingImageOrigin );
      m_MaskImageWarper->SetOutputSpacing( this->m_MovingImageSpacing );
      m_MaskImageWarper->SetOutputDirection( this->m_MovingImageDirection );
      m_MaskImageWarper->SetInput( m_MaskImage );
      m_MaskImageWarper->SetEdgePaddingValue( 0 );
      m_MaskImageWarper->SetDisplacementField( this->GetDisplacementField() );
      m_MaskImageWarper->GetOutput()->SetRequestedRegion( this->GetDisplacementField()->GetRequestedRegion() );
      m_MaskImageWarper->Update();

   }

  // setup moving image interpolator for further access
  m_MovingImageInterpolator->SetInputImage( this->GetMovingImage() );
  

  this->EvaluateHighOrderTerms();
 }


template<class TFixedImage,class TMovingImage, class TDeformationField>
void
LocalCriteriaOptimizer<TFixedImage,TMovingImage,TDeformationField>
::PrintSelf(std::ostream& os, Indent indent) const 
{
SuperClass::PrintSelf(os,indent);

os<< indent << "Printself" << std::endl;

}

template <class TFixedImage, class TMovingImage, class TDeformationField>
void
LocalCriteriaOptimizer<TFixedImage,TMovingImage,TDeformationField>
::EvaluateHighOrderTerms(void)
{ 
  FixedImagePointer FixImage =   m_FixedImageWarper->GetOutput();

  MovingImagePointer MovImage = m_MovingImageWarper->GetOutput();

  FixedImagePointer  Fix2= Operation<TFixedImage,TFixedImage>(FixImage,FixImage,2);
  MovingImagePointer  Mov2= Operation<TMovingImage,TMovingImage>(MovImage,MovImage,2);

  FixedImagePointer  GFix2=BoundarySmoothing<TFixedImage>(SmoothGivenField <TFixedImage> (Fix2,m_Sigma),this->m_BoundaryCheck);
  MovingImagePointer GMov2=BoundarySmoothing<TMovingImage>(SmoothGivenField <TMovingImage>(Mov2,m_Sigma),this->m_BoundaryCheck);

  FixedImagePointer  SqGFix2=Sqrt<TFixedImage> (GFix2);
  MovingImagePointer SqGMov2=Sqrt<TMovingImage>(GMov2);
  
  FixedImagePointer FixMov=Operation<TFixedImage,TMovingImage> (FixImage,MovImage,2);
  FixedImagePointer GFixMov=BoundarySmoothing<TFixedImage>(SmoothGivenField <TFixedImage> (FixMov,m_Sigma),this->m_BoundaryCheck);
  
  FixedImagePointer Denom=BoundarySmoothing<TFixedImage>(Operation<TFixedImage,TMovingImage> (SqGFix2,SqGMov2,2),this->m_BoundaryCheck);
  
  FixedImagePointer Corr=Division<TFixedImage,TFixedImage>(GFixMov,Denom);
  

  FixedImagePointer  GFix=BoundarySmoothing<TFixedImage>(SmoothGivenField <TFixedImage> (FixImage,m_Sigma),this->m_BoundaryCheck);
  MovingImagePointer GMov=BoundarySmoothing<TMovingImage>(SmoothGivenField <TMovingImage>(MovImage,m_Sigma),this->m_BoundaryCheck);


/**
  *Compute the gradient of the Moving image : D(M o phi)
  **/
typedef  itk::GradientImageFilter<MovingImageType> GradientMType;
typename GradientMType::Pointer GradientM = GradientMType::New();
GradientM->SetInput(MovImage);
GradientM->Update();

typedef  itk::GradientImageFilter<FixedImageType> GradientFType;
typename GradientFType::Pointer GradientF = GradientFType::New();
GradientF->SetInput(FixImage);
GradientF->Update();




  typedef typename itk::ImageRegionIterator <VectorImageType> VectIteratorType;

    
  typename VectorImageType::IndexType IndexV,IndexV1;
  typename VectorImageType::PixelType vector,vector1;

  typename VectorImageType::Pointer GradFix = VectorImageType::New();
  GradFix->SetRegions(FixImage->GetBufferedRegion());
  GradFix->SetOrigin(FixImage->GetOrigin());
  GradFix->SetSpacing(FixImage->GetSpacing());
  GradFix->SetDirection(FixImage->GetDirection());
  GradFix->Allocate();
  
  typename VectorImageType::Pointer GradMov = VectorImageType::New();
  GradMov->SetRegions(MovImage->GetBufferedRegion());
  GradMov->SetOrigin(MovImage->GetOrigin());
  GradMov->SetSpacing(MovImage->GetSpacing());
  GradMov->SetDirection(MovImage->GetDirection());
  GradMov->Allocate();


  VectIteratorType ItF = VectIteratorType( GradFix, GradFix->GetBufferedRegion());
  VectIteratorType ItM = VectIteratorType( GradMov, GradMov->GetBufferedRegion());

 
  for( ItF.GoToBegin(),ItM.GoToBegin(); !ItF.IsAtEnd(); ++ItF,++ItM )
    {
      IndexV=ItF.GetIndex();	
      IndexV1=ItM.GetIndex();
       
      for (int i=0;i<FixedImageDimension;++i)
        {
         vector[i]=GradientF->GetOutput()->GetPixel(IndexV)[i];
         vector1[i]=GradientM->GetOutput()->GetPixel(IndexV1)[i];
        }  
	ItF.Set(vector);
        ItM.Set(vector1);
    }



typename VectorImageType::Pointer MGradFix=Operation<VectorImageType,MovingImageType>(GradFix,MovImage,2);
typename VectorImageType::Pointer FGradMov=Operation<VectorImageType,FixedImageType>(GradMov,FixImage,2);

typename VectorImageType::Pointer Num1=OperationV<VectorImageType,VectorImageType>(FGradMov,MGradFix,1,FixedImageDimension);

typename VectorImageType::Pointer GNum1=SmoothGivenField<VectorImageType> (Num1,m_Sigma);

typename VectorImageType::Pointer Term1=DivisionV<VectorImageType,FixedImageType>(GNum1,Denom,FixedImageDimension);

typename VectorImageType::Pointer FGradFix=Operation<VectorImageType,FixedImageType>(GradFix,FixImage,2);
typename VectorImageType::Pointer MGradMov=Operation<VectorImageType,MovingImageType>(GradMov,MovImage,2);

typename VectorImageType::Pointer GFGradFix=SmoothGivenField<VectorImageType> (FGradFix,m_Sigma);
typename VectorImageType::Pointer GMGradMov=SmoothGivenField<VectorImageType> (MGradMov,m_Sigma);

typename VectorImageType::Pointer Term2a=DivisionV<VectorImageType,FixedImageType>(GFGradFix,GFix2,FixedImageDimension);
typename VectorImageType::Pointer Term2b=DivisionV<VectorImageType,MovingImageType>(GMGradMov,GMov2,FixedImageDimension);

typename VectorImageType::Pointer Term2=OperationV<VectorImageType,VectorImageType>(Term2a,Term2b,1,FixedImageDimension);

typename VectorImageType::Pointer FO=OperationV<VectorImageType,VectorImageType> (Term1,Term2,0,FixedImageDimension); 


 this->SetSimilarityImage(Corr);

/**
  *Compute G(Fix-Mov)*X
  * This is the first order term to be used for the optimization
  **/

  this->SetSimGrad(FO);



}


template <class TFixedImage, class TMovingImage, class TDeformationField>
void
LocalCriteriaOptimizer<TFixedImage,TMovingImage,TDeformationField>
::ReleaseGlobalDataPointer( void *gd ) const
{   
  GlobalDataStruct * globalData = (GlobalDataStruct *) gd;
    
  m_MetricCalculationLock.lock();
  m_LCC += globalData->m_LCC;
  m_NumberOfPixelsProcessed += globalData->m_NumberOfPixelsProcessed;
  m_SumOfSquaredChange += globalData->m_SumOfSquaredChange;
  if( m_NumberOfPixelsProcessed )
    {     
    m_Metric = m_LCC /
               static_cast<double>( m_NumberOfPixelsProcessed );
    m_RMSChange = std::sqrt( m_SumOfSquaredChange /
               static_cast<double>( m_NumberOfPixelsProcessed ) );
    }
  m_MetricCalculationLock.unlock();

  delete globalData;
} 








}//end namespace itk
#endif

