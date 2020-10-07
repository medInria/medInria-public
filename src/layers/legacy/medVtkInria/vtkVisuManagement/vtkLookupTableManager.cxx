/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkLookupTableManager.h"
#include <vtkObjectFactory.h>
#include <vtkMath.h>


vtkStandardNewMacro(vtkLookupTableManager);

#include "lut/Spectrum.h"
#include "lut/HotMetal.h"
#include "lut/GEColor.h"
#include "lut/Flow.h"
#include "lut/LONI.h"
#include "lut/LONI2.h"
#include "lut/Asymmetry.h"
#include "lut/VRMusclesBones.h"
#include "lut/Cardiac.h"
#include "lut/GrayRainbow.h"
#include "lut/HotGreen.h"
#include "lut/HotIron.h"
#include "lut/Stern.h"
#include "lut/VRBones.h"
#include "lut/VRRedVessels.h"
#include "lut/BlackBody.h"
#include "lut/jet.h"


#include <time.h>

vtkLookupTableManager::vtkLookupTableManager()
{}


vtkLookupTableManager::~vtkLookupTableManager()
{}


std::vector<std::string> vtkLookupTableManager::GetAvailableLookupTables()
{
  std::string lutNames[]={"Default",
			  "Black & White",
			  "Black & White Inversed",
			  "Spectrum",
			  "Hot Metal",
			  "Hot Green",
			  "Hot Iron",
			  "GE",
			  "Flow",
			  "Loni",
              "Loni Inversed",
			  "Loni 2",
			  "Asymmetry",
			  "P-Value",
			  "Red Black Alpha",
			  "Green Black Alpha",
			  "Blue Black Alpha",
			  "Muscles & Bones",
			  "Bones",
			  "Red Vessels",
			  "Cardiac",
			  "Gray Rainbow",
			  "Stern",
			  "Black Body",
              "Jet",
              "Binary Map"};
    

  std::vector<std::string> v_lutNames;
  int nbLUTs = sizeof(lutNames)/sizeof(std::string);
  for( int i=0; i<nbLUTs; i++)
  {
    v_lutNames.push_back(lutNames[i]);
  }

  return v_lutNames;
}


QStringList vtkLookupTableManager::GetAvailableLookupTablesInStringList()
{
    QStringList luts;
    const std::vector<std::string> & vec = vtkLookupTableManager::GetAvailableLookupTables();
    for (std::vector<std::string>::const_iterator it = vec.begin(); it < vec.end(); ++it)
        luts.append(QString::fromStdString(*it));
    return luts;
}

vtkLookupTable* vtkLookupTableManager::GetLookupTable(const int& n)
{
  std::vector< std::string > luts = GetAvailableLookupTables();
  if (n<0 || n>= (int)luts.size())
    return vtkLookupTableManager::GetBWLookupTable();

  return GetLookupTable ( luts[n] );
}


vtkLookupTable* vtkLookupTableManager::GetLookupTable(const std::string & name)
{
  if (name=="Default")
    return vtkLookupTableManager::GetBWLookupTable();
  else if ( name == "Black & White" )
    return vtkLookupTableManager::GetBWLookupTable();
  else if ( name == "Black & White Inversed" )
    return vtkLookupTableManager::GetBWInverseLookupTable();
  else if ( name == "Spectrum" )
    return vtkLookupTableManager::GetSpectrumLookupTable();
  else if ( name == "Hot Metal" )
    return vtkLookupTableManager::GetHotMetalLookupTable();
  else if ( name == "Hot Green" )
    return vtkLookupTableManager::GetHotGreenLookupTable();
  else if ( name == "Hot Iron" )
    return vtkLookupTableManager::GetHotIronLookupTable();
  else if ( name == "GE" )
    return vtkLookupTableManager::GetGEColorLookupTable();
  else if ( name == "Flow" )
    return vtkLookupTableManager::GetFlowLookupTable();
  else if ( name == "Loni" )
    return  vtkLookupTableManager::GetLONILookupTable();
  else if ( name == "Loni Inversed" )
    return  vtkLookupTableManager::GetLONIInversedLookupTable();
  else if ( name == "Loni 2" )
    return vtkLookupTableManager::GetLONI2LookupTable();
  else if ( name == "Asymmetry" )
    return vtkLookupTableManager::GetAsymmetryLookupTable();
  else if ( name == "P-Value" )
    return vtkLookupTableManager::GetPValueLookupTable();
  else if ( name == "Red Black Alpha" )
    return vtkLookupTableManager::GetRedBlackAlphaLookupTable();
  else if ( name == "Green Black Alpha" )
    return vtkLookupTableManager::GetGreenBlackAlphaLookupTable();
  else if ( name == "Blue Black Alpha" )
    return vtkLookupTableManager::GetBlueBlackAlphaLookupTable();
  else if ( name == "Muscles & Bones" )
    return vtkLookupTableManager::GetVRMusclesBonesLookupTable();
  else if ( name == "Bones" )
    return vtkLookupTableManager::GetVRBonesLookupTable();
  else if ( name == "Red Vessels" )
    return vtkLookupTableManager::GetVRRedVesselsLookupTable();
  else if ( name == "Cardiac" )
    return vtkLookupTableManager::GetCardiacLookupTable();
  else if ( name == "Gray Rainbow" )
    return vtkLookupTableManager::GetGrayRainbowLookupTable();
  else if ( name == "Stern" )
    return vtkLookupTableManager::GetSternLookupTable();
  else if ( name == "Black Body" )
    return vtkLookupTableManager::GetBlackBodyLookupTable();
  else if ( name == "Jet" )
    return vtkLookupTableManager::GetJetLookupTable();
  else if (name == "Binary Map")
    return vtkLookupTableManager::GetBinaryMapLookupTable();
  else
    return vtkLookupTableManager::GetBWLookupTable();
}

