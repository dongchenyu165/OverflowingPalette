#ifndef GAMEPANELCONFIG_H
#define GAMEPANELCONFIG_H

#include <QVector>
#include <QPair>
#include <qcolor.h>
#include <QJsonObject>
#include <QJsonArray>
#include <qcontainerfwd.h>
#include <qdebug.h>


class GamePanelConfig
{
public:
    GamePanelConfig();

    // Add default copy constructor and copy assignment operator.
    GamePanelConfig(const GamePanelConfig&) = default;
    GamePanelConfig& operator=(const GamePanelConfig&) = default;

    // Add move constructor and move assignment operator.
    GamePanelConfig(GamePanelConfig&&) noexcept;
    GamePanelConfig& operator=(GamePanelConfig&&) noexcept;

    QString Name = "DefaultGame";
    int Rows = 8;
    int Cols = 10;
    int MaxStep = 4;  // The maximum steps that player can make.
    int TargetColorIndex = 0;  // The index of color that player should make all buttons to.

    // Storage the index of color for each button. In Row-Major order.
    QVector<int> GameGridColors {
        0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 
        1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 
        0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 
        0, 0, 1, 1, 3, 3, 0, 0, 1, 0, 
        0, 1, 1, 1, 3, 3, 0, 0, 0, 0, 
        0, 0, 1, 1, 3, 3, 1, 0, 0, 0, 
        0, 0, 1, 0, 3, 3, 1, 0, 0, 0, 
        0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 
    };

    // The color data.
    QVector<QColor> ColorPalette{
        QColor(255, 0, 0),
        QColor(0, 255, 0),
        QColor(0, 0, 255),
        QColor(255, 255, 0),
    };
    QStringList ColorPaletteName{
        "RED",
        "GREEN",
        "BLUE",
        "YELLOW",
    };

    static bool IsValidJson(const QJsonObject& InJsonObj);
    GamePanelConfig& FromJson(const QJsonObject& json);
    void ToJson(QJsonObject& json) const;
    QString JsonSerializer();

    void IndexToCoordinate(const int& InIndex, int& OutRow, int& OutCol) const
    {
        OutRow = InIndex / Cols;
        OutCol = InIndex % Cols;
    }
    
    int CoordinateToIndex(const int& InRow, const int& InCol) const
    {
        return InRow * Cols + InCol;
    }

    bool operator==(const GamePanelConfig& InOther) const
    {
        qDebug() << "Name: " << (Name == InOther.Name)
                 << "\nRows: " << (Rows == InOther.Rows)
                 << "\nCols: " << (Cols == InOther.Cols)
                 << "\nMaxStep: " << (MaxStep == InOther.MaxStep)
                 << "\nTargetColorIndex: " << (TargetColorIndex == InOther.TargetColorIndex)
                 << "\nColorPalette: " << (ColorPalette == InOther.ColorPalette)
                 << "\nColorPaletteName: " << (ColorPaletteName == InOther.ColorPaletteName)
                 << "\nGameGridColors: " << (GameGridColors == InOther.GameGridColors);
        return Name == InOther.Name &&
               Rows == InOther.Rows &&
               Cols == InOther.Cols &&
               MaxStep == InOther.MaxStep &&
               TargetColorIndex == InOther.TargetColorIndex &&
               ColorPalette == InOther.ColorPalette &&
               ColorPaletteName == InOther.ColorPaletteName &&
               GameGridColors == InOther.GameGridColors
               ;
    }

    void PrintGridColors(const QString& InPrefix = "") const
    {
        qDebug() << InPrefix << ":";
        for (int i = 0; i < Rows; i++)
        {
            QString RowStr = "";
            for (int j = 0; j < Cols; j++)
            {
                RowStr += QString::number(GameGridColors[i * Cols + j]) + ", ";
            }
            qDebug() << RowStr;
        }
    }

private:
    QString GameGridColorListSerializer() const;
    QString ColorInfoSerializer() const;
};

#endif // GAMEPANELCONFIG_H
