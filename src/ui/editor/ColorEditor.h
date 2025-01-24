#ifndef COLOREDITOR_H
#define COLOREDITOR_H

#include "GamePanelConfig.h"
#include <QWidget>
#include <qtmetamacros.h>

namespace Ui {
class ColorEditor;
}

class ColorEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ColorEditor(QWidget *parent = nullptr);
    ~ColorEditor();

    void InitFromConfig(GamePanelConfig* InConfigPtr);


public:
	QColor PickedColor;
	QString PickedColorName;
private:
    Ui::ColorEditor *ui;
    GamePanelConfig* ConfigPtr = nullptr;
    void __AddColorItemToList(const QString& InText, const QColor& InColor);
    void OnItemClicked(const QModelIndex& InClickedIndex);

signals:
    void OnColorItemClicked(const int& InIndex, const QString& InColorName, const QColor& InColor);
    void OnNewColorAdded(const int& InIndex, const QString& InColorName, const QColor& InColor);
    void OnColorDeleted(const int& InIndex, const QString& InColorName, const QColor& InColor);
public slots:
};

#endif // COLOREDITOR_H
