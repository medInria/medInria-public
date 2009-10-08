/* medImageStack.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 20:03:17 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 21:18:01 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 30
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medImageStack.h"

#include <QtCore>
#include <QtGui>

// /////////////////////////////////////////////////////////////////
// Helper functions
// /////////////////////////////////////////////////////////////////

#define THUMB_SHADOW_TEXT (48)
#define THUMB_SHADOW	  (64)
#define THUMB_SIZE	  (70)
#define THUMB_HALF_SIZE	  (35)
#define THUMB_STEP1	  (19)
#define THUMB_STEP2	  (38)
#define THUMB_NUMBER	   (3)

static void drawImage(QPainter *painter, int x, int y, int angle, const QPixmap& pixmap)
{
    painter->save();
    painter->translate(x, y);
    painter->translate(THUMB_HALF_SIZE, THUMB_HALF_SIZE);
    painter->rotate(angle);
    painter->translate(-THUMB_HALF_SIZE, -THUMB_HALF_SIZE);
    painter->drawPixmap(0, 0, THUMB_SIZE, THUMB_SIZE, pixmap);
    painter->restore();
}

static void drawShadow(QPainter *painter, int x, int y, int angle, const QBrush& brush)
{
    painter->save();
    painter->translate(x, y);
    painter->translate(THUMB_HALF_SIZE, THUMB_HALF_SIZE);
    painter->rotate(angle);
    painter->translate(-THUMB_HALF_SIZE, -THUMB_HALF_SIZE);
    painter->fillRect(0, 0, THUMB_SIZE, THUMB_SIZE, brush);
    painter->restore();
}

static void drawImageShadow(QPainter *painter, int x, int y, int angle, const QBrush& color)
{
    drawShadow(painter, x + 2, y,  angle, color);
    drawShadow(painter, x - 2, y,  angle, color);
    drawShadow(painter, x, y + 2,  angle, color);
    drawShadow(painter, x, y - 2,  angle, color);
}

static void drawShadowedText(QPainter *painter, int x, int y, const QString& text)
{
    painter->save();
    
    painter->setPen(QColor(0x00, 0x00, 0x00, THUMB_SHADOW_TEXT));
    painter->drawText(x + 1, y, text);
    painter->drawText(x - 1, y, text);
    painter->drawText(x, y + 1, text);
    painter->drawText(x, y - 1, text);
    
    painter->setPen(Qt::black);
    painter->drawText(x, y, text);
    
    painter->restore();
}

static void drawRoundedRect(QPainter *painter, int x, int y, int w, int h, const QBrush& brush)
{
    painter->setBrush(brush);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(x, y, w, h, 6.0, 6.0);
}

static void drawBadge(QPainter *painter, int x, int y, const QString& text)
{
    painter->save();    
    painter->setFont(QFont(painter->font().family(), 10, QFont::Bold));
    
    QFontMetrics fMetrics = painter->fontMetrics();
    int w = fMetrics.width(text) + 12;
    int h = fMetrics.xHeight() + 12;
    
    QRadialGradient badgeGradient(0.0, 0.0, 17.0, 30 - 3, 25 - 3);
    badgeGradient.setColorAt(0.0, QColor(0x80, 0x84, 0xb));
    badgeGradient.setColorAt(1.0, QColor(0x0c, 0x0c, 0x00));
            
    QColor shadowColor(0x00, 0x00, 0x00, h);
    drawRoundedRect(painter, x + 1, y, w, h, shadowColor);
    drawRoundedRect(painter, x - 1, y, w, h, shadowColor);
    drawRoundedRect(painter, x, y + 1, w, h, shadowColor);
    drawRoundedRect(painter, x, y - 1, w, h, shadowColor);

    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(badgeGradient);
    painter->drawRoundedRect(x, y, w - 3, h - 3, 6.0, 6.0);

    painter->setPen(QPen(Qt::white, 1));
    painter->drawText(x, y, w - 3, h - 3, Qt::AlignCenter, text);
    
    painter->restore();
}

static void drawThreeImages(QPainter *painter, const QPixmap& pixmap1, const QPixmap& pixmap2, const QPixmap& pixmap3)
{
    painter->save();
    
    QColor shadowColor(0x00, 0x00, 0x00, THUMB_SHADOW);
    painter->translate(8, 8);
    
    drawImageShadow(painter, THUMB_STEP2, 0,   5, shadowColor);
    drawImage(painter, THUMB_STEP2,  0,   5, pixmap3);
    
    drawImageShadow(painter, THUMB_STEP1, 0, -10, shadowColor);
    drawImage(painter, THUMB_STEP1,  0, -10, pixmap2);
    
    drawImageShadow(painter,  0, 0,  10, shadowColor);
    drawImage(painter,  0,  0,  10, pixmap1);
    
    painter->restore();
}

QPixmap loadImage(const QString& name)
{
    QPixmap pixmap(QString(":/img/%1.jpg").arg(name));

    return pixmap.scaled(THUMB_SIZE, THUMB_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

// /////////////////////////////////////////////////////////////////
// medImageStack
// /////////////////////////////////////////////////////////////////

class medImageStackPrivate
{
public:
};

medImageStack::medImageStack(QWidget *parent) : QWidget(parent), d(new medImageStackPrivate)
{

}

medImageStack::~medImageStack(void)
{
    delete d;
    
    d = NULL;
}

void medImageStack::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::Antialiasing);
    
    p.setFont(QFont(p.font().family(), 11, QFont::Bold));
    
    p.save();
    p.translate(10, 10);
    drawBadge(&p, 0, 0, "3");
    drawThreeImages(&p, loadImage("1"), loadImage("2"), loadImage("3"));
    drawShadowedText(&p, 40, 100, "Study 1");
    p.restore();
    
    p.save();
    p.translate(160, 10);
    drawThreeImages(&p, loadImage("2"), loadImage("3"), loadImage("1"));
    drawShadowedText(&p, 40, 100, "Study 2");
    p.restore();
    
    p.save();
    p.translate(10, 130);
    drawThreeImages(&p, loadImage("4"), loadImage("3"), loadImage("5"));
    drawShadowedText(&p, 40, 100, "Study 3");
    p.restore();
    
    p.save();
    p.translate(160, 130);
    drawThreeImages(&p, loadImage("5"), loadImage("4"), loadImage("3"));
    drawShadowedText(&p, 40, 100, "Study 4");
    p.restore();
    
    p.end();
}
