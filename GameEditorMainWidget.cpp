#include "GameEditorMainWidget.h"
#include "GameEditorSideToolbox.h"
#include "ui_GameEditorMainWidget.h"

GameEditorMainWidget::GameEditorMainWidget(QWidget* parent)
	: QWidget(parent), ui(new Ui::GameEditorMainWidget)
{
	ui->setupUi(this);

	ui->GameConfigToolbox->setDisabled(true);

	connect(ui->ConfigManager, &GameConfigManager::OnGameConfigChanged, this, [this](GamePanelConfig& InConfig)
		{
			ui->GameConfigToolbox->Handle_OnOperatingConfigChanged(&InConfig);
			ui->GamePreviewer->Handle_OnConfigChanged(&InConfig);

			ui->GameConfigToolbox->setEnabled(true);
		}
	);

	connect(ui->GamePreviewer, &GameEditorPanelWidget::OnGameConfigEdited, ui->ConfigManager, &GameConfigManager::Handle_OnOperatingConfigEdited);
	connect(ui->GameConfigToolbox, &GameEditorSideToolbox::OnGameConfigEdited, ui->ConfigManager, &GameConfigManager::Handle_OnOperatingConfigEdited);

	// ToolBox -> GamePanelWidget(Previewer)
	// clang-format off
	// Page 1: All in-widget editing, so NO connection needed.
	connect(ui->GameConfigToolbox, &GameEditorSideToolbox::OnMaxStepChanged, ui->GamePreviewer, &GameEditorPanelWidget::Handle_OnMaxStepChanged);
	connect(ui->GameConfigToolbox, &GameEditorSideToolbox::OnTargetColorChanged, ui->GamePreviewer, &GameEditorPanelWidget::Handle_OnTargetColorChanged);
	// Page 2:
	connect(ui->GameConfigToolbox, &GameEditorSideToolbox::OnRequestRebuild, ui->GamePreviewer, &GameEditorPanelWidget::Handle_OnRequestRebuild);
	// Page 3:
	connect(ui->GameConfigToolbox, &GameEditorSideToolbox::OnColorFillingMethodChanged, ui->GamePreviewer, &GameEditorPanelWidget::Handle_OnColorSettingMethod);
	connect(ui->GameConfigToolbox, &GameEditorSideToolbox::OnRandomGrowSizeChanged, ui->GamePreviewer, &GameEditorPanelWidget::Handle_OnRandomGrowSizeChanged);
	connect(ui->GameConfigToolbox, &GameEditorSideToolbox::OnRemoveAllButtonsColor, ui->GamePreviewer, &GameEditorPanelWidget::Handle_OnRemoveAllColor);
	// Page 4:
	connect(ui->GameConfigToolbox, &GameEditorSideToolbox::OnColorItemClicked, ui->GamePreviewer, &GameEditorPanelWidget::Handle_OnColorItemClicked);
	// clang-format on
}

GameEditorMainWidget::~GameEditorMainWidget() { delete ui; }

QString GameEditorMainWidget::GetWindowTitleForEditingTab()
{
	return ui->ConfigManager->GetEditorTitleName();
}
