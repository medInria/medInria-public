/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medWorkspace.h>


class medAbstractView;
class QGraphicsSceneMouseEvent;
class medProgressionStack;
class medSegmentationWorkspacePrivate;
class medSegmentationSelectorToolBox;

/**
 * class medSegmentationWorkspace
 * Defines the segmentation workspace.
 */
class medSegmentationWorkspace : public medWorkspace
{
    Q_OBJECT

public:
    medSegmentationWorkspace(QWidget * parent = NULL);

    virtual ~medSegmentationWorkspace();

    //! Overrides base class.
    QString description() const;
    //! Implement base class
    QString identifier() const;

    static bool isUsable();

    //! Implement abstract method in base class.
    void setupViewContainerStack();

    //! Register with factory, return true if successful.
    static bool registerWithViewerWorkspaceFactory();


    //!
    medProgressionStack * progressionStack();

    medSegmentationSelectorToolBox * segmentationToobox();

    void buildWorkspace(  );

    

signals:
    void viewAdded(medAbstractView* view);
    void viewRemoved(medAbstractView* view);

public slots:
    void onViewAdded(medAbstractView* view);
    void onViewRemoved(medAbstractView* view);

    /**@brief Connects toolboxes to the current container
     * @param name the container name */
    virtual void connectToolboxesToCurrentContainer(const QString &name);


private:
    static QString s_identifier();
    medSegmentationWorkspacePrivate *d;
};


