#ifndef COLORSELECTORWIDGET_H
#define COLORSELECTORWIDGET_H

#include <QObject>
#include <QWidget>
#include <qpushbutton.h>

class ColorSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorSelectorWidget(const QColor& InShowingColor, QWidget *parent = nullptr);

    QColor SelfColor;
    QPushButton* SelectingBtn;
signals:
};

#endif // COLORSELECTORWIDGET_H
