#include "GameConfigManager.h"
#include "GamePanelConfig.h"
#include "ui_GameConfigManager.h"
#include <QFileDialog>
#include <qcontainerfwd.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <QJsonDocument>
#include <QMessageBox>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <QInputDialog>
#include <quuid.h>


GameConfigManager::GameConfigManager(QWidget* parent)
	: QWidget(parent), ui(new Ui::GameConfigManager)
{
	ui->setupUi(this);

	SetUI_InitState();

	connect(ui->Btn_EditorOpenJson, &QPushButton::clicked, this, &GameConfigManager::Handle_OnOpenNewJson);
	connect(ui->Btn_EditorSaveJson, &QPushButton::clicked, this, &GameConfigManager::Handle_OnSaveJson);
	connect(ui->Btn_EditorNewJson, &QPushButton::clicked, this, &GameConfigManager::Handle_OnNewJsonFile);

	// Single game config editor related widgets' events.
	connect(
		ui->Btn_EditorSave,
		&QPushButton::clicked,
		[this]()
		{
			const int Idx = ui->ComboBox_ConfigSelector->currentIndex();
			if (OperatingGameConfig == GameConfigList[Idx])
			{
				return;
			}

			Exec_SaveSingleConfigToList();
		}
	);
	connect(
		ui->Btn_EditorNew,
		&QPushButton::clicked,
		this,
		&GameConfigManager::Handle_OnNewSingleGameConfigClicked
	);
	connect(
		ui->Btn_EditName,
		&QPushButton::clicked,
		this,
		[this]()
		{
			const int Idx   = ui->ComboBox_ConfigSelector->currentIndex();
			QString NewName = QInputDialog::getText(
				this,
				"Edit Name",
				"Input the new name of the game config:",
				QLineEdit::Normal,
				OperatingGameConfig.Name
			);

			OperatingGameConfig.Name = NewName;
			bRequestSaveToFile = true;

			ui->ComboBox_ConfigSelector->setItemText(Idx, NewName);
		}
	);

	connect(
		ui->ComboBox_ConfigSelector,
		QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this](int Index)
		{
			if (GameConfigList.size() == 0)
			{
				return;
			}
			OperatingGameConfig = GameConfigList[Index];
			emit OnGameConfigChanged(OperatingGameConfig);
		}
	);
}

GameConfigManager::~GameConfigManager() { delete ui; }

void GameConfigManager::SetUI_InitState()
{
	ui->Btn_EditorNewJson->setEnabled(true);
	ui->Btn_EditorOpenJson->setEnabled(true);
	ui->Btn_EditorSaveJson->setEnabled(false);

	ui->Btn_EditorNew->setEnabled(false);
	ui->Btn_EditName->setEnabled(false);
	ui->Btn_EditorSave->setEnabled(false);
	ui->ComboBox_ConfigSelector->setEnabled(false);
}
void GameConfigManager::SetUI_EditingState()
{
	ui->Btn_EditorSaveJson->setEnabled(true);

	ui->Btn_EditorNew->setEnabled(true);
	ui->Btn_EditName->setEnabled(true);
	ui->Btn_EditorSave->setEnabled(true);
	ui->ComboBox_ConfigSelector->setEnabled(true);
}

void GameConfigManager::SetUI_PlayingMode()
{
	UnsavedPrefix = "";

	ui->Btn_EditorNewJson->hide();
	ui->Btn_EditorSaveJson->hide();

	ui->Btn_EditorNew->hide();
	ui->Btn_EditName->hide();
	ui->Btn_EditorSave->hide();
}

