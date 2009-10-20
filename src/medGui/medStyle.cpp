/* medStyle.cpp ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Jul  3 19:21:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Oct 19 16:44:22 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 630
 */

/* Commentary: 
 * - Splitter
 * - Dock
 */

/* Change log:
 * 
 */

#include "medStyle.h"

// /////////////////////////////////////////////////////////////////
// medStylePrivate
// /////////////////////////////////////////////////////////////////

class medStylePrivate
{
public:
    medStylePrivate(void);

public:
    QColor base;
    QColor border;
    QColor dark;
    QColor gradientUp;
    QColor gradientMd;
    QColor gradientDw;
    QColor highlight;
    QColor highlightedText;
    QColor light;
    QColor mid;
    QColor midlight;
    QColor shadow;
    QColor window;
    QColor windowText;
    QColor brightText;
    QColor link;
    QColor linkVisited;
    QColor tooltipbase;
    QColor tooltiptext;
};

medStylePrivate::medStylePrivate(void)
{
    base            = QColor(0x31, 0x31, 0x31).lighter(150); // window.lighter(150)
    border          = QColor(0x1e, 0x1e, 0x1e);
    brightText      = QColor(0x00, 0x66, 0xca);
    gradientUp      = QColor(0x4b, 0x4b, 0x4b);
    gradientMd      = QColor(0x2e, 0x2e, 0x2e);
    gradientDw      = QColor(0x4b, 0x4b, 0x4b);
    highlight       = QColor(0x00, 0x66, 0xca);
    highlightedText = QColor(0xe4, 0xe4, 0xe4);
    light           = QColor(0x2e, 0x2e, 0x2e);
    dark            = QColor(0x21, 0x21, 0x21);
    mid             = QColor(0x24, 0x24, 0x24);
    midlight        = QColor(0x24, 0x24, 0x24);
    shadow          = QColor(0x21, 0x21, 0x21);
    tooltipbase     = QColor(0x00, 0x66, 0xca);
    tooltiptext     = QColor(0x21, 0x21, 0x21);
    link            = QColor(Qt::blue);
    linkVisited     = QColor(Qt::magenta);
    window          = QColor(0x31, 0x31, 0x31);
    windowText      = QColor(0xa8, 0xa8, 0xa8);
}

// /////////////////////////////////////////////////////////////////
// medStyle
// /////////////////////////////////////////////////////////////////

medStyle::medStyle(void) : QCleanlooksStyle(), d(new medStylePrivate)
{
    setObjectName(QLatin1String("medStyle"));
}

medStyle::~medStyle(void)
{

}

void medStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case PE_FrameLineEdit: {
            bool isEnabled = option->state & State_Enabled;
            bool hasFocus = option->state & State_HasFocus;

            QRect rect = option->rect.adjusted(0, 0, -1, -1);

            if (isEnabled) {
                medDrawRectWithColor(painter, rect, d->base, d->border);
            } else {
                medDrawRectWithColor(painter, rect, d->window, d->border);
            }

            if (isEnabled && hasFocus) {
                painter->save();
                painter->setPen(d->highlight);
                painter->drawRoundedRect(rect.adjusted(1, 1, -1, -1), 2, 2);
                painter->restore();
            }
        }
        break;
    case PE_Frame:
    case PE_FrameDefaultButton:
    case PE_FrameFocusRect:
        // Do not draw the focus frame !
        break;
    case PE_IndicatorCheckBox: {
            bool on = false;
            bool sunken = false;
            bool enabled = false;
            bool unchanged = false;
            bool hasFocus = option->state & State_HasFocus;

            if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
                on = button->state & State_On;
                sunken = button->state & State_Sunken;
                enabled = button->state & State_Enabled;
                unchanged = button->state & State_NoChange;
            }

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            // exterior square
            hasFocus ? painter->setPen(d->highlight) : painter->setPen(d->border);
            sunken ? painter->setBrush(d->base.darker()) : painter->setBrush(d->base);
            painter->drawRect(QRectF(option->rect));

            // interior square
            if (on || (enabled && sunken) || unchanged) {
                painter->setPen(Qt::NoPen);
                unchanged ? painter->setBrush(d->windowText.darker(150)) : painter->setBrush(d->windowText);
                painter->drawRect(QRectF(option->rect).adjusted(3, 3, -3, -3));
            }

            painter->restore();
        }
        break;
    case PE_IndicatorRadioButton: {
            bool on = false;
            bool sunken = false;
            bool enabled = false;
            bool hasFocus = option->state & State_HasFocus;

            if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
                on = button->state & State_On;
                sunken = button->state & State_Sunken;
                enabled = button->state & State_Enabled;
            }

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            // exterior ellipse
            if(hasFocus)
                painter->setPen(d->highlight);
            else
                painter->setPen(d->border);
            if(sunken)
                painter->setBrush(d->base.darker());
            else
                painter->setBrush(d->base);
            painter->drawEllipse(QRectF(option->rect));

            // interior ellipse
            if (on || (enabled && sunken)) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(d->windowText);
                painter->drawEllipse(QRectF(option->rect).adjusted(3, 3, -3, -3));
            }

            painter->restore();
        }
        break;
    case PE_PanelButtonCommand: {
            bool isDefault = false;
            bool isFlat = false;
            bool isDown = (option->state & State_Sunken) || (option->state & State_On);
            bool isEnabled = option->state & State_Enabled;
            bool isHovered = option->state & State_MouseOver;
            bool hasFocus = option->state & State_HasFocus;

            if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton*>(option)) {
                isDefault = (button->features & QStyleOptionButton::DefaultButton) && (button->state & State_Enabled);
                isFlat = (button->features & QStyleOptionButton::Flat);
            }

            QRect rect = option->rect.adjusted(0, 0, -1, -1);

            if (isEnabled && isDown) {
                medDrawRoundRectWithDoubleLinearGradient(painter, rect, d->gradientUp.darker(150), d->gradientMd.darker(150), d->gradientMd.darker(150), d->gradientDw.darker(150), 4, 4, 4, 4, d->border);
            } else if (isEnabled && isHovered) {
                medDrawRoundRectWithDoubleLinearGradient(painter, rect, d->gradientUp.lighter(110), d->gradientMd.lighter(110), d->gradientMd.lighter(110), d->gradientDw.lighter(110), 4, 4, 4, 4, d->border);
            } else if (isEnabled) {
                medDrawRoundRectWithDoubleLinearGradient(painter, rect, d->gradientUp, d->gradientMd, d->gradientMd, d->gradientDw, 4, 4, 4, 4, d->border);
            } else {
                medDrawRoundRectWithDoubleLinearGradient(painter, rect, d->gradientUp.darker(120), d->gradientMd.darker(120), d->gradientMd.darker(120), d->gradientDw.darker(120), 4, 4, 4, 4, d->border);
            }

            if (isEnabled && hasFocus) {
                painter->save();
                painter->setPen(d->highlight);
                painter->drawLine(rect.topLeft().x()+2, rect.topLeft().y()+1, rect.topRight().x()-1, rect.topRight().y()+1);
                painter->drawLine(rect.bottomLeft().x()+2, rect.bottomLeft().y(), rect.bottomRight().x()-1, rect.bottomRight().y());
                painter->restore();
            }
        }
        break;
    case PE_PanelStatusBar:
        medDrawRectWithDoubleLinearGradient(painter, option->rect.adjusted(-1, 0, 0, 0), d->gradientUp, d->gradientMd, d->gradientMd, d->gradientDw, d->border);
        break;
    default:
        QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
        break;
    }
}

void medStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case CE_HeaderSection:
        medFillRectWithDoubleLinearGradient(painter, option->rect.adjusted(0, 0, -1, -1), d->gradientUp, d->gradientMd, d->gradientMd, d->gradientDw);
        break;
    case CE_ToolBar:
        painter->save();
        if (const QStyleOptionToolBar *toolbar = qstyleoption_cast<const QStyleOptionToolBar *>(option)) {
            QRect rect = option->rect;

            bool paintLeftBorder = true;
            bool paintRightBorder = true;
            bool paintBottomBorder = true;

            switch (toolbar->toolBarArea) {
            case Qt::BottomToolBarArea:
                switch(toolbar->positionOfLine) {
                case QStyleOptionToolBar::Beginning:
                case QStyleOptionToolBar::OnlyOne:
                    paintBottomBorder = false;
                default:
                    break;
                }
            case Qt::TopToolBarArea:
                switch (toolbar->positionWithinLine) {
                case QStyleOptionToolBar::Beginning:
                    paintLeftBorder = false;
                    break;
                case QStyleOptionToolBar::End:
                    paintRightBorder = false;
                    break;
                case QStyleOptionToolBar::OnlyOne:
                    paintRightBorder = false;
                    paintLeftBorder = false;
                default:
                    break;
                }
                if (QApplication::layoutDirection() == Qt::RightToLeft) {
                    bool tmp = paintLeftBorder;
                    paintRightBorder=paintLeftBorder;
                    paintLeftBorder=tmp;
                }
                break;
            case Qt::RightToolBarArea:
                switch (toolbar->positionOfLine) {
                case QStyleOptionToolBar::Beginning:
                case QStyleOptionToolBar::OnlyOne:
                    paintRightBorder = false;
                    break;
                default:
                    break;
                }
                break;
            case Qt::LeftToolBarArea:
                switch (toolbar->positionOfLine) {
                case QStyleOptionToolBar::Beginning:
                case QStyleOptionToolBar::OnlyOne:
                    paintLeftBorder = false;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }

            medFillRectWithDoubleLinearGradient(painter, rect, d->gradientUp, d->gradientMd, d->gradientMd, d->gradientDw);

            painter->setPen(QPen(d->light));
            painter->drawLine(rect.topLeft().x(), rect.topLeft().y(), rect.topRight().x(), rect.topRight().y());

            if (paintLeftBorder) {
                painter->setPen(QPen(d->light));
                painter->drawLine(rect.topLeft().x(), rect.topLeft().y(), rect.bottomLeft().x(), rect.bottomLeft().y());
            }

            if (paintRightBorder) {
                painter->setPen(QPen(d->shadow));
                painter->drawLine(rect.topRight().x(), rect.topRight().y(), rect.bottomRight().x(), rect.bottomRight().y());
            }

            if (paintBottomBorder) {
                painter->setPen(QPen(d->shadow));
                painter->drawLine(rect.bottomLeft().x(), rect.bottomLeft().y(), rect.bottomRight().x(), rect.bottomRight().y());
            }
        }
        painter->restore();
        break;
    default:
        QCleanlooksStyle::drawControl(element, option, painter, widget);
        break;
    }
}

void medStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    switch (control) {
    case CC_ComboBox:
        QCleanlooksStyle::drawComplexControl(control, option, painter, widget);
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            if (comboBox->state && State_Enabled && comboBox->state & State_HasFocus) {
                QRect rect = option->rect.adjusted(0, 0, -1, -1);
                painter->save();
                painter->setPen(d->highlight);
                painter->drawLine(rect.topLeft().x()+2, rect.topLeft().y()+1, rect.topRight().x()-1, rect.topRight().y()+1);
                painter->drawLine(rect.bottomLeft().x()+2, rect.bottomLeft().y(), rect.bottomRight().x()-1, rect.bottomRight().y());
                painter->restore();
            } 
        }      
        break;
// #ifndef QT_NO_SCROLLBAR
//     case CC_ScrollBar:
//         painter->save();
//         if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
//             bool isEnabled = scrollBar->state & State_Enabled;
//             bool reverse = scrollBar->direction == Qt::RightToLeft;
//             bool horizontal = scrollBar->orientation == Qt::Horizontal;
//             bool sunken = scrollBar->state & State_Sunken;

//             painter->fillRect(option->rect, option->palette.background());

//             QRect rect = scrollBar->rect;
//             QRect scrollBarSubLine = proxy()->subControlRect(control, scrollBar, SC_ScrollBarSubLine, widget);
//             QRect scrollBarAddLine = proxy()->subControlRect(control, scrollBar, SC_ScrollBarAddLine, widget);
//             QRect scrollBarSlider = proxy()->subControlRect(control, scrollBar, SC_ScrollBarSlider, widget);
//             QRect grooveRect = proxy()->subControlRect(control, scrollBar, SC_ScrollBarGroove, widget);

