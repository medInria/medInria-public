/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkTransferFunctionPresets.h"

#include <time.h>
#include <string>

#include <vtkMath.h>
#include <vtkObjectFactory.h>

#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

vtkStandardNewMacro(vtkTransferFunctionPresets);

#include "lut/Spectrum.h"
#include "lut/HotMetal.h"
#include "lut/HotGreen.h"
#include "lut/HotIron.h"
#include "lut/GEColor.h"
#include "lut/Flow.h"
#include "lut/LONI.h"
#include "lut/LONI2.h"
#include "lut/Asymmetry.h"
#include "lut/VRMusclesBones.h"
#include "lut/VRBones.h"
#include "lut/VRRedVessels.h"
#include "lut/Cardiac.h"
#include "lut/GrayRainbow.h"
#include "lut/Stern.h"
#include "lut/BlackBody.h"
#include "lut/Channelness.h"

vtkTransferFunctionPresets::vtkTransferFunctionPresets()
{}


vtkTransferFunctionPresets::~vtkTransferFunctionPresets()
{}


std::vector< std::string > vtkTransferFunctionPresets::GetAvailablePresets()
{
  std::vector< std::string > presetNames;

  presetNames.resize( 32 );	// size >= #names
  std::vector< std::string >::iterator it = presetNames.begin();

  // it's save to just comment lines if you feel like
  * it = "Default";                ++it;
  * it = "Black & White";          ++it;
  * it = "Black & White Inversed"; ++it;
  * it = "Spectrum";               ++it;
  * it = "Hot Metal";              ++it;
  * it = "Hot Green";              ++it;
  * it = "Hot Iron";               ++it;
  * it = "GE";                     ++it;
  * it = "Flow";                   ++it;
  * it = "Loni";                   ++it;
  * it = "Loni Inversed";          ++it;
  * it = "Loni 2";                 ++it;
  * it = "Asymmetry";              ++it;
  * it = "P-Value";                ++it;
  // * it = "Segmentation";           ++it;
  * it = "Red Black Alpha";        ++it;
  * it = "Green Black Alpha";      ++it;
  * it = "Blue Black Alpha";       ++it;
  * it = "Muscles & Bones";        ++it;
  * it = "Bones";                  ++it;
  * it = "Red Vessels";            ++it;
  * it = "Cardiac";                ++it;
  * it = "Gray Rainbow";           ++it;
  * it = "Stern";                  ++it;
  * it = "Black Body";             ++it;
  * it = "Binary Map";             ++it;
  * it = "Channelness";            ++it;

  presetNames.erase( it, presetNames.end() );

  return presetNames;
}

void vtkTransferFunctionPresets::GetTransferFunction(
  const std::string & name,
  vtkColorTransferFunction * rgb,
  vtkPiecewiseFunction * alpha )
{
  if ( rgb == NULL )
    rgb = vtkColorTransferFunction::New();
  else
    rgb->RemoveAllPoints();

  if ( alpha == NULL )
    alpha = vtkPiecewiseFunction::New();
  else
    alpha->RemoveAllPoints();

  // rgb->ClampingOff();
  // alpha->ClampingOff();

  typedef vtkTransferFunctionPresets Self;
  if ( name == "Default" )
    Self::GetBWTransferFunction( rgb, alpha );
  else if ( name == "Black & White" )
    Self::GetBWTransferFunction( rgb, alpha );
  else if ( name == "Black & White Inversed" )
    Self::GetBWInverseTransferFunction( rgb, alpha );
  else if ( name == "Spectrum" )
    Self::GetSpectrumTransferFunction( rgb, alpha );
  else if ( name == "Hot Metal" )
    Self::GetHotMetalTransferFunction( rgb, alpha );
  else if ( name == "Hot Green" )
    Self::GetHotGreenTransferFunction( rgb, alpha );
  else if ( name == "Hot Iron" )
    Self::GetHotIronTransferFunction( rgb, alpha );
  else if ( name == "GE" )
    Self::GetGEColorTransferFunction( rgb, alpha );
  else if ( name == "Flow" )
    Self::GetFlowTransferFunction( rgb, alpha );
  else if ( name == "Loni" )
    Self::GetLONITransferFunction( rgb, alpha );
  else if ( name == "Loni Inversed" )
    Self::GetLONIInversedTransferFunction( rgb, alpha );
  else if ( name == "Loni 2" )
    Self::GetLONI2TransferFunction( rgb, alpha );
  else if ( name == "Asymmetry" )
    Self::GetAsymmetryTransferFunction( rgb, alpha );
  else if ( name == "P-Value" )
    Self::GetPValueTransferFunction( rgb, alpha );
  else if ( name == "Red Black Alpha" )
    Self::GetRedBlackAlphaTransferFunction( rgb, alpha );
  else if ( name == "Green Black Alpha" )
    Self::GetGreenBlackAlphaTransferFunction( rgb, alpha );
  else if ( name == "Blue Black Alpha" )
    Self::GetBlueBlackAlphaTransferFunction( rgb, alpha );
  // else if ( name == "Segmentation" )
  //   Self::GetROITransferFunction( rgb, alpha );
  else if ( name == "Muscles & Bones" )
    Self::GetVRMusclesBonesTransferFunction( rgb, alpha );
  else if ( name == "Bones" )
    Self::GetVRBonesTransferFunction( rgb, alpha );
  else if ( name == "Red Vessels" )
    Self::GetVRRedVesselsTransferFunction( rgb, alpha );
  else if ( name == "Cardiac" )
    Self::GetCardiacTransferFunction( rgb, alpha );
  else if ( name == "Gray Rainbow" )
    Self::GetGrayRainbowTransferFunction( rgb, alpha );
  else if ( name == "Stern" )
    Self::GetSternTransferFunction( rgb, alpha );
  else if ( name == "Black Body" )
    Self::GetBlackBodyTransferFunction( rgb, alpha );
  else if (name == "Binary Map")
    Self::GetBinaryMapTransferFunction( rgb, alpha );
  else if (name == "Channelness")
    Self::GetChannelnessTransferFunction( rgb, alpha );
  else
    Self::GetBWTransferFunction( rgb, alpha );
}


