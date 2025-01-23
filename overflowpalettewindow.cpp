#include "overflowpalettewindow.h"
#include "ui_overflowpalettewindow.h"
#include <qapplication.h>
#include <qtimer.h>

OverflowPaletteWindow::OverflowPaletteWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OverflowPaletteWindow)
{
    ui->setupUi(this);

	ui->MainGamePanel->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
	// ui->MainGamePanel->Handle_OnConfigChanged(&GameConfig);

	ui->ConfigManager->SetUI_PlayingMode();
	connect(ui->ConfigManager, &GameConfigManager::OnGameConfigChanged, this,
	[this](GamePanelConfig& InConfig)
		{
			ui->MainGamePanel->Handle_OnConfigChanged(&InConfig);
		}
	);

	connect(ui->MainTab, &QTabWidget::currentChanged, this, [this](int Index)
		{
			if (Index == 0)
			{
				this->setWindowTitle("Overflowing Palette Game");
			}
			else if (Index == 1)
			{
				this->setWindowTitle(ui->GameEditorMainPanelWidget->GetWindowTitleForEditingTab());
			}
		}
	);
	// ui->MainTab
}

OverflowPaletteWindow::~OverflowPaletteWindow()
{
    delete ui;
}
