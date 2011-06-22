/*
    Copyright (c) 2011, Alexandre <email>
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

medHomepageButton::medHomepageButton(QWidget * parent) : QToolButton(parent)
{
    this->setStyleSheet("QToolButton{border: 0px; \
                border-left: 1px solid #4b4b4b; \
                border-radius: 0px; \
                background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #5e5e5e, stop: 1 #2b2b2b); \
                color: #b2b8b2; \
                font-size: 10px; \
                height: 16px; \
                padding-left: 1px; \
                padding-right: 1px; \
                min-width: 2px;}\
                QToolButton:pressed, QToolButton:checked {\
                background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #282828, stop: 1 #4a4a4a);\
}");
    QObject::connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void medHomepageButton::onClick ( void )
{
    emit clicked(this->text());
}