void vtkTransferFunctionPresets::GetTransferFunctionFromTable(
  unsigned int size,
  const int * table,
  bool logAlpha,
  vtkColorTransferFunction * rgb,
  vtkPiecewiseFunction * alpha )
{
  double * rgbTable   = new double[3 * size];
  double * alphaTable = new double[size];

  double normFactor   = static_cast< double >( size - 1 );

  for ( unsigned int i = 0; i < size; ++i )
  {
    for ( unsigned int j = 0; j < 3; ++j )
      rgbTable[3 * i + j] =
	static_cast< double >( table[j * size + i] ) / 255.0;

    alphaTable[i] = static_cast< double >( i ) / normFactor;
    if ( logAlpha )
      alphaTable[i] = log( 1.0 + 9.0 * alphaTable[i] ) / log( 10.0 );
  }

  rgb->BuildFunctionFromTable( 0.0, 1.0, size, rgbTable );
  delete [] rgbTable;

  alpha->BuildFunctionFromTable( 0.0, 1.0, size, alphaTable );
  delete [] alphaTable;
}

void vtkTransferFunctionPresets::GetTransferFunctionFromTable(
  unsigned int size,
  const float table[][3],
  bool logAlpha,
  vtkColorTransferFunction * rgb,
  vtkPiecewiseFunction * alpha )
{
  double * rgbTable   = new double[3 * size];
  double * alphaTable = new double[size];

  double normFactor   = static_cast< double >( size - 1 );

  for ( unsigned int i = 0; i < size; ++i )
  {
    for ( unsigned int j = 0; j < 3; ++j )
      rgbTable[3 * i + j] = static_cast< double >( table[i][j] );

    alphaTable[i] = static_cast< double >( i ) / normFactor;
    if ( logAlpha )
      alphaTable[i] = log( 1.0 + 9.0 * alphaTable[i] ) / log( 10.0 );
  }

  rgb->BuildFunctionFromTable( 0.0, 1.0, size, rgbTable );
  delete [] rgbTable;

  alpha->BuildFunctionFromTable( 0.0, 1.0, size, alphaTable );
  delete [] alphaTable;
}

void vtkTransferFunctionPresets::GetTransferFunctionFromInversedTable(
  unsigned int size,
  const float table[][3],
  bool logAlpha,
  vtkColorTransferFunction * rgb,
  vtkPiecewiseFunction * alpha )
{
  double * rgbTable   = new double[3 * size];
  double * alphaTable = new double[size];

  double normFactor   = static_cast< double >( size - 1 );

  for ( unsigned int i = 0; i < size; ++i )
  {
    for ( unsigned int j = 0; j < 3; ++j )
      rgbTable[3 * i + j] = static_cast< double >( table[size-i-1][j] );

    alphaTable[i] = static_cast< double >( i ) / normFactor;
    if ( logAlpha )
      alphaTable[i] = log( 1.0 + 9.0 * alphaTable[i] ) / log( 10.0 );
  }

  rgb->BuildFunctionFromTable( 0.0, 1.0, size, rgbTable );
  delete [] rgbTable;

  alpha->BuildFunctionFromTable( 0.0, 1.0, size, alphaTable );
  delete [] alphaTable;
}


