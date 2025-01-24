#ifndef GAMEEDITORSIDETOOLBOX_H
#define GAMEEDITORSIDETOOLBOX_H

#include "GameEditorPanelWidget.h"
#include "GamePanelConfig.h"
#include <QWidget>

namespace Ui
{
class GameEditorSideToolbox;
}

class GameEditorSideToolbox : public QWidget
{
	Q_OBJECT

public:
	explicit GameEditorSideToolbox(QWidget* parent = nullptr);
	~GameEditorSideToolbox();
	void SizeChangedDataMove(int InNewRows, int InNewCols);
	void RefreshTargetColorComboBox();

signals:
	void OnTargetColorChanged(const QString& InColorName, QColor InColor);
    void OnMaxStepChanged(int InNewMaxStep);

	void OnColorFillingMethodChanged(EColorSettingMethod InNewMethod);
	void OnRandomGrowSizeChanged(int InNewGrowSize);
	void OnColorItemClicked(const int& InIndex, const QString& InColorName, const QColor& InColor);
	void OnRemoveAllButtonsColor();
	void OnRequestRebuild();

	void OnGameConfigEdited(GamePanelConfig* InConfigPtr);

public slots:
	void Handle_OnOperatingConfigChanged(GamePanelConfig* InConfigPtr);
private:
	Ui::GameEditorSideToolbox* ui;
	GamePanelConfig* OperatingConfigPtr = nullptr;
};

#endif // GAMEEDITORSIDETOOLBOX_H
