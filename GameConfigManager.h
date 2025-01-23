#ifndef GAMECONFIGMANAGER_H
#define GAMECONFIGMANAGER_H

#include "GamePanelConfig.h"
#include <QWidget>
#include <qdir.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <QMessageBox>

namespace Ui
{
class GameConfigManager;
}

class GameConfigManager : public QWidget
{
	Q_OBJECT

public:
	explicit GameConfigManager(QWidget* parent = nullptr);
	~GameConfigManager();
	QString GetEditorTitleName() const { return EditorTitle; }

	/**
	 * @brief Just hide some buttons for editing purpose.
	 * 
	 */
	void SetUI_PlayingMode();

private:
	void Handle_OnOpenNewJson();
	void Handle_OnSaveJson();
	void Handle_OnNewJsonFile();
	void Handle_OnNewSingleGameConfigClicked();

	bool IsCurrentConfigUnSaved();
	QMessageBox::StandardButton HandleConfigUnSaved();
	QMessageBox::StandardButton HandleCurrentConfigFileUnSaved();
	bool HandleSwitchToNewConfig(GamePanelConfig& InNewConfig);
	bool HandleSwitchToNewJsonFile();
	
	void Exec_SaveSingleConfigToList();

	void SetUI_InitState();
	void SetUI_EditingState();

	GamePanelConfig CreateGameConfig(const QString& InConfigName = "");

private:
	Ui::GameConfigManager* ui;

	bool bRequestSaveToFile = false;
	bool bRequestSaveOperatingConfig = true;

	QFile OperatingFile;
	QJsonArray GameConfigsJsonList;
	QJsonObject OperatingJsonObj;

	GamePanelConfig OperatingGameConfig;
	QVector<GamePanelConfig> GameConfigList;

	QString EditorTitle = "Editing: Please Open/Create a JSON file.";
	QString UnsavedPrefix = "**(UnSaved)** ";
	QString JsonFileUnsavedPrefix = "**(Unsave Config Json File)** ";

signals:
	void OnGameConfigChanged(GamePanelConfig& InConfig);

public slots:
	void Handle_OnOperatingConfigEdited(GamePanelConfig* InConfigPtr);
};

#endif // GAMECONFIGMANAGER_H
