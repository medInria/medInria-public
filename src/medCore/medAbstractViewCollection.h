/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medAbstractView.h"

#include "medCoreExport.h"

class medAbstractView;
class medAbstractViewCollectionPrivate;

/**
* @class medAbstractViewCollection
* @author Pierre Fillard
* @brief Extends medAbstractView to carry a collection of views instead of one (container class).
*
* This class is a simple extension of medAbtractView to make it able to carry more than
* one view at a time. It might be desirable when:
* - data contains more than one actual data (such as medAbstractDataCollection)
*
* The method views() must be reimplemented in subclass to return the list of views the object
* is carrying. Morevoer, the signals viewAdded() and viewRemoved() must be called when a
* new view is created / deleted.
*
* Note that this class works in cunjunction with a medViewPool. When passed to a pool, all views
* will automatically be added instead of the view itself. The pool listens to the signal viewAdded()
* and viewRemoved() to maintain its internal list of views up-to-date.
*/
class MEDCORE_EXPORT medAbstractViewCollection : public medAbstractView
{
    Q_OBJECT

public:
             medAbstractViewCollection (medAbstractViewCollection * parent = 0);
             medAbstractViewCollection (const medAbstractViewCollection& view);
    virtual ~medAbstractViewCollection();

    /**
    * Returns the list of view carried by the view. Must be reimplemented in subclass.
    */
    virtual QList <medAbstractView*> views() const;

signals:
    void viewAdded   (medAbstractView *);
    void viewRemoved (medAbstractView *);

protected:

private:
    medAbstractViewCollectionPrivate *d;
};