//             // paint groove
//             if (scrollBar->subControls & SC_ScrollBarGroove) {
//                 painter->setBrush(Qt::green);
//                 painter->setPen(Qt::NoPen);
//                 if (horizontal) {
//                     painter->drawRect(grooveRect);
//                     painter->setPen(Qt::black);
//                     painter->drawLine(grooveRect.topLeft(), grooveRect.topRight());
//                     painter->drawLine(grooveRect.bottomLeft(), grooveRect.bottomRight());
//                 } else {
//                     painter->drawRect(grooveRect);
//                     painter->setPen(Qt::black);
//                     painter->drawLine(grooveRect.topLeft(), grooveRect.bottomLeft());
//                     painter->drawLine(grooveRect.topRight(), grooveRect.bottomRight());
//                 }
//             }

//             //paint slider
//             if (scrollBar->subControls & SC_ScrollBarSlider) {
//                 QRect pixmapRect = scrollBarSlider;
//                 if (horizontal)
//                     pixmapRect.adjust(-1, 0, 0, -1);
//                 else
//                     pixmapRect.adjust(0, -1, -1, 0);

//                 if (isEnabled) {
//                     QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(),
//                                              pixmapRect.center().x(), pixmapRect.bottom());
//                     if (!horizontal)
//                         gradient = QLinearGradient(pixmapRect.left(), pixmapRect.center().y(),
//                                                    pixmapRect.right(), pixmapRect.center().y());

//                     if (option->palette.button().gradient()) {
//                         gradient.setStops(option->palette.button().gradient()->stops());
//                     } else {
//                         if (sunken || (option->state & State_MouseOver &&
//                             (scrollBar->activeSubControls & SC_ScrollBarSlider))) {
//                             gradient.setColorAt(0, d->gradientUp.lighter(110));
//                             gradient.setColorAt(1, d->gradientDw.lighter(110));
//                         } else {
//                             gradient.setColorAt(0, d->gradientUp);
//                             gradient.setColorAt(1, d->gradientDw);
//                         }
//                     }
//                     painter->setPen(QPen(Qt::black, 0));
//                     painter->setBrush(gradient);
//                     painter->drawRect(pixmapRect);
//                 } else {
//                     QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(),
//                                              pixmapRect.center().x(), pixmapRect.bottom());
//                     if (!horizontal) {
//                         gradient = QLinearGradient(pixmapRect.left(), pixmapRect.center().y(),
//                                                    pixmapRect.right(), pixmapRect.center().y());
//                     }
//                     if (sunken) {
//                         gradient.setColorAt(0, d->gradientUp.lighter(110));
//                         gradient.setColorAt(1, d->gradientDw.lighter(110));
//                     } else {
//                         gradient.setColorAt(0, d->gradientUp);
//                         gradient.setColorAt(1, d->gradientDw);
//                     }
//                     painter->setPen(Qt::black);
//                     painter->setBrush(gradient);
//                     painter->drawRect(pixmapRect);
//                 }
//             }
//         }
//         painter->restore();
//         break;
// #endif // QT_NO_SCROLLBAR
    case CC_SpinBox:
        QCleanlooksStyle::drawComplexControl(control, option, painter, widget);
        painter->save();
        painter->setPen(d->window);
        painter->drawLine(QPoint(option->rect.left()+2, option->rect.bottom()), QPoint(option->rect.right()-2, option->rect.bottom()));
        painter->restore();
        break;
    default:
        QCleanlooksStyle::drawComplexControl(control, option, painter, widget);
    }
}

void medStyle::polish(QWidget *widget)
{
    QCleanlooksStyle::polish(widget);
}

void medStyle::polish(QApplication *application)
{
    QCleanlooksStyle::polish(application);
}