vtkLookupTable* vtkLookupTableManager::removeLUTAlphaChannel(vtkLookupTable * lut)
{
    // remove the alpha channel from the LUT, it messes up the mesh
    if (lut)
    {
        double values[4];
        for (int i = 0; i < lut->GetNumberOfTableValues(); i++)
        {
            lut->GetTableValue(i, values);
            values[3] = 1.0;
            lut->SetTableValue(i, values);
        }
        return lut;
    }
    return nullptr;

}

vtkLookupTable* vtkLookupTableManager::GetBWLookupTable()
{
  vtkLookupTable* bwLut = vtkLookupTable::New();
  bwLut->SetTableRange (0, 1);
  bwLut->SetSaturationRange (0, 0);
  bwLut->SetHueRange (0, 0);
  bwLut->SetValueRange (0, 1);
  bwLut->SetAlphaRange (0, 1);
  bwLut->Build();

  return bwLut;
}


vtkLookupTable* vtkLookupTableManager::GetBWInverseLookupTable()
{
  vtkLookupTable* bwLut = vtkLookupTable::New();
  bwLut->SetTableRange (0, 1);
  bwLut->SetSaturationRange (0, 0);
  bwLut->SetHueRange (0, 0);
  bwLut->SetValueRange (1, 0);
  bwLut->SetAlphaRange (1, 0);
  bwLut->Build();

  return bwLut;
}



vtkLookupTable* vtkLookupTableManager::GetSpectrumLookupTable()
{
   vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();

  for( int i=0; i<256; i++)
  {
	lut->SetTableValue(i, (double)Spectrum[i]/255.0, (double)Spectrum[256+i]/255.0, (double)Spectrum[256*2+i]/255.0, (double)(i)/255.0 );
  }

  return lut;
}


vtkLookupTable* vtkLookupTableManager::GetHotMetalLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();

  for( int i=0; i<256; i++)
  {
	lut->SetTableValue(i, (double)HotMetal[i]/255.0, (double)HotMetal[256+i]/255.0, (double)HotMetal[256*2+i]/255.0, (double)i/255.0 );
  }

  return lut;
}


vtkLookupTable* vtkLookupTableManager::GetGEColorLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();

  for( int i=0; i<256; i++)
  {
	lut->SetTableValue(i, (double)GEColor[i]/255.0, (double)GEColor[256+i]/255.0, (double)GEColor[256*2+i]/255.0, (double)i/255.0 );
  }

  return lut;
}


vtkLookupTable* vtkLookupTableManager::GetFlowLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();

  for( int i=0; i<256; i++)
  {
	lut->SetTableValue(i, (double)Flow[i]/255.0, (double)Flow[256+i]/255.0, (double)Flow[256*2+i]/255.0, (double)i/255.0);
  }

  return lut;
}


vtkLookupTable* vtkLookupTableManager::GetLONILookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(203);
  lut->Build();

  for( int i=0; i<203; i++)
  {
    lut->SetTableValue(i, (double)LONI[i][0], (double)LONI[i][1], (double)LONI[i][2], (double)i/202.0);
  }

  return lut;
}

