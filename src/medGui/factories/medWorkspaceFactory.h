/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"

#include <dtkCore/dtkAbstractFactory.h>

#include <QtCore>

class medWorkspace;
class medWorkspaceFactoryPrivate;
struct medWorkspaceDetails;

class MEDGUI_EXPORT medWorkspaceFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medWorkspace *(*medWorkspaceCreator)(QWidget* parent);
    typedef bool (*medWorkspaceIsUsable)();

public:
    static medWorkspaceFactory *instance();

    QList<QString> workspaces();

    /**
     * @brief Registers a medWorkspace type with the factory.
     *
     *
     * This method is templated with the workspaceType.
     * This is a convience method.
     * The constructor of the workspace - with a (QWidget *parent) signature -
     * is used to create a function pointer to allocate memory.
     * The workspace source code doesn't need to contain any such
     * function itself.
     *
     * @param identifier Identifier of the type.
     * @param name Human readable name (Potentially localised).
     * @param description short description (Potentially localised).
     */
    template <typename workspaceType>
    bool registerWorkspace(QString identifier,
                         QString name,
                         QString description){
        //we must keep the templated part in the .h file for library users
        medWorkspaceCreator creator = create<workspaceType>;
        return registerWorkspace(identifier,name,description,creator,workspaceType::isUsable);
    }

    /**
     * @brief Registers a medWorkspace type with the factory.
     *
     * This method requires the developer to provide his own function pointer
     * to allocate the workspace memory.
     *
     * @param identifier Identifier of the type.
     * @param name Human readable name (Potentially localised).
     * @param description short description, mainly used for tooltips
     * (Potentially localised).
     * @param creator function pointer allocating memory for the toolbox.
     */
    bool registerWorkspace(QString identifier,
                         QString name,
                         QString description,
                         medWorkspaceCreator creator,
                         medWorkspaceIsUsable isUsable=NULL);

    /**
     * @brief Gives the details of all workspaces.
     *
     */
    QHash<QString, medWorkspaceDetails *> workspaceDetails() const;

    /**
     * @brief Gives the details of one workspace.
     *
     */
    medWorkspaceDetails * workspaceDetailsFromId(QString identifier) const;

    bool isUsable(QString identifier) const;

public slots:
    /**
     * @brief allocates the memory for a medWorkspace.
     * @param type identifier for the Workspace type.
     * @param parent the parentWidget for all the Widget created in the workspace, even if the workspace is not a widget, its children can be destroyed by the qobject hierarchy.
     */
    medWorkspace *createWorkspace(QString type,QWidget* parent=0);

protected:
     medWorkspaceFactory();
    ~medWorkspaceFactory();

private:
    static medWorkspaceFactory *s_instance;
    /**
     * @brief Templated method returning a pointer to an allocated workspace.
     * @see template<class workspaceType> registerWorkspace
     * @warning keep it static if you don't want to freeze your brain (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T >
    static medWorkspace* create ( QWidget* parent ) {
    return ( new T(parent) );
    }

private:
    medWorkspaceFactoryPrivate *d;
};

/**
 * @brief stores the details for a particular workspace,
 * and a function to allocate memory.
 *
 */
struct MEDGUI_EXPORT medWorkspaceDetails{
    QString name; /** Readable name*/
    QString description; /** (tooltip) short description of the workspace */
    medWorkspaceFactory::medWorkspaceCreator creator; /** function pointer allocating memory for the workspace*/
    medWorkspaceFactory::medWorkspaceIsUsable isUsable;
    medWorkspaceDetails(QString name,QString description,medWorkspaceFactory::medWorkspaceCreator creator, medWorkspaceFactory::medWorkspaceIsUsable isUsable = NULL):
        name(name),description(description),creator(creator),isUsable(isUsable){}
};


