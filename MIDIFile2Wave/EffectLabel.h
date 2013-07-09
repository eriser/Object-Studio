/**
 * (C) 2010 Wolfgang Plaschg <wolfgang.plaschg@gmail.com>
 * Licensed under MIT-LICENSE 
 */
#ifndef EFFECTLABEL_H
#define EFFECTLABEL_H

#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QIcon>

class EffectLabel : public QLabel
{
	Q_OBJECT

public:
	enum Effect {
	    Plain = 0,
		Sunken,
		Raised
	};
	EffectLabel(QWidget* parent = 0);
        void setShadowColor(QColor c);
        void setIcon(QIcon i);
        void setIconSize(QSize s);
        QIcon icon();
	void paintEvent(QPaintEvent *event);
	Effect effect() const
		{ return effect_; }
	void setEffect(Effect effect)
		{ effect_ = effect; repaint(); }
private:
    void drawTextEffect(QPainter* painter, QPoint offset);
	Effect effect_;
    QColor shadowColor_;
    QIcon icon_;
    QSize iconSize_;
    QRect textRect_;
};

#endif