void GameConfigManager::Handle_OnNewJsonFile()
{
	if (!HandleSwitchToNewJsonFile())
	{
		return;
	}

	QString FilePath =
		QFileDialog::getSaveFileName(this, "Select/Create a game config JSON file.", "./", "*.json");
	if ( FilePath.isEmpty() )
	{
		QMessageBox::critical(this, "Failed to open file", "Empty file path.");
		return;
	}

	QFile ReadingJsonFile(FilePath);
	if (ReadingJsonFile.exists())
	{
		QMessageBox::StandardButton OverwriteResult = QMessageBox::critical(
			this,
			"File already exists",
			"The file already exists, Overwrite it?",
			QMessageBox::Yes | QMessageBox::No
		);
		if (OverwriteResult == QMessageBox::No)
		{
			return;
		}
	}
	OperatingFile.setFileName(FilePath);

	GameConfigList.clear();
	ui->ComboBox_ConfigSelector->blockSignals(true);
	ui->ComboBox_ConfigSelector->clear();
	ui->ComboBox_ConfigSelector->blockSignals(false);

	EditorTitle = JsonFileUnsavedPrefix + OperatingFile.fileName();
	qobject_cast<QMainWindow *>(this->window())->setWindowTitle(EditorTitle);

	SetUI_EditingState();

	// GameConfigList.append(CreateGameConfig());
	// OperatingGameConfig = GameConfigList.back();
	Handle_OnNewSingleGameConfigClicked();
	// Select the first one.
	// Broadcast the game config to the outside (By [ui->ComboBox_ConfigSelector]'s signal).
	ui->ComboBox_ConfigSelector->setCurrentIndex(0);  
	emit OnGameConfigChanged(OperatingGameConfig);
	OperatingGameConfig = GameConfigList[ui->ComboBox_ConfigSelector->currentIndex()];
}

void GameConfigManager::Handle_OnOpenNewJson()
{
	if (!HandleSwitchToNewJsonFile())
	{
		return;
	}

	QString FilePath =
		QFileDialog::getOpenFileName(this, "Select game config JSON file.", "./", "*.json");
	if ( FilePath.isEmpty() )
	{
		QMessageBox::critical(this, "Failed to open file", "Empty file path.");
		return;
	}

	QFile ReadingJsonFile(FilePath);  
	if ( !ReadingJsonFile.open(QIODevice::ReadOnly) )
	{
		QMessageBox::critical(
			this,
			"Failed to open file",
			"File: " + FilePath + "\nReason: " + ReadingJsonFile.errorString()
		);
		qDebug() << "Failed to open file: " << FilePath;
		return;
	}

	QByteArray JsonFileRawData = ReadingJsonFile.readAll();
	ReadingJsonFile.close();

	QJsonDocument JsonDocObj = QJsonDocument::fromJson(JsonFileRawData);
	if ( JsonDocObj.isNull() )
	{
		QMessageBox::critical(this, "Failed to parse JSON", "Invalid JSON format.");
		return;
	}

	QJsonArray LoadingGameConfigsJsonList = JsonDocObj.array();
	if (LoadingGameConfigsJsonList.isEmpty())
	{
		QMessageBox::critical(this, "Failed to parse JSON", "Empty JSON array.");
		return;
	}

	QJsonObject TestingJsonObj = LoadingGameConfigsJsonList[0].toObject();
	if ( !GamePanelConfig::IsValidJson(TestingJsonObj) )
	{
		QMessageBox::critical(
			this,
			"Invalid game config JSON",
			"The file content is an invalid game config JSON.\nPlease check the Keys: "
			"[Name] [Rows] [Cols] [MaxStep] [TargetColorIndex] [GameGridColors] [ColorPalette] [ColorPaletteName]"
		);
		return;
	}

	OperatingFile.setFileName(FilePath);

	EditorTitle = "Editing: " + OperatingFile.fileName();
	qobject_cast<QMainWindow *>(this->window())->setWindowTitle(EditorTitle);

	// Load the game configs' name to the ComboBox.
	ui->ComboBox_ConfigSelector->blockSignals(true);
	ui->ComboBox_ConfigSelector->clear();
	GameConfigList.clear();
	for (const auto& Config : LoadingGameConfigsJsonList)
	{
		// QJsonObject a = Config.toObject();
		GameConfigList.append(GamePanelConfig().FromJson(Config.toObject()));
		ui->ComboBox_ConfigSelector->addItem(Config.toObject()["Name"].toString());
	}
	ui->ComboBox_ConfigSelector->blockSignals(false);

	// Load the first game config json to [GamePanelConfig] object.
	OperatingGameConfig = GameConfigList[0];

	//
	SetUI_EditingState();

	// Select the first one.
	// Broadcast the game config to the outside (By [ui->ComboBox_ConfigSelector]'s signal).
	ui->ComboBox_ConfigSelector->setCurrentIndex(0);  
	emit OnGameConfigChanged(OperatingGameConfig);

	ui->ComboBox_ConfigSelector->setItemText(ui->ComboBox_ConfigSelector->currentIndex(), OperatingGameConfig.Name);
	bRequestSaveToFile = false;
	bRequestSaveOperatingConfig = false;
	OperatingGameConfig = GameConfigList[ui->ComboBox_ConfigSelector->currentIndex()];
}

