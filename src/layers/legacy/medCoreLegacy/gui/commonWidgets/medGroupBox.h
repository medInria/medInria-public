#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QGroupBox>
#include <QChildEvent>

#include <medCoreLegacyExport.h>

class medGroupBoxPrivate;

/**
 * @brief Extends a QGroupBox to make it collapsible.
 *
 */
class MEDCORELEGACY_EXPORT medGroupBox : public QGroupBox
{
    Q_OBJECT

    Q_PROPERTY(bool collapsible READ isCollapsible WRITE setCollapsible)

public:
    explicit medGroupBox(QWidget *parent = nullptr);
    ~medGroupBox();
    bool isCollapsible();
    void setCollapsible(bool collapsible);
    bool isCollapsed();
    bool isExpanded();

public slots:
    void setCollapsed(bool collapse);
    void setExpanded(bool expand);

protected:
        virtual void childEvent(QChildEvent* event);

private:
    medGroupBoxPrivate * d;
};
