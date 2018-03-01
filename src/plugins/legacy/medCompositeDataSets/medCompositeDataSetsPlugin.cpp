/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCompositeDataSetsPlugin.h>
#include <medCompositeDataSetsReader.h>
#include <medCompositeDataSetsWriter.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medCompositeDataSetsPluginPrivate
//
// This class is where all data specific to the plugin and that should
// be hidden from the outside world should go. The content of this class
// is totally free.
// /////////////////////////////////////////////////////////////////

class medCompositeDataSetsPluginPrivate 
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// medCompositeDataSetsPlugin
// /////////////////////////////////////////////////////////////////

//  Plugin construction.
//  Allocate private data here (see class %1PluginPrivate).

medCompositeDataSetsPlugin::medCompositeDataSetsPlugin(QObject *parent): dtkPlugin(parent) {
}

//  Plugin destruction.
//  Free all members allocated in the constructor.

medCompositeDataSetsPlugin::~medCompositeDataSetsPlugin() { }

//  Initialization of the plugin.

bool medCompositeDataSetsPlugin::initialize() {
    const bool have_types  = MedInria::medCompositeDataSetsBase::initialize().size()!=0;
    const bool have_reader = medCompositeDataSetsReader::initialize();
    const bool have_writer = medCompositeDataSetsWriter::initialize();
    return have_types && have_reader && have_writer;
}

//  Unitialize the plugin.
//  If some private data are allocated in the initialize method
//  free them here.

bool medCompositeDataSetsPlugin::uninitialize() {
    return true;
}

//  Name of the plugin.

QString medCompositeDataSetsPlugin::name() const {
    return "medCompositeDataSetsPlugin";
}

//  Description of the plugin.

QString medCompositeDataSetsPlugin::description() const {
    return tr("Manager for composite data types");
}

//  Tags to appear in the tag cloud.

QStringList medCompositeDataSetsPlugin::tags() const {
    return QStringList() << "composite data sets" << "metadata" << "archive";
}

//  Names of the types handled by the plugin.

QStringList medCompositeDataSetsPlugin::types() const {
    return registered_types;
}

Q_EXPORT_PLUGIN2(medCompositeDataSetsPlugin, medCompositeDataSetsPlugin)
