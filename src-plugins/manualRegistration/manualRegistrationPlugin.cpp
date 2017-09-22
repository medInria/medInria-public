/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <manualRegistration.h>
#include <manualRegistrationPlugin.h>
#include <manualRegistrationToolBox.h>

manualRegistrationPlugin::manualRegistrationPlugin(QObject *parent) : medPlugin(parent)
{
}

bool manualRegistrationPlugin::initialize()
{
    if(!manualRegistration::registered())
    {
        dtkWarn() << "Unable to register manualRegistration type";
    }
    
    if ( !manualRegistrationToolBox::registered() )
    {
        dtkWarn() << "Unable to register manualRegistration toolbox";
    }
    
    return true;
}

QString manualRegistrationPlugin::description() const
{
    QString description = \
            tr("Applies transformations as they can be found in the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>: <br><br> \
               - <a href=\"https://itk.org/Doxygen/html/classitk_1_1VersorRigid3DTransform.html\" style=\"color: #cc0000\" >Rigid</a> transformation<br>\
            - <a href=\"https://itk.org/Doxygen/html/classitk_1_1AffineTransform.html\" style=\"color: #cc0000\" >Affine</a> transformation");
    return description;
}

QString manualRegistrationPlugin::name() const
{
    return "Manual Registration";
}

QString manualRegistrationPlugin::version() const
{
    return MANUALREGISTRATIONPLUGIN_VERSION;
}

QStringList manualRegistrationPlugin::types() const
{
    return QStringList() << "manualRegistration";
}

Q_EXPORT_PLUGIN2(manualRegistrationPlugin, manualRegistrationPlugin)
