#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QGridLayout>
#include <medViewContainerSplitter.h>
#include <medTabbedViewContainers.h>

class medRootContainer : public QWidget
{
    Q_OBJECT
private:
    medViewContainerSplitter *m_poSplitter;
    QGridLayout m_oInternalLayout;
    medTabbedViewContainers *m_poParent;

public:
    medRootContainer(medTabbedViewContainers* parent);
    virtual ~medRootContainer();

    void replaceSplitter(medViewContainerSplitter *pi_poSplitter);
    inline medViewContainerSplitter * getSplitter() const { return m_poSplitter; }
};