void vtkTransferFunctionPresets::GetBWTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
  rgb->AddRGBPoint( 1.0, 1.0, 1.0, 1.0 );

  alpha->AddPoint( 0.0, 0.0 );
  alpha->AddPoint( 1.0, 1.0 );
}


void vtkTransferFunctionPresets::GetBWInverseTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->AddRGBPoint( 0.0, 1.0, 1.0, 1.0 );
  rgb->AddRGBPoint( 1.0, 0.0, 0.0, 0.0 );

  alpha->AddPoint( 0.0, 1.0 );
  alpha->AddPoint( 1.0, 0.0 );
}


void vtkTransferFunctionPresets::GetSpectrumTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->AddRGBPoint( 0.0, 1.0, 0.0, 1.0 );
  rgb->AddRGBPoint( 0.2, 0.0, 0.0, 1.0 );
  rgb->AddRGBPoint( 0.4, 0.0, 1.0, 1.0 );
  rgb->AddRGBPoint( 0.6, 0.0, 1.0, 0.0 );
  rgb->AddRGBPoint( 0.8, 1.0, 1.0, 0.0 );
  rgb->AddRGBPoint( 1.0, 1.0, 0.0, 0.0 );

  alpha->AddPoint( 0.0, 1.0 );
  alpha->AddPoint( 0.2, 1.0 );
  alpha->AddPoint( 0.4, 1.0 );
  alpha->AddPoint( 0.6, 1.0 );
  alpha->AddPoint( 0.8, 1.0 );
  alpha->AddPoint( 1.0, 1.0 );
}


void vtkTransferFunctionPresets::GetHotMetalTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, HotMetal, false, rgb, alpha );

  // double * table = new double[3 * 256];
  // for ( unsigned int i = 0; i < 256; ++i )
  //   for ( unsigned int j = 0; j < 3; ++j )
  //     table[3 * i + j] =
  // 	static_cast< double >( HotMetal[j * 256 + i] ) / 255.0;

  // // lut->SetTableValue(i, (double)HotMetal[i]/255.0,
  // // 		     (double)HotMetal[256+i]/255.0,
  // // 		     (double)HotMetal[256*2+i]/255.0, (double)i/255.0 );
  // rgb->BuildFunctionFromTable( 0.0, 1.0, 256, table );
  // delete [] table;

  // alpha->AddPoint( 0.0, 0.0 );
  // alpha->AddPoint( 1.0, 1.0 );
}


void vtkTransferFunctionPresets::GetHotGreenTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, HotGreen, true, rgb, alpha );
}


void vtkTransferFunctionPresets::GetHotIronTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, HotIron, true, rgb, alpha );
}


void vtkTransferFunctionPresets::GetGEColorTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->AddRGBPoint( 0.0,  0.0, 0.0, 0.0 );
  rgb->AddRGBPoint( 0.25, 0.0, 0.5, 0.5 );
  rgb->AddRGBPoint( 0.5,  0.5, 0.0, 1.0 );
  rgb->AddRGBPoint( 0.75, 1.0, 0.5, 0.0 );
  rgb->AddRGBPoint( 1.0,  1.0, 1.0, 1.0 );

  alpha->AddPoint( 0.0,  0.0  );
  alpha->AddPoint( 0.25, 0.25 );
  alpha->AddPoint( 0.5,  0.5  );
  alpha->AddPoint( 0.75, 0.75 );
  alpha->AddPoint( 1.0,  1.0  );
}


void vtkTransferFunctionPresets::GetFlowTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, Flow, false, rgb, alpha );
}


void vtkTransferFunctionPresets::GetLONITransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    203, LONI, false, rgb, alpha );

  // double * rgbTable   = new double[3 * 203];
  // double * alphaTable = new double[203];

  // for ( unsigned int i = 0; i < 203; ++i )
  // {
  //   for ( unsigned int j = 0; j < 3; ++j )
  //     table[3 * i + j] = static_cast< double >( LONI[i][j] );

  //   alphaTable[i] = static_cast< double >( i ) / 202.0;
  // }

  // // lut->SetTableValue(
  // //   i, (double)LONI[i][0], (double)LONI[i][1], (double)LONI[i][2],
  // //   (double)i/202.0);

  // rgb->BuildFunctionFromTable( 0.0, 1.0, size, rgbTable );
  // delete [] rgbTable;

  // alpha->BuildFunctionFromTable( 0.0, 1.0, size, alphaTable );
  // delete [] alphaTable;
}