vtkLookupTable* vtkLookupTableManager::GetLONIInversedLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(203);
  lut->Build();

  for( int i=0; i<203; i++)
  {
    lut->SetTableValue(i, (double)LONI[202-i][0], (double)LONI[202-i][1], (double)LONI[202-i][2], (double)i/202.0);
  }

  return lut;
}


vtkLookupTable* vtkLookupTableManager::GetLONI2LookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(120);
  lut->Build();

  for( int i=0; i<120; i++)
  {
    lut->SetTableValue(i, (double)LONI2[i][0], (double)LONI2[i][1], (double)LONI2[i][2], (double)i/119.0);
  }

  return lut;
}


vtkLookupTable* vtkLookupTableManager::GetAsymmetryLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();

  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)AsymmetryLUT[i][0], (double)AsymmetryLUT[i][1], (double)AsymmetryLUT[i][2], (double)i/255.0);
  }

  return lut;
}


vtkLookupTable* vtkLookupTableManager::GetPValueLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetTableRange (0, 1);
  lut->SetSaturationRange (1.0, 1.5);
  lut->SetHueRange (0.666, 0.8333);
  //lut->SetHueRange (0.666, 0.0);
  lut->SetValueRange (0.5, 1.0);
  lut->SetAlphaRange (0, 1);
  lut->Build();

  return lut;
}


vtkLookupTable*  vtkLookupTableManager::GetRedBlackAlphaLookupTable()
{  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();

  lut->SetTableValue(0,0,0,0,0);

  for( int i=1; i<256; i++)
  {
      lut->SetTableValue(i, i/255.0, 0.0, 0.0, (i/255.0*68.0 + 187)/255.0);
  }

  return lut;
}

vtkLookupTable*   vtkLookupTableManager::GetGreenBlackAlphaLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();

  lut->SetTableValue(0,0,0,0,0);

  for( int i=1; i<256; i++)
  {
      lut->SetTableValue(i, 0.0, i/255.0, 0.0, (i/255.0*68.0 + 187)/255.0);
  }

  return lut;

}

vtkLookupTable* vtkLookupTableManager::GetBlueBlackAlphaLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();

  lut->SetTableValue(0,0,0,0,0);

  for( int i=1; i<256; i++)
  {
      lut->SetTableValue(i, 0.0, 0.0, i/255.0, (i/255.0*68.0 + 187)/255.0);
  }

  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetVRMusclesBonesLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)VRMusclesBones[i]/255.0, (double)VRMusclesBones[256+i]/255.0, (double)VRMusclesBones[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}

vtkLookupTable* vtkLookupTableManager::GetROILookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues (256);
  lut->SetTableValue (0, 0.0, 0.0, 0.0, 0.0); // erase color

  lut->SetTableValue (1, 1.0, 0.0, 0.0, 0.5);
  lut->SetTableValue (2, 0.0, 1.0, 0.0, 0.5);
  lut->SetTableValue (3, 0.0, 0.0, 1.0, 0.5);
  lut->SetTableValue (4, 1.0, 1.0, 0.0, 0.5);
  lut->SetTableValue (5, 0.0, 1.0, 1.0, 0.5);
  lut->SetTableValue (6, 1.0, 0.0, 1.0, 0.5);

  lut->SetTableValue (7, 1.0, 0.5, 0.0, 0.5);
  lut->SetTableValue (8, 0.0, 1.0, 0.5, 0.5);
  lut->SetTableValue (9, 0.5, 0.0, 1.0, 0.5);
  lut->SetTableValue (10, 1.0, 1.0, 0.5, 0.5);
  lut->SetTableValue (11, 0.5, 1.0, 1.0, 0.5);
  lut->SetTableValue (12, 1.0, 0.5, 1.0, 0.5);



  // Fill the rest of the labels with color ramps, code taken from SNAP
//   for (int i = 13; i < 256; i++)
//   {
//     if (i < 85)
//     {
//       lut->SetTableValue (i, ((84.0-i)/85.0 * 200.0 + 50.0)/255.0, (i/85.0 * 200.0 + 50.0)/255.0, 0, 0.5);
//     }
//     else if (i < 170)
//     {
//       lut->SetTableValue (i, 0, ((169.0-i)/85.0 * 200.0 + 50)/255.0, ((i-85)/85.0 * 200.0 + 50)/255.0, 0.5);
//     }
//     else
//     {
//       lut->SetTableValue (i, ((i-170)/85.0 * 200.0 + 50)/255.0, 0.0, ((255.0-i)/85.0 * 200.0 + 50)/255.0, 0.5);
//     }
//   }


  // Fill the rest with random colors

//   for (int i = 13; i < 256; i++)
//   {
//     srand (clock());

