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


#ifndef MEDHOMEPAGEBUTTON_H
#define MEDHOMEPAGEBUTTON_H

#include <QtGui>
#include "medGuiExport.h"

class medHomepagePushButton : public QPushButton
{
    Q_OBJECT
    
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected);
    
public:
    medHomepagePushButton ( QWidget * parent = 0 );

    /**
     * @brief Sets an identifier string for the translated valu displayed in text.
     *
     * This String is used to store the identifier corresponding to a workspace in the button.
     * @see identifier
     */
    void setIdentifier(const QString & id);

    /**
     * @brief Returns the identifier string stored in the button.
     * @see setIdentifier
     */
    QString identifier ();

    bool selected() const;
    bool isSelected() const;

public slots:
    void onClick();
    void setSelected(bool value);

signals:
    /**
     * @brief is emitted when the button is clicked.
     * @param the identifier, or if not present the displayed text.append
     */
    void clicked ( QString text );

private:
    QString id;
    bool m_selected;
};

class MEDGUI_EXPORT medQuickAccessPushButton : public QPushButton
{
    Q_OBJECT

public:
    medQuickAccessPushButton ( QWidget * parent = 0 );

public slots:
    void onClick();

signals:
    void clicked ( QString text );
};

class MEDGUI_EXPORT medHomepageButton : public QToolButton
{
    Q_OBJECT
public:
    medHomepageButton ( QWidget * parent = 0 );

    /**
     * @brief Sets an identifier string for the translated valu displayed in text.
     *
     * This String is used to store the identifier corresponding to a workspace in the button.
     * @see identifier
     */
    void setIdentifier(const QString & id);

    /**
     * @brief Returns the identifier string stored in the button.
     * @see setIdentifier
     */
    QString identifier ();
public slots:
    void onClick();

signals:
    /**
     * @brief is emitted when the button is clicked.
     * @param the identifier, or if not present the displayed text.append
     */
    void clicked ( QString text );

private:
    QString id;

};

#endif // MEDHOMEPAGEBUTTON_H