void medStyle::polish(QPalette &palette)
{
    QCleanlooksStyle::polish(palette);

    // /////////////////////////////////////////////////////////////////
    // Active palette
    // /////////////////////////////////////////////////////////////////
    palette.setColor(QPalette::Active, QPalette::Base, d->base);
    palette.setColor(QPalette::Active, QPalette::AlternateBase, d->base.lighter(110));
    palette.setColor(QPalette::Active, QPalette::Window, d->window);
    palette.setColor(QPalette::Active, QPalette::WindowText, d->windowText);
    palette.setColor(QPalette::Active, QPalette::Button, d->gradientMd);
    palette.setColor(QPalette::Active, QPalette::ButtonText, d->windowText.lighter());
    palette.setColor(QPalette::Active, QPalette::ToolTipBase, d->tooltipbase);
    palette.setColor(QPalette::Active, QPalette::ToolTipText, d->tooltiptext);
    palette.setColor(QPalette::Active, QPalette::BrightText, d->brightText);
    palette.setColor(QPalette::Active, QPalette::Text, d->windowText);
    // --
    palette.setColor(QPalette::Active, QPalette::Light, d->light);
    palette.setColor(QPalette::Active, QPalette::Midlight, d->midlight);
    palette.setColor(QPalette::Active, QPalette::Dark, d->dark);
    palette.setColor(QPalette::Active, QPalette::Mid, d->mid);
    palette.setColor(QPalette::Active, QPalette::Shadow, d->shadow);
    // --
    palette.setColor(QPalette::Active, QPalette::Highlight, d->highlight);
    palette.setColor(QPalette::Active, QPalette::HighlightedText, d->highlightedText);
    // --
    palette.setColor(QPalette::Active, QPalette::Link, d->link);
    palette.setColor(QPalette::Active, QPalette::Link, d->linkVisited);
    // --
    palette.setColor(QPalette::Active, QPalette::NoRole, Qt::red);

    // /////////////////////////////////////////////////////////////////
    // Inactive palette
    // /////////////////////////////////////////////////////////////////
    palette.setColor(QPalette::Inactive, QPalette::Base, d->base);
    palette.setColor(QPalette::Inactive, QPalette::AlternateBase, d->base.lighter(110));
    palette.setColor(QPalette::Inactive, QPalette::Window, d->window);
    palette.setColor(QPalette::Inactive, QPalette::WindowText, d->windowText);
    palette.setColor(QPalette::Inactive, QPalette::Button, d->gradientMd);
    palette.setColor(QPalette::Inactive, QPalette::ButtonText, d->windowText.lighter());
    palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, d->tooltipbase);
    palette.setColor(QPalette::Inactive, QPalette::ToolTipText, d->tooltiptext);
    palette.setColor(QPalette::Inactive, QPalette::BrightText, d->brightText);
    palette.setColor(QPalette::Inactive, QPalette::Text, d->windowText);
    // --
    palette.setColor(QPalette::Inactive, QPalette::Light, d->light);
    palette.setColor(QPalette::Inactive, QPalette::Midlight, d->midlight);
    palette.setColor(QPalette::Inactive, QPalette::Dark, d->dark);
    palette.setColor(QPalette::Inactive, QPalette::Mid, d->mid);
    palette.setColor(QPalette::Inactive, QPalette::Shadow, d->shadow);
    // --
    palette.setColor(QPalette::Inactive, QPalette::Highlight, d->highlight);
    palette.setColor(QPalette::Inactive, QPalette::HighlightedText, d->highlightedText);
    // --
    palette.setColor(QPalette::Inactive, QPalette::Link, d->link);
    palette.setColor(QPalette::Inactive, QPalette::Link, d->linkVisited);
    // --
    palette.setColor(QPalette::Inactive, QPalette::NoRole, Qt::red);

    // /////////////////////////////////////////////////////////////////
    // Disabled palette
    // /////////////////////////////////////////////////////////////////
    palette.setColor(QPalette::Disabled, QPalette::Base, d->window);
    palette.setColor(QPalette::Disabled, QPalette::AlternateBase, d->base.lighter(110));
    palette.setColor(QPalette::Disabled, QPalette::Window, d->window);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, d->windowText.darker(150));
    palette.setColor(QPalette::Disabled, QPalette::Button, d->gradientMd);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, d->border);
    palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, d->tooltipbase);
    palette.setColor(QPalette::Disabled, QPalette::ToolTipText, d->tooltiptext);
    palette.setColor(QPalette::Disabled, QPalette::BrightText, d->brightText);
    palette.setColor(QPalette::Disabled, QPalette::Text, d->border);
    // --
    palette.setColor(QPalette::Disabled, QPalette::Light, d->light);
    palette.setColor(QPalette::Disabled, QPalette::Midlight, d->midlight);
    palette.setColor(QPalette::Disabled, QPalette::Dark, d->dark);
    palette.setColor(QPalette::Disabled, QPalette::Mid, d->mid);
    palette.setColor(QPalette::Disabled, QPalette::Shadow, d->shadow);
    // --
    palette.setColor(QPalette::Disabled, QPalette::Highlight, d->highlight);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, d->highlightedText);
    // --
    palette.setColor(QPalette::Disabled, QPalette::Link, d->link);
    palette.setColor(QPalette::Disabled, QPalette::Link, d->linkVisited);
}