void vtkTransferFunctionPresets::GetLONIInversedTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromInversedTable(
    203, LONI, false, rgb, alpha );
}

void vtkTransferFunctionPresets::GetLONI2TransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    120, LONI2, false, rgb, alpha );

  // double * rgbTable   = new double[3 * 120];
  // double * alphaTable = new double[120];

  // for ( unsigned int i = 0; i < 120; ++i )
  // {
  //   for ( unsigned int j = 0; j < 3; ++j )
  //     table[3 * i + j] = static_cast< double >( LONI2[i][j] );

  //   alphaTable[i] = static_cast< double >( i ) / 119.0;
  // }

  // // lut->SetTableValue(
  // //   i, (double)LONI2[i][0], (double)LONI2[i][1], (double)LONI2[i][2],
  // //   (double)i/119.0);

  // rgb->BuildFunctionFromTable( 0.0, 1.0, size, rgbTable );
  // delete [] rgbTable;

  // alpha->BuildFunctionFromTable( 0.0, 1.0, size, alphaTable );
  // delete [] alphaTable;
}

void vtkTransferFunctionPresets::GetAsymmetryTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->AddRGBPoint( 0.0, 0.0, 0.0, 1.0 );
  rgb->AddRGBPoint( 0.2, 0.0, 1.0, 1.0 );
  rgb->AddRGBPoint( 0.4, 0.0, 1.0, 0.0 );
  rgb->AddRGBPoint( 0.6, 1.0, 1.0, 0.0 );
  rgb->AddRGBPoint( 0.8, 1.0, 0.0, 0.0 );
  rgb->AddRGBPoint( 1.0, 1.0, 0.0, 1.0 );

  alpha->AddPoint( 0.0, 1.0 );
  alpha->AddPoint( 0.2, 1.0 );
  alpha->AddPoint( 0.4, 1.0 );
  alpha->AddPoint( 0.6, 1.0 );
  alpha->AddPoint( 0.8, 1.0 );
  alpha->AddPoint( 1.0, 1.0 );
}


void vtkTransferFunctionPresets::GetPValueTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->SetColorSpaceToHSV();
  rgb->AddHSVPoint( 0.0, 0.6667, 0.5, 0.5 );
  rgb->AddHSVPoint( 1.0, 0.8333, 1.0, 1.0 );

  // rgb->AddHSVPoint(0, 0, 1, 1);
  // rgb->AddHSVPoint(0.05, 0.27, 0.6, 0.97, 0, 1);
  // rgb->AddHSVPoint(1, 0.6, 1, 1);

  alpha->AddPoint( 0.0, 0.0 );
  alpha->AddPoint( 1.0, 1.0 );

  // vtkLookupTable* lut = vtkLookupTable::New();
  // lut->SetTableRange (0, 1);
  // lut->SetSaturationRange (1.0, 1.5);
  // lut->SetHueRange (0.666, 0.8333);
  // //lut->SetHueRange (0.666, 0.0);
  // lut->SetValueRange (0.5, 1.0);
  // lut->SetAlphaRange (0, 1);
  // lut->Build();
}


void  vtkTransferFunctionPresets::GetRedBlackAlphaTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
  rgb->AddRGBPoint( 1.0, 1.0, 0.0, 0.0 );

  alpha->AddPoint( 0.0, 0.7333 );
  alpha->AddPoint( 1.0, 1.0    );

  // lut->SetTableValue(i, i/255.0, 0.0, 0.0, (i/255.0*68.0 + 187)/255.0);
}


void   vtkTransferFunctionPresets::GetGreenBlackAlphaTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
  rgb->AddRGBPoint( 1.0, 0.0, 1.0, 0.0 );

  alpha->AddPoint( 0.0, 0.7333 );
  alpha->AddPoint( 1.0, 1.0    );

  // lut->SetTableValue(i, 0.0, i/255.0, 0.0, (i/255.0*68.0 + 187)/255.0);
}

void vtkTransferFunctionPresets::GetBlueBlackAlphaTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  rgb->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
  rgb->AddRGBPoint( 1.0, 0.0, 0.0, 1.0 );

  alpha->AddPoint( 0.0, 0.7333 );
  alpha->AddPoint( 1.0, 1.0    );

  // lut->SetTableValue(i, 0.0, 0.0, i/255.0, (i/255.0*68.0 + 187)/255.0);
}