void GameConfigManager::Handle_OnSaveJson()
{
	if (bRequestSaveOperatingConfig)
	{
		Exec_SaveSingleConfigToList();
	}

	GamePanelConfig ConfigForSerialization;
	QStringList ConfigJsonStringList;
	for (int i = 0; i < GameConfigList.size(); i++)
	{
		ConfigJsonStringList.append(GameConfigList[i].JsonSerializer());
	}

	QString FinalJsonString = ConfigJsonStringList.join(",\n");
	FinalJsonString.replace("\n", "\n\t");  // Add a tab-char for each line.
	// Insert the head and tail of the JSON string.
	FinalJsonString.insert(0, "[\n\t");
	FinalJsonString.append("\n]");
	QByteArray JsonData = FinalJsonString.toUtf8();

	if ( !OperatingFile.open(QIODevice::WriteOnly) )
	{
		QMessageBox::critical(
			this,
			"Failed to save JSON",
			"File: " + OperatingFile.fileName() + "\nReason: " + OperatingFile.errorString()
		);
		qDebug() << "Failed to open file: " << OperatingFile.fileName();
		return;
	}

	OperatingFile.write(JsonData);
	OperatingFile.close();
	bRequestSaveToFile = false;

	EditorTitle = "Editing: " + OperatingFile.fileName();
	qobject_cast<QMainWindow *>(this->window())->setWindowTitle(EditorTitle);
}

GamePanelConfig GameConfigManager::CreateGameConfig(const QString& InConfigName)
{
	GamePanelConfig NewConfig;

	if (InConfigName.isEmpty())
	{
		// Generate a random string by GUuid for new game config name.
		QString DefaultConfigName = "NewGameConfig_" + QUuid::createUuid().toString().mid(1, 8);

		// Create a dialog for inputting the name of new game config.
		QString NewConfigName = QInputDialog::getText(this, "New Game Config", "Input the name of new game config:", QLineEdit::Normal, DefaultConfigName);

		NewConfig.Name = NewConfigName;
	}
	else
	{
		NewConfig.Name = InConfigName;
	}

	return NewConfig;
}

bool GameConfigManager::IsCurrentConfigUnSaved()
{
	if (GameConfigList.isEmpty())
	{
		return false;
	}

	return !(OperatingGameConfig == GameConfigList[ui->ComboBox_ConfigSelector->currentIndex()]);
}

/**
 * @brief Handles unsaved changes in the game configuration
 * 
 * Displays a message box prompting the user to decide how to handle unsaved changes
 * in the current game configuration before switching to a new one.
 * 
 * @param InNewConfig The new game configuration that would replace the current one
 * 
 * @return QMessageBox::StandardButton The user's choice:
 *         - QMessageBox::Yes: Changes were saved and operation continues
 *         - QMessageBox::No: Changes were discarded and operation continues
 *         - QMessageBox::Cancel: Operation was cancelled, keeping current state
 * 
 * This function provides three options to the user:
 * - Save the current changes and continue with the new configuration
 * - Discard the current changes and continue with the new configuration
 * - Cancel the operation entirely and maintain the current state
 */
QMessageBox::StandardButton GameConfigManager::HandleConfigUnSaved()
{
	QString Message = "Current game config [%1] has unsaved changes !!\n"
		"\tYes:     Save and continue\n"
		"\tNo:      Discard changes and continue\n"
		"\tCancel:  Cancel this operation";
	Message = Message.arg(OperatingGameConfig.Name);
	return QMessageBox::question(
		this,
		"Unsaved changes",
		Message
		,
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
	);
}
QMessageBox::StandardButton GameConfigManager::HandleCurrentConfigFileUnSaved()
{
	OperatingFile.fileName();
	QString Message = "Current config file [%1] has unsaved changes !!\n"
		"\tYes:     Save and continue\n"
		"\tNo:      Discard changes and continue\n"
		"\tCancel:  Cancel this operation";
	Message = Message.arg(OperatingFile.fileName());
	return QMessageBox::question(
		this,
		"Unsaved changes",
		Message
		,
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
	);
}