void medStyle::unpolish(QWidget *widget)
{
    QCleanlooksStyle::unpolish(widget);
}

void medStyle::unpolish(QApplication *application)
{
    QCleanlooksStyle::unpolish(application);
}

QPalette medStyle::standardPalette(void) const
{
    QPalette palette = QCleanlooksStyle::standardPalette();

    return palette;
}

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////

void medStyle::medDrawRect(QPainter *painter, const QRect& rect, QColor borderColor)
{
    painter->save();
    painter->setPen(borderColor);
    painter->drawRect(rect);
    painter->restore();
}

void medStyle::medDrawRectWithColor(QPainter *painter, const QRect& rect, QColor fillColor, QColor borderColor)
{
    painter->save();
    painter->setPen(borderColor);
    painter->setBrush(fillColor);
    painter->drawRect(rect);
    painter->restore();
}

void medStyle::medFillRectWithColor(QPainter *painter, const QRect& rect, QColor fillColor)
{
    painter->fillRect(rect, fillColor);
}

void medStyle::medDrawRoundRectWithColor(QPainter *painter, const QRectF& rect, QColor fillColor, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius, QColor borderColor)
{
    painter->save();

    // painter->setRenderHint(QPainter::Antialiasing);

    QPainterPath path(QPoint(rect.left(), rect.top() + leftTopRadius));
    path.quadTo(rect.left(), rect.top(), rect.left() + leftTopRadius, rect.top());
    path.lineTo(rect.right() - rightTopRadius, rect.top());
    path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + rightTopRadius);
    path.lineTo(rect.right(), rect.bottom() - rightBottomRadius);
    path.quadTo(rect.right(), rect.bottom(), rect.right() - rightBottomRadius, rect.bottom());
    path.lineTo(rect.left() + leftBottomRadius, rect.bottom());
    path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - leftBottomRadius);
    path.closeSubpath();

    painter->setPen(borderColor);
    painter->setBrush(fillColor);
    painter->drawPath(path);

    painter->restore();
}

void medStyle::medFillRoundRectWithColor(QPainter *painter, const QRectF& rect, QColor fillColor, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius)
{
    painter->save();

    // painter->setRenderHint(QPainter::Antialiasing);

    QPainterPath path(QPoint(rect.left(), rect.top() + leftTopRadius));
    path.quadTo(rect.left(), rect.top(), rect.left() + leftTopRadius, rect.top());
    path.lineTo(rect.right() - rightTopRadius, rect.top());
    path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + rightTopRadius);
    path.lineTo(rect.right(), rect.bottom() - rightBottomRadius);
    path.quadTo(rect.right(), rect.bottom(), rect.right() - rightBottomRadius, rect.bottom());
    path.lineTo(rect.left() + leftBottomRadius, rect.bottom());
    path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - leftBottomRadius);
    path.closeSubpath();

    painter->fillPath(path, fillColor);

    painter->restore();
}

void medStyle::medDrawRectWithDoubleLinearGradient(QPainter *painter, const QRect& rect, QColor upStart, QColor upStop, QColor dwStart, QColor dwStop, QColor borderColor)
{
    QLinearGradient linearGrad(QPointF(rect.x(), rect.y()), QPointF(rect.x(), rect.y() + rect.height()));
    linearGrad.setColorAt(0.00, upStart);
    linearGrad.setColorAt(0.49, upStop);
    linearGrad.setColorAt(0.50, dwStart);
    linearGrad.setColorAt(1.00, dwStop);

    painter->save();
    painter->setPen(borderColor);
    painter->setBrush(linearGrad);
    painter->drawRect(rect);
    painter->restore();
}

