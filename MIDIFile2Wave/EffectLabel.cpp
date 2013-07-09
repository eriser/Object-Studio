/**
 * (C) 2010 Wolfgang Plaschg <wolfgang.plaschg@gmail.com>
 * Licensed under MIT-LICENSE 
 */
#include <QPen>
#include "EffectLabel.h"

EffectLabel::EffectLabel(QWidget* parent) :
	QLabel(parent),
	effect_(Plain)
{
    shadowColor_=palette().color(QPalette::Shadow);
    iconSize_=QSize(24,24);
}

void EffectLabel::setShadowColor(QColor c)
{
    shadowColor_=c;
}

void EffectLabel::drawTextEffect(QPainter *painter, 
                                 QPoint offset)
{
	Q_ASSERT(painter != 0); 
	// Draw shadow.
        painter->setPen(shadowColor_);
        painter->drawText(textRect_.translated(offset),
                      alignment() | Qt::TextWordWrap, text());
	// Draw text.
        painter->setPen(palette().color(QPalette::WindowText));
        painter->drawText(textRect_, alignment() | Qt::TextWordWrap, text());
}

void EffectLabel::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setFont(font());
    textRect_=rect();
    //painter.setRenderHint(QPainter::TextAntialiasing);
    //painter.setRenderHint(QPainter::Antialiasing);
    if (!icon_.isNull())
    {
        QRect r=geometry();
        int w=rect().width();
        if (iconSize_.width()>rect().width())
        {
            w=iconSize_.width();
        }
        int h=rect().height();
        if (iconSize_.height()>rect().height())
        {
            h=iconSize_.height()+rect().height();
        }
        r.setSize(QSize(w,h));
        if (geometry() != r) setGeometry(r);
        textRect_.setTop(iconSize_.height()-1);
        textRect_.setLeft(0);
        textRect_.setWidth(r.width());
        textRect_.setHeight(r.height()-iconSize_.height());
        QPixmap pm(icon_.pixmap(iconSize_));
        painter.drawPixmap((r.width()-iconSize_.width())/2,2,pm);
    }

    if (effect_ == Plain)
    painter.drawText(textRect_, alignment() | Qt::TextWordWrap, text());
    else if (effect_ == Sunken)
        // Shadow above the text.
        drawTextEffect(&painter, QPoint(0, 1));
    else if (effect_ == Raised)
        // Shadow below the text.
        drawTextEffect(&painter, QPoint(0, -1));
}

void EffectLabel::setIcon(QIcon i)
{
    icon_=i;
}

QIcon EffectLabel::icon()
{
    return icon_;
}

void EffectLabel::setIconSize(QSize s)
{
    iconSize_=s;
}
