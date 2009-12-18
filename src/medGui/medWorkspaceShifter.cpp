/* medWorkspaceShifter.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 11:11:53 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 11:17:35 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medWorkspaceShifter.h"

// /////////////////////////////////////////////////////////////////
// medWorkspaceShifterPainter
// /////////////////////////////////////////////////////////////////

class medWorkspaceShifterPainter : public QPainter
{
public:
     medWorkspaceShifterPainter(void);
     medWorkspaceShifterPainter(QPaintDevice *device);
    ~medWorkspaceShifterPainter(void);

    static QPainterPath roundRectangle(const QRectF& rect, qreal radius);
    static QPainterPath roundRectangle(const QRectF& rect, qreal leftRadius, qreal rightRadius);
    static QPainterPath roundRectangle(const QRectF& rect, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius);

    void drawRoundRect(const QRectF& rect, qreal radius);
    void drawRoundRect(const QRectF& rect, qreal leftRadius, qreal rightRadius);
    void drawRoundRect(const QRectF& rect, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius);

    void fillRoundRect(const QRectF& rect, qreal radius, const QBrush& brush);
    void fillRoundRect(const QRectF& rect, qreal leftRadius, qreal rightRadius, const QBrush& brush);
    void fillRoundRect(const QRectF& rect, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius, const QBrush& brush);
};

medWorkspaceShifterPainter::medWorkspaceShifterPainter(void) : QPainter()
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
}

medWorkspaceShifterPainter::medWorkspaceShifterPainter(QPaintDevice *device) : QPainter(device)
{
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
}

medWorkspaceShifterPainter::~medWorkspaceShifterPainter(void)
{

}

QPainterPath medWorkspaceShifterPainter::roundRectangle(const QRectF& rect, qreal radius)
{
    return roundRectangle(rect, radius, radius, radius, radius);
}

QPainterPath medWorkspaceShifterPainter::roundRectangle(const QRectF& rect, qreal leftRadius, qreal rightRadius)
{
    return roundRectangle(rect, leftRadius, leftRadius, rightRadius, rightRadius);
}

QPainterPath medWorkspaceShifterPainter::roundRectangle(const QRectF& rect, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius)
{
    QPainterPath path(QPoint(rect.left(), rect.top() + leftTopRadius));
    path.quadTo(rect.left(), rect.top(), rect.left() + leftTopRadius, rect.top());
    path.lineTo(rect.right() - rightTopRadius, rect.top());
    path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + rightTopRadius);
    path.lineTo(rect.right(), rect.bottom() - rightBottomRadius);
    path.quadTo(rect.right(), rect.bottom(), rect.right() - rightBottomRadius, rect.bottom());
    path.lineTo(rect.left() + leftBottomRadius, rect.bottom());
    path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - leftBottomRadius);
    path.closeSubpath();

    return path;
}

void medWorkspaceShifterPainter::drawRoundRect(const QRectF& rect, qreal radius)
{
    drawPath(roundRectangle(rect, radius, radius, radius, radius));
}

void medWorkspaceShifterPainter::drawRoundRect(const QRectF& rect, qreal leftRadius, qreal rightRadius)
{
    drawPath(roundRectangle(rect, leftRadius, leftRadius, rightRadius, rightRadius));
}

void medWorkspaceShifterPainter::drawRoundRect(const QRectF& rect, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius)
{
    drawPath(roundRectangle(rect, leftTopRadius, leftBottomRadius, rightTopRadius, rightBottomRadius));
}

void medWorkspaceShifterPainter::fillRoundRect(const QRectF& rect, qreal radius, const QBrush& brush)
{
    fillPath(roundRectangle(rect, radius, radius, radius, radius), brush);
}

void medWorkspaceShifterPainter::fillRoundRect(const QRectF& rect, qreal leftRadius, qreal rightRadius, const QBrush& brush)
{
    fillPath(roundRectangle(rect, leftRadius, leftRadius, rightRadius, rightRadius), brush);
}

void medWorkspaceShifterPainter::fillRoundRect(const QRectF& rect, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius, const QBrush& brush)
{
    fillPath(roundRectangle(rect, leftTopRadius, leftBottomRadius, rightTopRadius, rightBottomRadius), brush);
}

// /////////////////////////////////////////////////////////////////
// medWorkspaceShifterAction
// /////////////////////////////////////////////////////////////////

class medWorkspaceShifterActionPrivate
{
public:
    bool isHovered;
    bool isChecked;
    QString text;
    QIcon icon;
};

medWorkspaceShifterAction::medWorkspaceShifterAction(QObject *parent) : QObject(parent), d(new medWorkspaceShifterActionPrivate)
{
    d->isHovered = false;
    d->isChecked = false;
}

medWorkspaceShifterAction::medWorkspaceShifterAction(const QString& text, QObject *parent) : QObject(parent), d(new medWorkspaceShifterActionPrivate)
{
    d->isHovered = false;
    d->isChecked = false;
    d->text = text;
}

medWorkspaceShifterAction::medWorkspaceShifterAction(const QIcon& icon, const QString& text, QObject *parent) : QObject(parent), d(new medWorkspaceShifterActionPrivate)
{
    d->isHovered = false;
    d->isChecked = false;
    d->icon = icon;
    d->text = text;
}

medWorkspaceShifterAction::~medWorkspaceShifterAction(void)
{
    delete d;
}

bool medWorkspaceShifterAction::isChecked(void) const
{
    return(d->isChecked);
}

bool medWorkspaceShifterAction::isHovered(void) const
{
    return(d->isHovered);
}

QIcon medWorkspaceShifterAction::icon(void) const
{
    return(d->icon);
}

void medWorkspaceShifterAction::setIcon(const QIcon& icon)
{
    d->icon = icon;
}

QString medWorkspaceShifterAction::text(void) const
{
    return(d->text);
}

void medWorkspaceShifterAction::setText(const QString& text)
{
    d->text = text;
}

void medWorkspaceShifterAction::hover(bool isHovered)
{
    d->isHovered = isHovered;

    if (d->isHovered)
        emit hovered();
}

void medWorkspaceShifterAction::toggle(void)
{
    emit toggled(d->isChecked);
}

void medWorkspaceShifterAction::trigger(void)
{
    emit triggered(d->isChecked);
}

void medWorkspaceShifterAction::setChecked(bool checked)
{
    d->isChecked = checked;
}

// /////////////////////////////////////////////////////////////////
// medWorkspaceShifter
// /////////////////////////////////////////////////////////////////

class medWorkspaceShifterPrivate
{
public:
    QList<medWorkspaceShifterAction *> actionList;
    medWorkspaceShifterAction *checkedAction;
    medWorkspaceShifterAction *hoveredAction;
};

medWorkspaceShifter::medWorkspaceShifter(QWidget *parent) : QWidget(parent), d(new medWorkspaceShifterPrivate)
{
    setMouseTracking(true);

    d->hoveredAction = NULL;
    d->checkedAction = NULL;
}

medWorkspaceShifter::~medWorkspaceShifter(void)
{
    delete d;
}

medWorkspaceShifterAction *medWorkspaceShifter::addAction(medWorkspaceShifterAction *action)
{
    d->actionList.append(action);

    return action;
}

medWorkspaceShifterAction *medWorkspaceShifter::addAction(const QString& text)
{
    medWorkspaceShifterAction *action = new medWorkspaceShifterAction(text, this);
    d->actionList.append(action);

    return action;
}

QSize medWorkspaceShifter::minimumSizeHint(void) const
{
    int itemsWidth = calculateButtonWidth() * d->actionList.size();
    return(QSize(30 + itemsWidth, 28));
}

void medWorkspaceShifter::paintEvent(QPaintEvent *event)
{
    int height = event->rect().height();
    int width = event->rect().width();
    int mh = (height / 2) - 1;

    medWorkspaceShifterPainter p(this);

    QLinearGradient linearGradUp(QPointF(0, 0), QPointF(0, mh));
    linearGradUp.setColorAt(0, QColor(0x59, 0x59, 0x5e));
    linearGradUp.setColorAt(1, QColor(0x46, 0x45, 0x4b));
    p.fillRect(0, 0, width, mh, QBrush(linearGradUp));

    QLinearGradient linearGradDw(QPointF(0, mh), QPointF(0, height));
    linearGradDw.setColorAt(0, QColor(0x2c, 0x2c, 0x2e));
    linearGradDw.setColorAt(1, QColor(0x2c, 0x2c, 0x2e));
    p.fillRect(0, mh, width, mh, QBrush(linearGradDw));

    int buttonWidth = calculateButtonWidth();
    int buttonsWidth = buttonWidth * d->actionList.size();
    int buttonsX = (width / 2) - (buttonsWidth / 2);

    p.translate(0, 4);
    QRect rect(buttonsX, 0, buttonWidth, 20);
    foreach (medWorkspaceShifterAction *action, d->actionList) {
        drawButton(&p, rect, action);
        rect.moveLeft(rect.x() + rect.width());
    }
    p.translate(0, -4);

    p.fillRect(buttonsX, height - 4, buttonsWidth, 1, QColor(0x6d, 0x6d, 0x6d));

    p.end();
}

void medWorkspaceShifter::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    medWorkspaceShifterAction *action = hoveredAction(event->pos());

    if (action == NULL && d->hoveredAction != NULL) {
        d->hoveredAction->hover(false);
        d->hoveredAction = NULL;
        update();
    } else if (action != NULL) {
        d->hoveredAction = action;
        action->hover(true);
        update();
    }
}

void medWorkspaceShifter::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    if (d->hoveredAction != NULL) {
        if (d->checkedAction != NULL)
            d->checkedAction->setChecked(false);

        d->checkedAction = d->hoveredAction;
        d->hoveredAction->setChecked(true);
        d->hoveredAction->trigger();

        update();
    }
}

medWorkspaceShifterAction *medWorkspaceShifter::hoveredAction(const QPoint& pos) const
{
    if (pos.y() <= 4 || pos.y() >= 24)
        return(NULL);

    int buttonWidth = calculateButtonWidth();
    int buttonsWidth = buttonWidth * d->actionList.size();
    int buttonsX = (width() / 2) - (buttonsWidth / 2);
    if (pos.x() <= buttonsX || pos.x() >= (buttonsX + buttonsWidth))
        return(NULL);

    int buttonIndex = (pos.x() - buttonsX) / buttonWidth;
    return(d->actionList[buttonIndex]);
}

int medWorkspaceShifter::calculateButtonWidth(void) const
{
    QFontMetrics fontMetrics(QFont("Arial", 8, QFont::Bold));
    int tmpItemWidth, itemWidth = 0;
    foreach (medWorkspaceShifterAction *action, d->actionList) {
        tmpItemWidth = fontMetrics.width(action->text());
        if (itemWidth < tmpItemWidth) itemWidth = tmpItemWidth;
    }
    return(itemWidth + 20);
}

void medWorkspaceShifter::drawUnselectedButton(QPainter *painter, const QRect& rect, const medWorkspaceShifterAction *action)
{
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, rect.height() / 2));
    linearGrad.setColorAt(0, QColor(0x8e, 0x8e, 0x8e));
    linearGrad.setColorAt(1, QColor(0x5c, 0x5c, 0x5c));
    drawButton(painter, rect, linearGrad, QColor(0x41, 0x41, 0x41), action);
}

void medWorkspaceShifter::drawSelectedButton(QPainter *painter, const QRect& rect, const medWorkspaceShifterAction *action)
{
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, rect.height() / 2));
    linearGrad.setColorAt(0, QColor(0x6d, 0x6d, 0x6d));
    linearGrad.setColorAt(1, QColor(0x25, 0x25, 0x25));
    drawButton(painter, rect, linearGrad, QColor(0x00, 0x00, 0x00), action);
}

void medWorkspaceShifter::drawButton(QPainter *painter, const QRect& rect, const medWorkspaceShifterAction *action)
{
    if (action->isChecked())
        drawSelectedButton(painter, rect, action);
    else
        drawUnselectedButton(painter, rect, action);
}

void medWorkspaceShifter::drawButton(QPainter *painter, const QRect& rect, const QLinearGradient& gradient, const QColor& color, const medWorkspaceShifterAction *action)
{
    painter->save();

    int height = rect.height();
    int width = rect.width();
    int mh = (height / 2) - 2;

    painter->translate(rect.x(), rect.y());
    painter->setPen(QColor(0x28, 0x28, 0x28));

    painter->fillRect(0, 0, width, mh, QBrush(gradient));
    painter->fillRect(0, mh, width, mh, color);
    painter->drawRect(0, 0, width, height);

    painter->setFont(QFont("Arial", 8, QFont::Bold));
    painter->setPen(QPen(QColor(0xff, 0xff, 0xff), 1));
    painter->drawText(0, -1, width, height, Qt::AlignCenter, action->text());

    painter->restore();
}