// void vtkTransferFunctionPresets::GetROITransferFunction(
//   vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
// {
//   vtkLookupTable* lut = vtkLookupTable::New();
//   lut->SetNumberOfTableValues (256);
//   lut->SetTableValue ( 0, 0.0, 0.0, 0.0, 0.0); // erase color

//   double colors[15][3] = {
//     { 1.0, 0.0, 0.0 }, // red	       
//     { 0.0, 1.0, 0.0 }, // green	       
//     { 0.0, 0.0, 1.0 }, // blue	       
//     { 1.0, 1.0, 0.0 }, // yellow       
//     { 0.0, 1.0, 1.0 }, // cyan	       
//     { 1.0, 0.0, 1.0 }, // magenta      
//     { 1.0, 0.5, 0.0 }, // orange       
//     { 0.0, 1.0, 0.5 }, // pale green   
//     { 0.5, 0.0, 1.0 }, // violet       
//     { 1.0, 1.0, 0.5 }, // pale yellow  
//     { 0.5, 1.0, 1.0 }, // pale cyan    
//     { 1.0, 0.5, 1.0 }, // pale magenta 
//     { 0.5, 1.0, 0.0 }, // yellow green 
//     { 0.0, 0.5, 1.0 }, // light blue   
//     { 1.0, 0.0, 0.5 }  // pink
//   };

//   for ( unsigned int i = 1; i < 256; i++)
//   {
//     double c = colors[(i - 1) % 15];
//     lut->SetTableValue( i, c[0], c[1], c[2], 0.5 );
//   }
// }


void vtkTransferFunctionPresets::GetVRMusclesBonesTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, VRMusclesBones, true, rgb, alpha );

  // double * rgbTable   = new double[3 * 256];
  // double * alphaTable = new double[256];
  // for ( unsigned int i = 0; i < 256; ++i )
  // {
  //   for ( unsigned int j = 0; j < 3; ++j )
  //     rgbTable[3 * i + j] =
  // 	static_cast< double >( VRMusclesBones[j * 256 + i] ) / 255.0;

  //   alphaTable[i] = log( 1.0 + 9.0 * static_cast< double >( i ) / 255.0 )
  //     / log( 10.0 );
  // }

  // // lut->SetTableValue(i,
  // // 		     (double)VRMusclesBones[      i] / 255.0,
  // // 		     (double)VRMusclesBones[  256+i] / 255.0,
  // // 		     (double)VRMusclesBones[2*256+i] / 255.0,
  // // 		     // (double)(i)/255.0
  // // 		     log( 1.0 + 9.0 * (double)(i) / 255.0) / log( 10.0 ) );
  // rgb->BuildFunctionFromTable( 0.0, 1.0, 256, rgbTable );
  // delete [] rgbTable;

  // alpha->BuildFunctionFromTable( 0.0, 1.0, 256, alphaTable );
  // delete [] alphaTable;
}


void vtkTransferFunctionPresets::GetVRBonesTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, VRBones, true, rgb, alpha );
}


void vtkTransferFunctionPresets::GetVRRedVesselsTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, VRRedVessels, true, rgb, alpha );
}


void vtkTransferFunctionPresets::GetCardiacTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, Cardiac, true, rgb, alpha );
}


void vtkTransferFunctionPresets::GetGrayRainbowTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, GrayRainbow, true, rgb, alpha );
}



void vtkTransferFunctionPresets::GetSternTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, Stern, true, rgb, alpha );
}


void vtkTransferFunctionPresets::GetBlackBodyTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, BlackBody, true, rgb, alpha );
}

void vtkTransferFunctionPresets::GetBinaryMapTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
    double realHueValue = 0;
    double factor = (1.0 + sqrt(5.0)) / 2.0;

    float rgbValue[3];
    float hsv[3];

    hsv[1] = 1.0;
    hsv[2] = 1.0;

    rgb->AddRGBPoint(0.0,0.0,0.0,0.0);

    for (unsigned int i = 1;i < 256;++i)
    {
        hsv[0] = realHueValue;
        vtkMath::HSVToRGB(hsv, rgbValue);

        rgb->AddRGBPoint(i / 255.0,rgbValue[0],rgbValue[1],rgbValue[2]);

        realHueValue += 1.0 / factor;
        if (realHueValue > 1.0)
            realHueValue -= 1.0;
    }

    alpha->AddPoint(0.0, 1.0);
    alpha->AddPoint(1.0, 1.0);
}

void vtkTransferFunctionPresets::GetChannelnessTransferFunction(
  vtkColorTransferFunction * rgb, vtkPiecewiseFunction * alpha )
{
  vtkTransferFunctionPresets::GetTransferFunctionFromTable(
    256, Channelness, true, rgb, alpha );
}

