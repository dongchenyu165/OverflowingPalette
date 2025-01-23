#ifndef GAMEEDITORPANELWIDGET_H
#define GAMEEDITORPANELWIDGET_H

#include "OverflowPaletteGamePanelWidget.h"
#include <QObject>
#include <QWidget>
#include <qtmetamacros.h>


enum class EColorSettingMethod
{
	SingleCell,  // Single click to change one cell's color.
	SingleRandomGrow,  // Single click to change color, and the color will grow randomly.
	FullyRandom,
	METHOD_END
};
inline static std::map<EColorSettingMethod, QString> ColorSettingMethodMap = {
	{EColorSettingMethod::SingleCell, "Single Cell"},
	{EColorSettingMethod::SingleRandomGrow, "Single Random Grow"},
	{EColorSettingMethod::FullyRandom, "Fully Random"},
};

class GameEditorPanelWidget : public OverflowPaletteGamePanelWidget
{
	Q_OBJECT
public:
	GameEditorPanelWidget(QWidget* InParent = nullptr);
    ~GameEditorPanelWidget() override = default;

protected:
	bool eventFilter(QObject* InObject, QEvent* InEvent) override;
	void mousePressEvent(QMouseEvent* InEvent) override;
	void mouseReleaseEvent(QMouseEvent* InEvent) override;
	void mouseMoveEvent(QMouseEvent* InEvent) override;

    virtual void Handle_OnPanelButtonClicked() override;
    virtual void Handle_OnColorPickerButtonClicked() override;
	virtual void OnGridButtonCreated(QPushButton* InButton) override;

signals:
	void OnGameConfigEdited(GamePanelConfig* InConfigPtr);

public slots:
	// Slots for changing the editor's state.
	void Handle_OnConfigChanged(GamePanelConfig* InConfigPtr) override;
	void Handle_OnColorSettingMethod(EColorSettingMethod InNewMethod) { ColorSettingMethod = InNewMethod; }
	void Handle_OnRemoveAllColor();
	void Handle_OnRandomGrowSizeChanged(int InNewGrowSize) { RandomGrowSize = InNewGrowSize; }

	void Handle_OnColorItemClicked(int InIndex, const QString& InColorName, const QColor& InColor);


	// Slots for editing the game config.
	void Handle_OnRequestRebuild()
	{
		if (ConfigPtr == nullptr)
			return;
		ConfigPtr->GameGridColors.resize(ConfigPtr->Rows * ConfigPtr->Cols);
		GenerateGamePanel(*ConfigPtr);
	}
	// void OnMaxStepChanged(int InNewMaxStep) { ConfigPtr->MaxStep = InNewMaxStep; }
	// void OnTargetColorIndexChanged(int InNewTargetColorIndex) { ConfigPtr->TargetColorIndex = InNewTargetColorIndex; }

	//// Top widget
	// "Name": "Medium Grid Test123",  // F

	//// Middle widget
	// "MaxStep": 15,
	// "TargetColorIndex": 0,

	//// Game grid panel widget
	// "Rows": 5,
	// "Cols": 6,
	// "GameGridColors": ,

	//// Color editor widget
	// "ColorsInfo": {
	// 	"Green": {"r": 0, "g": 255, "b": 0}, 
	// 	"Red": {"r": 255, "g": 0, "b": 0}
	// }
	//border-radius: 30px

public:
private:
	EColorSettingMethod ColorSettingMethod = EColorSettingMethod::SingleCell;
	int RandomGrowSize = 2;

	bool IsMousePressed = false;
	QPushButton* HoveringButton = nullptr;
};

#endif // GAMEEDITORPANELWIDGET_H
