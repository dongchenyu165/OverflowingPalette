#include "ColorSelectorWidget.h"

ColorSelectorWidget::ColorSelectorWidget(const QColor& InShowingColor, QWidget *parent)
    : SelfColor(InShowingColor), QWidget{parent}
{
    SelectingBtn = new QPushButton;
    SelectingBtn->setParent(this);

    QColor ButtonColor(255, 0, 0, 255);
    QPalette Palette = SelectingBtn->palette();
    Palette.setColor(QPalette::Button, ButtonColor); // 设置按钮背景为绿色
    // ButtonStyle->
    // SelectingBtn->sty
}
