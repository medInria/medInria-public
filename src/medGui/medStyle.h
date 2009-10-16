/* medStyle.h ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Jul  3 19:20:07 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 14 18:32:00 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 60
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef medStyle_H
#define medStyle_H

#include <QtGui>

class medStylePrivate;

class medStyle : public QCleanlooksStyle
{
    Q_OBJECT

public:
    medStyle(void);
   ~medStyle(void);

   void drawPrimitive    (PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;
   void drawControl        (ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;
   void drawComplexControl (ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = 0) const;

   void polish(QWidget *widget);
   void polish(QApplication *application);
   void polish(QPalette &palette);
   
   void unpolish(QWidget *widget);
   void unpolish(QApplication *application);
   
   QPalette standardPalette(void) const;

public:
   static void medDrawRect(QPainter *painter, const QRect& rect, QColor borderColor);

   static void medDrawRectWithColor(QPainter *painter, const QRect& rect, QColor fillColor, QColor borderColor);
   static void medFillRectWithColor(QPainter *painter, const QRect& rect, QColor fillColor);

   static void medDrawRoundRectWithColor(QPainter *painter, const QRectF& rect, QColor fillColor, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius, QColor borderColor);
   static void medFillRoundRectWithColor(QPainter *painter, const QRectF& rect, QColor fillColor, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius);

   static void medDrawRectWithDoubleLinearGradient(QPainter *painter, const QRect& rect, QColor upStart, QColor upStop, QColor dwStart, QColor dwStop, QColor borderColor);
   static void medFillRectWithDoubleLinearGradient(QPainter *painter, const QRect& rect, QColor upStart, QColor upStop, QColor dwStart, QColor dwStop);

   static void medDrawRoundRectWithDoubleLinearGradient(QPainter *painter, const QRectF& rect, QColor upStart, QColor upStop, QColor dwStart, QColor dwStop, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius, QColor borderColor);
   static void medFillRoundRectWithDoubleLinearGradient(QPainter *painter, const QRectF& rect, QColor upStart, QColor upStop, QColor dwStart, QColor dwStop, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius);

private:
   medStylePrivate *d;
};

#endif
