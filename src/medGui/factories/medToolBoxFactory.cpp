/* medToolBoxFactory.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 14:33:05 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:06:00 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 32
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medToolBox.h"
#include "medToolBoxFactory.h"

struct MEDGUI_EXPORT medHelperStrings{
    QString name;
    QString description;
    medHelperStrings():name(),description(){}
    medHelperStrings(QString name,QString description):
        name(name),description(description){}
};

class medToolBoxFactoryPrivate
{
public:
    typedef QHash<QString,
        QPair < medToolBoxFactory::medToolBoxRegistrationCustomCreator,
                medHelperStrings> > medToolBoxRegistrationCustomCreatorHash;
    typedef QHash<QString,
        QPair < medToolBoxFactory::medToolBoxDiffusionCustomCreator,
                medHelperStrings > > medToolBoxDiffusionCustomCreatorHash;
    typedef QHash<QString,
        QPair < medToolBoxFactory::medToolBoxFilteringCustomCreator,
                medHelperStrings > > medToolBoxFilteringCustomCreatorHash;
    typedef QHash<QString,
        QPair < medToolBoxFactory::medToolBoxCompositeDataSetImporterCustomCreator,
                medHelperStrings > > medToolBoxCompositeDataSetImporterCustomCreatorHash;
    typedef QHash<QString,
        QPair < medToolBoxFactory::medToolBoxSegmentationCustomCreator,
                medHelperStrings > > medToolBoxSegmentationCustomCreatorHash;

    medToolBoxRegistrationCustomCreatorHash custom_registration_creators;
    medToolBoxDiffusionCustomCreatorHash custom_diffusion_creators;
    medToolBoxCompositeDataSetImporterCustomCreatorHash custom_compositedatasetimporter_creators;
    medToolBoxFilteringCustomCreatorHash custom_filtering_creators;
    medToolBoxSegmentationCustomCreatorHash custom_segmentation_creators;
};

medToolBoxFactory *medToolBoxFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medToolBoxFactory;

    return s_instance;
}

bool medToolBoxFactory::registerCustomRegistrationToolBox(QString identifier,
                                                          QString name,
                                                          QString description,
                                                          medToolBoxRegistrationCustomCreator func)
{
    if(!d->custom_registration_creators.contains(identifier))
    {
        medHelperStrings helpers(name,description);
        d->custom_registration_creators.insert( identifier,
                                                QPair <
                                                medToolBoxRegistrationCustomCreator,
                                                medHelperStrings>( func, helpers ));
        return true;
    }

    return false;
}

QList<QString> medToolBoxFactory::registrationToolBoxes(void)
{
    return d->custom_registration_creators.keys();
}

medToolBoxRegistrationCustom *medToolBoxFactory::createCustomRegistrationToolBox(QString type, QWidget *parent)
{
    if(!d->custom_registration_creators.contains(type))
        return NULL;

    medToolBoxRegistrationCustom *toolbox =
            (d->custom_registration_creators[type]).first(parent);

    return toolbox;
}

bool medToolBoxFactory::registerCustomDiffusionToolBox(QString identifier,
                                                       QString name,
                                                       QString description,
                                                       medToolBoxDiffusionCustomCreator func)
{
    if(!d->custom_diffusion_creators.contains(identifier))
    {
        medHelperStrings helpers(name,description);
        d->custom_diffusion_creators.insert(identifier,
                                            QPair <
                                            medToolBoxDiffusionCustomCreator,
                                            medHelperStrings>( func, helpers ));
        return true;
    }

    return false;
}

QList<QString> medToolBoxFactory::diffusionToolBoxes(void)
{
    return d->custom_diffusion_creators.keys();
}

medToolBoxDiffusionCustom *medToolBoxFactory::createCustomDiffusionToolBox(QString type, QWidget *parent)
{
    if(!d->custom_diffusion_creators.contains(type))
        return NULL;

    medToolBoxDiffusionCustom *toolbox =
            d->custom_diffusion_creators[type].first(parent);

    return toolbox;
}

bool medToolBoxFactory::registerCustomCompositeDataSetImporterToolBox(QString identifier,
                                                                      QString name,
                                                                      QString description,
                                                                      medToolBoxCompositeDataSetImporterCustomCreator func)
{
    if(!d->custom_compositedatasetimporter_creators.contains(identifier))
    {
        medHelperStrings helpers(name,description);
        d->custom_compositedatasetimporter_creators.insert(identifier,
                                                           QPair <
                                                           medToolBoxCompositeDataSetImporterCustomCreator,
                                                           medHelperStrings>( func, helpers ));
        return true;
    }
    return false;
}

bool medToolBoxFactory::registerCustomSegmentationToolBox(QString identifier,
                                                       QString name,
                                                       QString description,
                                                       medToolBoxSegmentationCustomCreator func)
{
    if(!d->custom_filtering_creators.contains(identifier))
    {
        medHelperStrings helpers(name,description);
        d->custom_segmentation_creators.insert(identifier,
                                            QPair <
                                            medToolBoxSegmentationCustomCreator,
                                            medHelperStrings>( func, helpers ));
        return true;
    }

    return false;
}

QList<QString> medToolBoxFactory::segmentationToolBoxes(void)
{
    return d->custom_segmentation_creators.keys();
}

medToolBoxSegmentationCustom *medToolBoxFactory::createCustomSegmentationToolBox(QString type, QWidget *parent)
{
    if(!d->custom_segmentation_creators.contains(type))
        return NULL;

    medToolBoxSegmentationCustom *toolbox =
            d->custom_segmentation_creators[type].first(parent);
    return toolbox;
}


bool medToolBoxFactory::registerCustomFilteringToolBox(QString identifier,
                                                       QString name,
                                                       QString description,
                                                       medToolBoxFilteringCustomCreator func)
{
    if(!d->custom_filtering_creators.contains(identifier))
    {
        medHelperStrings helpers(name,description);
        d->custom_filtering_creators.insert(identifier,
                                            QPair <
                                            medToolBoxFilteringCustomCreator,
                                            medHelperStrings>( func, helpers ));
        return true;
    }

    return false;
}

QList<QString> medToolBoxFactory::compositeDataSetImporterToolBoxes(void)
{
    return d->custom_compositedatasetimporter_creators.keys();
}

medToolBoxCompositeDataSetImporterCustom *medToolBoxFactory::createCustomCompositeDataSetImporterToolBox(QString type, QWidget *parent)
{
    if(!d->custom_compositedatasetimporter_creators.contains(type))
        return NULL;

    medToolBoxCompositeDataSetImporterCustom *toolbox =
            d->custom_compositedatasetimporter_creators[type].first(parent);
    return toolbox;
}

QList<QString> medToolBoxFactory::filteringToolBoxes(void)
{
    return d->custom_filtering_creators.keys();
}

medToolBoxFilteringCustom *medToolBoxFactory::createCustomFilteringToolBox(QString type, QWidget *parent)
{
    if(!d->custom_filtering_creators.contains(type))
        return NULL;

    medToolBoxFilteringCustom *toolbox =
            d->custom_filtering_creators[type].first(parent);

    return toolbox;
}

QPair<QString, QString>
medToolBoxFactory::diffusionToolBoxDetailsFromId (
        const QString& id )
{
    if (d->custom_diffusion_creators.contains(id))
    {
        medHelperStrings helper  = d->custom_diffusion_creators[id].second;
        return QPair<QString, QString>(helper.name,helper.description);
    }
    return QPair<QString, QString>();
}

QPair<QString, QString>
medToolBoxFactory::registrationToolBoxDetailsFromId (
        const QString& id )
{
    if (d->custom_registration_creators.contains(id))
    {
        medHelperStrings helper  = d->custom_registration_creators[id].second;
        return QPair<QString, QString>(helper.name,helper.description);
    }
    return QPair<QString, QString>();
}

QPair<QString, QString>
medToolBoxFactory::filteringToolBoxDetailsFromId (
        const QString& id )
{
    if (d->custom_filtering_creators.contains(id))
    {
        medHelperStrings helper  = d->custom_filtering_creators[id].second;
        return QPair<QString, QString>(helper.name,helper.description);
    }
    return QPair<QString, QString>();
}

QPair<QString, QString>
medToolBoxFactory::compositeToolBoxDetailsFromId (
        const QString& id )
{
    if (d->custom_compositedatasetimporter_creators.contains(id))
    {
        medHelperStrings helper  = d->custom_compositedatasetimporter_creators[id].second;
        return QPair<QString, QString>(helper.name,helper.description);
    }
    return QPair<QString, QString>();
}

QPair<QString, QString>
medToolBoxFactory::segmentationToolBoxDetailsFromId (
        const QString& id )
{
    if (d->custom_segmentation_creators.contains(id))
    {
        medHelperStrings helper  = d->custom_segmentation_creators[id].second;
        return QPair<QString, QString>(helper.name,helper.description);
    }
    return QPair<QString, QString>();
}

medToolBoxFactory::medToolBoxFactory(void) : dtkAbstractFactory(), d(new medToolBoxFactoryPrivate)
{

}

medToolBoxFactory::~medToolBoxFactory(void)
{
    delete d;

    d = NULL;
}

medToolBoxFactory *medToolBoxFactory::s_instance = NULL;
