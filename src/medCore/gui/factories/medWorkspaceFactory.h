/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <dtkCoreSupport/dtkAbstractFactory.h>

#include <QtCore>

class medAbstractWorkspace;
class medWorkspaceFactoryPrivate;

class MEDCORE_EXPORT medWorkspaceFactory : public dtkAbstractFactory
{
    Q_OBJECT

public:
    typedef medAbstractWorkspace *(*medWorkspaceCreator)(QWidget* parent);
    typedef bool (*medWorkspaceIsUsable)();

    struct Details
    {
        QString identifier;
        QString name;
        QString description;
        QString category;
        medWorkspaceCreator creator;
        medWorkspaceIsUsable isUsable;
        Details(QString id_,
                QString name_,
                QString description_,
                QString category_,
                medWorkspaceCreator creator_,
                medWorkspaceIsUsable isUsable_ = NULL)
            : identifier(id_)
            , name(name_)
            , description(description_)
            , category(category_)
            , creator(creator_)
            , isUsable(isUsable_)
        {}
    };

public:
    static medWorkspaceFactory* instance();

    QList<QString> workspaces();

    /**
     * @brief Registers a medAbstractWorkspace type with the factory.
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
    bool registerWorkspace(){
        //we must keep the templated part in the .h file for library users
        medWorkspaceCreator creator = create<workspaceType>;
        return registerWorkspace(workspaceType::staticIdentifier(),
                                 workspaceType::staticName(),
                                 workspaceType::staticDescription(),
                                 workspaceType::staticCategory(),
                                 creator,
                                 workspaceType::isUsable);
    }


    QHash<QString, medWorkspaceFactory::Details *> workspaceDetails() const;
    QList<medWorkspaceFactory::Details *> workspaceDetailsSortedByName() const;
    medWorkspaceFactory::Details * workspaceDetailsFromId(QString identifier) const;

    bool isUsable(QString identifier) const;

    QList<QString> workspacesFromCategory(const QString& category) const;

public slots:

    medAbstractWorkspace* createWorkspace(QString type,QWidget* parent=0);

protected:

    /**
     * @brief Registers a medAbstractWorkspace type with the factory.
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
                           QString category,
                           medWorkspaceCreator creator,
                           medWorkspaceIsUsable isUsable=NULL);

     medWorkspaceFactory();
    virtual ~medWorkspaceFactory();

private:
    static medWorkspaceFactory *s_instance;
    /**
     * @brief Templated method returning a pointer to an allocated workspace.
     * @see template<class workspaceType> registerWorkspace
     * @warning keep it static if you don't want to freeze your brain (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
     */
    template < typename T >
    static medAbstractWorkspace* create ( QWidget* parent ) {
    return ( new T(parent) );
    }

private:
    medWorkspaceFactoryPrivate *d;
};