bool GameConfigManager::HandleSwitchToNewConfig(GamePanelConfig& InNewConfig)
{
	bool bIsSaved = !IsCurrentConfigUnSaved();
	if (bIsSaved)
	{
		return false;
		// return true;
	}

	auto UserSelecting = HandleConfigUnSaved();
	if (UserSelecting == QMessageBox::Yes)
	{
		Exec_SaveSingleConfigToList();
		OperatingGameConfig = InNewConfig;
	}
	else if (UserSelecting == QMessageBox::No)
	{
		OperatingGameConfig = InNewConfig;
	}
	else if (UserSelecting == QMessageBox::Cancel)
	{
		return false;
	}
}

bool GameConfigManager::HandleSwitchToNewJsonFile()
{
	if (GameConfigList.size() == 0)
	{
		return true;
	}

	// The current game config.
	if ( !IsCurrentConfigUnSaved() ) 
	{
		return true;
	}

	auto UserSelecting = HandleConfigUnSaved();
	if (UserSelecting == QMessageBox::Yes)
	{
		Exec_SaveSingleConfigToList();
	}
	else if (UserSelecting == QMessageBox::No)
	{
		// Discard the game config, continue below to change config file.
	}
	else if (UserSelecting == QMessageBox::Cancel)
	{
		return false;
	}

	// The config file
	if ( !bRequestSaveToFile )
	{
		return true;
	}
	UserSelecting = HandleCurrentConfigFileUnSaved();
	if (UserSelecting == QMessageBox::Yes)
	{
		Handle_OnSaveJson();
	}
	else if (UserSelecting == QMessageBox::No)
	{
		// Discard the current config json file, change to new one.
		bRequestSaveToFile = false;
		return true;
	}
	else if (UserSelecting == QMessageBox::Cancel)
	{
		return false;
	}
	return true;
}

void GameConfigManager::Handle_OnNewSingleGameConfigClicked()
{
	GamePanelConfig NewConfig = CreateGameConfig();

	if (HandleSwitchToNewConfig(NewConfig)) 
	{
		return;
	}

	bRequestSaveToFile = true;
	GameConfigList.append(NewConfig);
	ui->ComboBox_ConfigSelector->addItem(NewConfig.Name);

	OperatingGameConfig = NewConfig;

	// Select the last one.
	const int LastIndex = ui->ComboBox_ConfigSelector->count() - 1;
	ui->ComboBox_ConfigSelector->setCurrentIndex(LastIndex);
}

void GameConfigManager::Handle_OnOperatingConfigEdited(GamePanelConfig* InConfigPtr)
{
	bRequestSaveToFile = true;
	assert(InConfigPtr == &OperatingGameConfig);
	if (!IsCurrentConfigUnSaved())
	{
		ui->ComboBox_ConfigSelector->setItemText(ui->ComboBox_ConfigSelector->currentIndex(), OperatingGameConfig.Name);
		bRequestSaveOperatingConfig = false;
		return;
	}
	if (bRequestSaveOperatingConfig)
	{
		return;
	}
	bRequestSaveOperatingConfig = true;

	QString ConfigTitle = UnsavedPrefix + OperatingGameConfig.Name;
	ui->ComboBox_ConfigSelector->setItemText(ui->ComboBox_ConfigSelector->currentIndex(), ConfigTitle);
}

void GameConfigManager::Exec_SaveSingleConfigToList()
{
	bRequestSaveToFile = true;
	
	EditorTitle = JsonFileUnsavedPrefix + OperatingFile.fileName();
	qobject_cast<QMainWindow *>(this->window())->setWindowTitle(EditorTitle);
	
	GameConfigList[ui->ComboBox_ConfigSelector->currentIndex()] = OperatingGameConfig;
	ui->ComboBox_ConfigSelector->setItemText(ui->ComboBox_ConfigSelector->currentIndex(), OperatingGameConfig.Name);
	bRequestSaveOperatingConfig = false;
}

