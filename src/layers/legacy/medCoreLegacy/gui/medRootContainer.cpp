/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medRootContainer.h"

medRootContainer::medRootContainer(medTabbedViewContainers* parent) : m_poParent(parent)
{
    m_poSplitter = nullptr;
    setContentsMargins(0, 0, 0, 0);
    m_oInternalLayout.setContentsMargins(0, 0, 0, 0);
    setLayout(&m_oInternalLayout);
}

medRootContainer::~medRootContainer()
{
}

void medRootContainer::replaceSplitter(medViewContainerSplitter *pi_poSplitter)
{
    if (m_poSplitter)
    {
        m_oInternalLayout.removeWidget(m_poSplitter);
        m_poSplitter->setParent(nullptr);
        m_poSplitter->disconnect(m_poParent);
    }

    m_poSplitter = pi_poSplitter;    

    if (m_poSplitter)
    {
        connect(m_poSplitter, SIGNAL(destroyed()), m_poParent, SLOT(closeCurrentTab()));
        m_oInternalLayout.addWidget(m_poSplitter);
        m_poSplitter->setParent(this);
    }
}
