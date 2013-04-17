/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medHomepageButton.h"

//medHomepagePushButton in the quick access menu
medQuickAccessPushButton::medQuickAccessPushButton ( QWidget * parent )
{
    QObject::connect ( this, SIGNAL ( clicked() ), this, SLOT ( onClick() ) );
}

void medQuickAccessPushButton::onClick(void)
{
    emit clicked (this->text());
}

medHomepagePushButton::medHomepagePushButton ( QWidget * parent ) : QPushButton ( parent )
{
    QObject::connect ( this, SIGNAL ( clicked() ), this, SLOT ( onClick() ) );
    m_selected = false;
    
    this->setMouseTracking(true);
}

void medHomepagePushButton::onClick()
{
    emit clicked ( this->identifier() );
}

medHomepageButton::medHomepageButton ( QWidget * parent ) : QToolButton ( parent )
{
    QObject::connect ( this, SIGNAL ( clicked() ), this, SLOT ( onClick() ) );
}

void medHomepageButton::onClick()
{
    emit clicked ( this->identifier() );
}

QString medHomepagePushButton::identifier()
{
    if (this->id.isEmpty())
        return this->text();
    else
        return this->id;
}

bool medHomepagePushButton::selected() const
{
    return m_selected;
}

bool medHomepagePushButton::isSelected() const
{
    return m_selected;
}

void medHomepagePushButton::setSelected(bool value)
{
    m_selected = value;
    this->style()->unpolish(this);
    this->style()->polish(this);
}

QString medHomepageButton::identifier()
{
    if (this->id.isEmpty())
        return this->text();
    else
        return this->id;
}

void medHomepagePushButton::setIdentifier(const QString & identifier)
{
    this->id = identifier;
}

void medHomepageButton::setIdentifier(const QString & identifier)
{
    this->id = identifier;
}
