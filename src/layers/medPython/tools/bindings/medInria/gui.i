
%{
#include <medAbstractWorkspaceLegacy.h>
#include <medDropSite.h>
#include <medTabbedViewContainers.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medWorkspaceFactory.h>
%}

%rename(DropSite) medDropSite;
%include "medDropSite.h"

SIGNAL(medDropSite, objectDropped, medDataIndex)

%rename(ViewContainer) medViewContainer;
%include "medViewContainer.h"

%rename(TabbedViewContainers) medTabbedViewContainers;
%include "medTabbedViewContainers.h"

%ignore medToolBoxDetails;

%feature("director") medToolBox;
%rename(ToolBox) medToolBox;
%include "medToolBox.h"

%pythoncode
%{

    def toolbox_interface(name, description, categories):
        def decorator_toolBox(cls):
            cls.staticIdentifier = classmethod(lambda _cls : sys.modules[cls.__module__].__package__ + '.' + cls.__name__)
            cls.staticName = staticmethod(lambda : name)
            cls.staticDescription = staticmethod(lambda : description)
            cls.staticCategories = staticmethod(lambda : categories)
            cls.identifier = lambda self : self.staticIdentifier()
            cls.name = lambda self : self.staticName()
            cls.description = lambda self : self.staticDescription()
            cls.categories = lambda self : self.staticCategories()
            return cls
        return decorator_toolBox

%}

%rename(ToolBoxFactory) medToolBoxFactory;
%include "medToolBoxFactory.h"

%extend medToolBoxFactory
{

    bool _registerToolBox(QString identifier, QString name, QString description, QList<QString> categories, PyObject* nativeClass)
    {
        auto creator = [](QWidget* parent, void* _nativeClass) -> medToolBox*
        {
            return med::python::Object::borrowed((PyObject*)_nativeClass)(parent).cast<medToolBox>();
        };

        return $self->registerToolBox(identifier, name, description, categories, (medToolBoxFactory::medToolBoxCreator)creator, nativeClass);
    }

    %pythoncode
    %{
        def registerToolBox(self, toolBoxClass):
            return self._registerToolBox(toolBoxClass.staticIdentifier(),
                                         toolBoxClass.staticName(),
                                         toolBoxClass.staticDescription(),
                                         toolBoxClass.staticCategories(),
                                         toolBoxClass)
    %}

}

%feature("director") medAbstractWorkspaceLegacy;
%rename(Workspace) medAbstractWorkspaceLegacy;
%include "medAbstractWorkspaceLegacy.h"

%pythoncode
%{

    def workspace_interface(name, description, category):
        def decorator_workspace(cls):
            cls.staticIdentifier = classmethod(lambda _cls : sys.modules[cls.__module__].__package__ + '.' + cls.__name__)
            cls.staticName = staticmethod(lambda : name)
            cls.staticDescription = staticmethod(lambda : description)
            cls.staticCategory = staticmethod(lambda : category)
            cls.identifier = lambda self : self.staticIdentifier()
            cls.name = lambda self : self.staticName()
            cls.description = lambda self : self.staticDescription()
            cls.category = lambda self : self.staticCategory()
            return cls
        return decorator_workspace

%}

%rename(WorkspaceFactory) medWorkspaceFactory;
%include "medWorkspaceFactory.h"

%extend medWorkspaceFactory
{

    bool _registerWorkspace(QString identifier, QString name, QString description, QString category, PyObject* nativeClass)
    {
        auto creator = [](QWidget* parent, void* _nativeClass) -> medAbstractWorkspaceLegacy*
        {
            return med::python::Object::borrowed((PyObject*)_nativeClass)(parent).cast<medAbstractWorkspaceLegacy>();
        };

        return $self->registerWorkspace(identifier, name, description, category, (medWorkspaceFactory::medWorkspaceCreator)creator, nativeClass, nullptr, true);
    }

    %pythoncode
    %{
        def registerWorkspace(self, workspaceClass):
            return self._registerWorkspace(workspaceClass.staticIdentifier(),
                                           workspaceClass.staticName(),
                                           workspaceClass.staticDescription(),
                                           workspaceClass.staticCategory(),
                                           workspaceClass)
    %}

}
