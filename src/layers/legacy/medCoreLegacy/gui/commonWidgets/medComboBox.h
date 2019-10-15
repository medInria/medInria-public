/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QComboBox>

#include <medCoreLegacyExport.h>

/**
 * @brief The medComboBox class is based on QComboBox. It deactivates the wheelEvent.
 */
class MEDCORELEGACY_EXPORT medComboBox: public QComboBox
{

    Q_OBJECT

public:

    medComboBox(QWidget* parent = nullptr): QComboBox(parent)
    {
        this->setFocusPolicy(Qt::StrongFocus);
    }
    
    // No wheel event for these QCombobox to avoid changes during a scroll
    void wheelEvent(QWheelEvent*){}
};
