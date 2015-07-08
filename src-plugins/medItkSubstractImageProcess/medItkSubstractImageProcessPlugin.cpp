#include "medItkSubstractImageProcessPlugin.h"

#include <dtkCore>


#include <medItkSubstractImageProcessPresenter.h>

// ///////////////////////////////////////////////////////////////////
// maphysSparseSolverPlugin
// ///////////////////////////////////////////////////////////////////

void medItkSubstractImageProcessPlugin::initialize(void)
{
    dtkDebug() << "\n\n\n\n\npouette";
    std::cout << "\n\n\n\n\npouette" << std::endl;
    process::substractImage::pluginFactory().record("medItkSubstractImageProcess", medItkSubstractImageProcessPresenterCreator);
}

void medItkSubstractImageProcessPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medItkSubstractImageProcess)

//
// maphysSparseSolverPlugin.cpp ends here