void medStyle::medFillRectWithDoubleLinearGradient(QPainter *painter, const QRect& rect, QColor upStart, QColor upStop, QColor dwStart, QColor dwStop)
{
    QLinearGradient linearGrad;
    // if(rect.width() > rect.height()) {
    //     linearGrad.setStart(QPointF(rect.x(), rect.y()));
    //     linearGrad.setFinalStop(QPointF(rect.x(), rect.y() + rect.height()));
    // } else {
    //     linearGrad.setStart(QPointF(rect.x(), rect.y()));
    //     linearGrad.setFinalStop(QPointF(rect.x() + rect.width(), rect.y()));
    // }
    linearGrad.setStart(QPointF(rect.x(), rect.y()));
    linearGrad.setFinalStop(QPointF(rect.x(), rect.y() + rect.height()));
    linearGrad.setColorAt(0.00, upStart);
    linearGrad.setColorAt(0.49, upStop);
    linearGrad.setColorAt(0.50, dwStart);
    linearGrad.setColorAt(1.00, dwStop);

    painter->fillRect(rect, linearGrad);
}

void medStyle::medDrawRoundRectWithDoubleLinearGradient(QPainter *painter, const QRectF& rect, QColor upStart, QColor upStop, QColor dwStart, QColor dwStop, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius, QColor borderColor)
{
    painter->save();

    // painter->setRenderHint(QPainter::Antialiasing);

    QPainterPath path(QPoint(rect.left(), rect.top() + leftTopRadius));
    path.quadTo(rect.left(), rect.top(), rect.left() + leftTopRadius, rect.top());
    path.lineTo(rect.right() - rightTopRadius, rect.top());
    path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + rightTopRadius);
    path.lineTo(rect.right(), rect.bottom() - rightBottomRadius);
    path.quadTo(rect.right(), rect.bottom(), rect.right() - rightBottomRadius, rect.bottom());
    path.lineTo(rect.left() + leftBottomRadius, rect.bottom());
    path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - leftBottomRadius);
    path.closeSubpath();
    
    QLinearGradient linearGrad(QPointF(rect.x(), rect.y()), QPointF(rect.x(), rect.y() + rect.height()));
    linearGrad.setColorAt(0.00, upStart);
    linearGrad.setColorAt(0.49, upStop);
    linearGrad.setColorAt(0.50, dwStart);
    linearGrad.setColorAt(1.00, dwStop);

    painter->setPen(borderColor);
    painter->setBrush(linearGrad);
    painter->drawPath(path);

    painter->restore();
}

void medStyle::medFillRoundRectWithDoubleLinearGradient(QPainter *painter, const QRectF& rect, QColor upStart, QColor upStop, QColor dwStart, QColor dwStop, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius)
{
    painter->save();

    // painter->setRenderHint(QPainter::Antialiasing);

    QPainterPath path(QPoint(rect.left(), rect.top() + leftTopRadius));
    path.quadTo(rect.left(), rect.top(), rect.left() + leftTopRadius, rect.top());
    path.lineTo(rect.right() - rightTopRadius, rect.top());
    path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + rightTopRadius);
    path.lineTo(rect.right(), rect.bottom() - rightBottomRadius);
    path.quadTo(rect.right(), rect.bottom(), rect.right() - rightBottomRadius, rect.bottom());
    path.lineTo(rect.left() + leftBottomRadius, rect.bottom());
    path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - leftBottomRadius);
    path.closeSubpath();
    
    QLinearGradient linearGrad(QPointF(rect.x(), rect.y()), QPointF(rect.x(), rect.y() + rect.height()));
    linearGrad.setColorAt(0.00, upStart);
    linearGrad.setColorAt(0.49, upStop);
    linearGrad.setColorAt(0.50, dwStart);
    linearGrad.setColorAt(1.00, dwStop);

    painter->fillPath(path, linearGrad);

    painter->restore();
}
