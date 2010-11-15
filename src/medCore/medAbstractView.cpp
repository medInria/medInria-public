#include "medAbstractView.h"


medAbstractView::medAbstractView(medAbstractView *parent) : dtkAbstractView(parent)
{
    QStringList lut;
    lut << "Default"
	<< "Black&White"
	<< "Black&WhiteInversed"
	<< "Spectrum"
	<< "HotMetal"
	<< "GE"
	<< "Flow"
	<< "Loni"
	<< "Loni2"
	<< "Asymmetry"
	<< "PValue"
	<< "blueBlackAlpha"
	<< "greenBlackAlpha"
	<< "redBlackAlpha"
	<< "Muscles&Bones"
      	<< "Red Vessels"
      	<< "Bones"
	<< "Stern";
    
    // properties to keep up to date synchronization
    this->addProperty ("Daddy",    QStringList() << "true" << "false");
    this->addProperty ("PositionLinked",  QStringList() << "true" << "false");
    this->addProperty ("CameraLinked",    QStringList() << "true" << "false");
    this->addProperty ("WindowingLinked", QStringList() << "true" << "false");
    
    // properties acting on image display
    this->addProperty ("Orientation",           QStringList() << "Axial" << "Sagittal" << "Coronal" << "3D");
    this->addProperty ("LookupTable",           lut);
    this->addProperty ("ShowScalarBar",         QStringList() << "true" << "false");
    this->addProperty ("ShowAxis",              QStringList() << "true" << "false");
    this->addProperty ("ShowRuler",             QStringList() << "true" << "false");
    this->addProperty ("ShowAnnotations",       QStringList() << "true" << "false");
    this->addProperty ("3DMode",                QStringList() << "VR" << "MPR" << "MIP - Maximum" << "MIP - Minimum" << "Off");    
    this->addProperty ("Renderer",              QStringList() << "GPU" << "Ray Cast / Texture" << "Ray Cast" << "Texture" << "Default");
    this->addProperty ("UseLOD",                QStringList() << "On" << "Off" );
    this->addProperty ("Cropping",              QStringList() << "true" << "false");
    this->addProperty ("Preset",                QStringList() << "None" << "VR Muscles&Bones"
		                                              << "Vascular I" << "Vascular II" << "Vascular III" << "Vascular IV"
		                                              << "Standard" << "Soft" << "Soft on White" << "Soft on Blue"
		                                              << "Red on White" << "Glossy");
    // image interaction
    this->addProperty ("MouseInteraction",      QStringList() << "Zooming" << "Windowing" << "Slicing" << "Measuring");

    // do not set properties, leave it to subclass
}


medAbstractView::medAbstractView(const medAbstractView& view) : dtkAbstractView(view)
{

}


void medAbstractView::setColorLookupTable(int min_range,
                                     int max_range,
                                     int size,
                                     const int & table)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::setColorLookupTable( QList<double> scalars,
					   QList<QColor> colors )
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::setTransferFunctions( QList<double> scalars,
					    QList<QColor> colors )
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::getTransferFunctions( QList<double> & scalars,
					    QList<QColor> & colors )
{
    DTK_DEFAULT_IMPLEMENTATION;
}

QWidget *medAbstractView::receiverWidget(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

void medAbstractView::linkPosition (dtkAbstractView *view, bool value)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::linkCamera (dtkAbstractView *view, bool value)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::linkWindowing (dtkAbstractView *view, bool value)
{
    DTK_DEFAULT_IMPLEMENTATION;
}