//     // put a random color
//     int i1 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
//     int i2 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
//     int i3 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
//     double rand_1 = (double)(i1)/(100.0);
//     double rand_2 = (double)(i2)/(100.0);
//     double rand_3 = (double)(i3)/(100.0);
//     double r = rand_1, g = rand_2, b = rand_3;
//     if (i1 < 33)
//       r = 1;
//     else if (i1 < 66)
//       g = 1;
//     else if (i1 < 100)
//       b = 1;

//     lut->SetTableValue (i, r, g, b, 0.5);
//   }

  for (int i = 12; i < 256; i++)
  {
    if (i%12 == 0)
      lut->SetTableValue (i, 1.0, 0.0, 0.0, 0.5);
    else if (i%12 == 1)
      lut->SetTableValue (i, 0.0, 1.0, 0.0, 0.5);
    else if (i%12 == 2)
      lut->SetTableValue (i, 0.0, 0.0, 1.0, 0.5);
    else if (i%12 == 3)
      lut->SetTableValue (i, 1.0, 1.0, 0.0, 0.5);
    else if (i%12 == 4)
      lut->SetTableValue (i, 0.0, 1.0, 1.0, 0.5);
    else if (i%12 == 5)
      lut->SetTableValue (i, 1.0, 0.0, 1.0, 0.5);
    else if (i%12 == 6)
      lut->SetTableValue (i, 1.0, 0.5, 0.0, 0.5);
    else if (i%12 == 7)
      lut->SetTableValue (i, 0.0, 1.0, 0.5, 0.5);
    else if (i%12 == 8)
      lut->SetTableValue (i, 0.5, 0.0, 1.0, 0.5);
    else if (i%12 == 9)
      lut->SetTableValue (i, 1.0, 1.0, 0.5, 0.5);
    else if (i%12 == 10)
      lut->SetTableValue (i, 0.5, 1.0, 1.0, 0.5);
    else if (i%12 == 11)
      lut->SetTableValue (i, 1.0, 0.5, 1.0, 0.5);
  }

  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetCardiacLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)Cardiac[i]/255.0, (double)Cardiac[256+i]/255.0, (double)Cardiac[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetGrayRainbowLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)GrayRainbow[i]/255.0, (double)GrayRainbow[256+i]/255.0, (double)GrayRainbow[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetHotGreenLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)HotGreen[i]/255.0, (double)HotGreen[256+i]/255.0, (double)HotGreen[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetHotIronLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)HotIron[i]/255.0, (double)HotIron[256+i]/255.0, (double)HotIron[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetSternLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)Stern[i]/255.0, (double)Stern[256+i]/255.0, (double)Stern[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetVRBonesLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)VRBones[i]/255.0, (double)VRBones[256+i]/255.0, (double)VRBones[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetVRRedVesselsLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)VRRedVessels[i]/255.0, (double)VRRedVessels[256+i]/255.0, (double)VRRedVessels[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetBlackBodyLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(256);
  lut->Build();
  
  for( int i=0; i<256; i++)
  {
    lut->SetTableValue(i, (double)BlackBody[i]/255.0, (double)BlackBody[256+i]/255.0, (double)BlackBody[256*2+i]/255.0, /*(double)(i)/255.0*/ log (1.0+(double)(i)/255.0*9.0)/log (10.0) );
  }
  
  return lut;

}


vtkLookupTable* vtkLookupTableManager::GetJetLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->SetNumberOfTableValues(64);
  lut->Build();
  
  for( int i=0; i<64; i++)
  {
    lut->SetTableValue(i, (double)jet[i][0], (double)jet[i][1], (double)jet[i][2], (double)1.0);
  }
  
  return lut;

}

vtkLookupTable* vtkLookupTableManager::GetBinaryMapLookupTable()
{
    vtkLookupTable* lut = vtkLookupTable::New();
    lut->SetNumberOfTableValues(256);
    lut->Build();

    double realHueValue = 0;
    double factor = (1.0 + sqrt(5.0)) / 2.0;

    float rgb[3];
    float hsv[3];

    hsv[1] = 1.0;
    hsv[2] = 1.0;

    lut->SetTableValue(0,0.0,0.0,0.0,0.0);

    for (unsigned int i = 1;i < 256;++i)
    {
        hsv[0] = realHueValue;
        vtkMath::HSVToRGB(hsv, rgb);

        lut->SetTableValue(i,rgb[0],rgb[1],rgb[2],1.0);

        realHueValue += 1.0 / factor;
        if (realHueValue > 1.0)
            realHueValue -= 1.0;
    }

    return lut;
}
