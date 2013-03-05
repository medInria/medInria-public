/*
    Copyright (c) 2011,  Alexandre Abadie <Alexandre.Abadie@irisa.fr>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Alexandre <email> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Alexandre <email> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "medHomepageButton.h"

//medHomepagePushButton in the quick access menu
medQuickAccessPushButton::medQuickAccessPushButton ( QWidget * parent )
{
    QObject::connect ( this, SIGNAL ( clicked() ), this, SLOT ( onClick() ) );
}

void medQuickAccessPushButton::onClick()
{
    emit clicked (this->text());
}

medHomepagePushButton::medHomepagePushButton ( QWidget * parent ) : QPushButton ( parent )
{
    QObject::connect ( this, SIGNAL ( clicked() ), this, SLOT ( onClick() ) );
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
