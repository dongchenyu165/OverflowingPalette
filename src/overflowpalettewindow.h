#ifndef OVERFLOWPALETTEWINDOW_H
#define OVERFLOWPALETTEWINDOW_H

#include "GamePanelConfig.h"
#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class OverflowPaletteWindow;
}
QT_END_NAMESPACE

class OverflowPaletteWindow : public QMainWindow
{
    Q_OBJECT

public:
    OverflowPaletteWindow(QWidget *parent = nullptr);
    ~OverflowPaletteWindow();

private:
    Ui::OverflowPaletteWindow *ui;
    GamePanelConfig GameConfig;
};


class ButtonGridWidget : public QWidget {
    Q_OBJECT

public:
    ButtonGridWidget(int rows, int cols, QWidget *parent = nullptr)
        : QWidget(parent), Rows(rows), Cols(cols) {
        // 创建网格布局
        QGridLayout *gridLayout = new QGridLayout(this);

        // 循环生成按钮并添加到布局中
        for (int i = 0; i < Rows; ++i) {
            for (int j = 0; j < Cols; ++j) {
                QPushButton *button = new QPushButton(QString("Button %1,%2").arg(i + 1).arg(j + 1), this);
                gridLayout->addWidget(button, i, j); // 添加按钮到网格布局

                // 可选：连接按钮点击信号到槽函数
                connect(button, &QPushButton::clicked, this, [i, j]() {
                    qDebug("Button at (%d, %d) clicked!", i + 1, j + 1);
                });
            }
        }

        // 设置布局
        setLayout(gridLayout);
    }

private:
    int Rows;
    int Cols;
};
#endif // OVERFLOWPALETTEWINDOW_H
