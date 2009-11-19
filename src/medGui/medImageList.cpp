/* medImageList.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct 16 11:41:28 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct 16 12:05:49 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medImageList.h"

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
    
    painter->setPen(Qt::lightGray);
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
    badgeGradient.setColorAt(0.0, QColor(0xf8, 0x1b, 0x17));
    badgeGradient.setColorAt(1.0, QColor(0x5b, 0x02, 0x03));
            
    QColor shadowColor(0xff, 0xff, 0xff, h);
    drawRoundedRect(painter, x + 1, y, w, h, shadowColor);
    drawRoundedRect(painter, x - 1, y, w, h, shadowColor);
    drawRoundedRect(painter, x, y + 1, w, h, shadowColor);
    drawRoundedRect(painter, x, y - 1, w, h, shadowColor);

    painter->setPen(QPen(Qt::white, 2));
    painter->setBrush(badgeGradient);
    painter->drawRoundedRect(x, y, w-4, h-4, 8.0, 8.0);

    painter->setPen(QPen(Qt::white, 1));
    painter->drawText(x, y, w - 3, h - 3, Qt::AlignCenter, text);
    
    painter->restore();
}

static void drawOneImage(QPainter *painter, const QPixmap& pixmap1)
{
    painter->save();
    
    QColor shadowColor(0x00, 0x00, 0x00, THUMB_SHADOW);
    painter->translate(8, 8);
    
    drawImageShadow(painter,  0, 0,  10, shadowColor);
    drawImage(painter,  0,  0,  10, pixmap1);
    
    painter->restore();
}

static void drawTwoImages(QPainter *painter, const QPixmap& pixmap1, const QPixmap& pixmap2)
{
    painter->save();
    
    QColor shadowColor(0x00, 0x00, 0x00, THUMB_SHADOW);
    painter->translate(8, 8);
    
    drawImageShadow(painter, THUMB_STEP1, 0, -10, shadowColor);
    drawImage(painter, THUMB_STEP1,  0, -10, pixmap2);
    
    drawImageShadow(painter,  0, 0,  10, shadowColor);
    drawImage(painter,  0,  0,  10, pixmap1);
    
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

static QPixmap loadImage(const QString& name)
{
    //QPixmap pixmap(QString(":/pixmaps/%1.jpg").arg(name));
    QPixmap pixmap(name);

    return pixmap.scaled(THUMB_SIZE, THUMB_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

// /////////////////////////////////////////////////////////////////
// medImageList
// /////////////////////////////////////////////////////////////////

class medImageListPrivate
{
public:
    QMap<int, QString> names;
    QMap<int, QString> pixNames;
    QMap<int, int> sizes;
};

medImageList::medImageList(QWidget *parent) : QWidget(parent), d(new medImageListPrivate)
{

}

medImageList::~medImageList(void)
{
    delete d;
    
    d = NULL;
}

void medImageList::clear(void)
{
    d->names.clear();
    d->sizes.clear();
}

void medImageList::setListName(int list, QString name)
{
    d->names.insert(list, name);
}

void medImageList::setPixName(int list, QString name)
{
    d->pixNames.insert(list, name);
}

void medImageList::setListSize(int list, int size)
{
    d->sizes.insert(list, size);
}

void medImageList::paintEvent(QPaintEvent *event)
{
    QPainter p;
    p.begin(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::Antialiasing);
    p.setFont(QFont(p.font().family(), 11, QFont::Bold));

    for(int i = 0; i < d->sizes.count(); i++) {
        p.save();
        p.translate(10 + i*160, 10);
        //drawOneImage(&p, loadImage("1"));
	if (d->pixNames.value(i)!=QString())
	    drawOneImage(&p, loadImage(d->pixNames.value(i)));
	else
	    drawOneImage(&p, loadImage(":/pixmaps/1.jpg"));
        drawShadowedText(&p, 40, 100, d->names.value(i));
        drawBadge(&p, 0, 0, QString::number(d->sizes.value(i)));
        p.restore();
    }
    
    p.end();
}
