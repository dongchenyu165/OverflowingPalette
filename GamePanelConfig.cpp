#include "GamePanelConfig.h"
#include <QJsonObject>
#include <qcontainerfwd.h>
#include <qjsondocument.h>
#include <qjsonvalue.h>



// Serialize the QJsonArray with custom formatting
QString GamePanelConfig::GameGridColorListSerializer() const
{
	QString CustomOutput = "[\n\t\t";
	// for (int RowIndex = 0; RowIndex < GameGridColors.size(); RowIndex += Cols)
	// {
	// 	for (int ColumnIndex = 0; ColumnIndex < Cols && (RowIndex + ColumnIndex) < GameGridColors.size(); ColumnIndex++)
	// 	{
	// 		CustomOutput += QString::number(GameGridColors[RowIndex + ColumnIndex]);
	// 		CustomOutput += ", ";
	// 	}

	// 	CustomOutput += "\n";
	// }

	for ( int i = 0; i < GameGridColors.size(); i++ )
	{
		if ( Q_UNLIKELY(i == GameGridColors.size() - 1) )
		{
			CustomOutput += QString("%1\n\t]").arg(GameGridColors[i]);
			break;
		}
		else
		{
			CustomOutput += QString("%1, ").arg(GameGridColors[i]);
		}

		if ( Q_UNLIKELY(i % Cols == Cols - 1) )
		{
			CustomOutput += "\n\t\t";
		}
	}
	// Remove the last comma and space.
	// CustomOutput.remove(CustomOutput.size() - 3, 2);

	// CustomOutput += "]";
	return CustomOutput;
}


GamePanelConfig::GamePanelConfig() {}

GamePanelConfig::GamePanelConfig(GamePanelConfig&& InOther) noexcept
	: Rows(InOther.Rows), Cols(InOther.Cols), MaxStep(InOther.MaxStep),
	  TargetColorIndex(InOther.TargetColorIndex), Name(std::move(InOther.Name)),
	  GameGridColors(std::move(InOther.GameGridColors)),
	  ColorPalette(std::move(InOther.ColorPalette)),
	  ColorPaletteName(std::move(InOther.ColorPaletteName))
{
}

GamePanelConfig& GamePanelConfig::operator=(GamePanelConfig&& InOther) noexcept
{
	if ( this != &InOther )
	{
		Rows             = InOther.Rows;
		Cols             = InOther.Cols;
		MaxStep          = InOther.MaxStep;
		TargetColorIndex = InOther.TargetColorIndex;
		Name             = std::move(InOther.Name);
		GameGridColors   = std::move(InOther.GameGridColors);
		ColorPalette     = std::move(InOther.ColorPalette);
		ColorPaletteName = std::move(InOther.ColorPaletteName);
	}
	return *this;
}

bool GamePanelConfig::IsValidJson(const QJsonObject& InJsonObj)
{
	// clang-format off
	if (!InJsonObj.contains("Name")) return false;
	if (!InJsonObj.contains("Rows")) return false;
	if (!InJsonObj.contains("Cols")) return false;
	if (!InJsonObj.contains("MaxStep")) return false;
	if (!InJsonObj.contains("TargetColorIndex")) return false;
	if (!InJsonObj.contains("GameGridColors")) return false;
	if (!InJsonObj.contains("ColorsInfo")) return false;
	// clang-format on
	return true;
}
GamePanelConfig& GamePanelConfig::FromJson(const QJsonObject& json)
{
	Name = json["Name"].toString();
	Rows = json["Rows"].toInt();
	Cols = json["Cols"].toInt();
	MaxStep = json["MaxStep"].toInt();
	TargetColorIndex = json["TargetColorIndex"].toInt();

	GameGridColors.clear();
	GameGridColors.reserve(Rows * Cols);
	QJsonArray colors = json["GameGridColors"].toArray();
	assert(colors.size() == Rows * Cols);
	int ColorIndexMax = -1;
	for (int i = 0; i < colors.size(); i++)
	{
		GameGridColors.push_back(colors[i].toInt());
		ColorIndexMax = std::max(ColorIndexMax, colors[i].toInt());
	}


	QJsonObject ColorsInfoDict = json["ColorsInfo"].toObject();
	ColorPalette.clear();
	ColorPaletteName.clear();
	ColorPaletteName = ColorsInfoDict.keys();
	for ( int i = 0; i < ColorsInfoDict.size(); i++ )
	{
		QJsonObject ColorJsonObj = ColorsInfoDict[ColorPaletteName[i]].toObject();
		ColorPalette.push_back(
			QColor(ColorJsonObj["r"].toInt(), ColorJsonObj["g"].toInt(), ColorJsonObj["b"].toInt())
		);
	}

	// ColorPalette.clear();
	// QJsonArray palette = json["ColorPalette"].toArray();
	// for (int i = 0; i < palette.size(); i++)
	// {
	// 	QJsonObject color = palette[i].toObject();
	// 	ColorPalette.push_back(QColor(color["r"].toInt(), color["g"].toInt(), color["b"].toInt()));
	// }
	// assert(ColorIndexMax < ColorPalette.size());

	// ColorPaletteName.clear();
	// QJsonArray paletteName = json["ColorPaletteName"].toArray();
	// assert(palette.size() == paletteName.size());
	// for (int i = 0; i < paletteName.size(); i++)
	// {
	// 	ColorPaletteName.push_back(paletteName[i].toString());
	// }
	// assert(ColorIndexMax < ColorPaletteName.size());

	return *this;
}

void GamePanelConfig::ToJson(QJsonObject& json) const
{
	json["Name"] = Name;
	json["Rows"] = Rows;
	json["Cols"] = Cols;
	json["MaxStep"] = MaxStep;
	json["TargetColorIndex"] = TargetColorIndex;

	QJsonArray colors;
	for (int i = 0; i < GameGridColors.size(); i++)
	{
		colors.push_back(GameGridColors[i]);
	}
	json["GameGridColors"] = colors;

	QJsonObject ColorsInfoDict;
	for ( int i = 0; i < ColorPalette.size(); i++ )
	{
		QJsonObject ColorJsonObj;
		ColorJsonObj["r"] = ColorPalette[i].red();
		ColorJsonObj["g"] = ColorPalette[i].green();
		ColorJsonObj["b"] = ColorPalette[i].blue();

		ColorsInfoDict[ColorPaletteName[i]] = ColorJsonObj;
	}
	json["ColorsInfo"] = ColorsInfoDict;

	// QJsonArray palette;
	// for (int i = 0; i < ColorPalette.size(); i++)
	// {
	// 	QJsonObject color;
	// 	color["r"] = ColorPalette[i].red();
	// 	color["g"] = ColorPalette[i].green();
	// 	color["b"] = ColorPalette[i].blue();
	// 	palette.push_back(color);
	// }
	// json["ColorPalette"] = palette;

	// QJsonArray paletteName;
	// for (int i = 0; i < ColorPaletteName.size(); i++)
	// {
	// 	paletteName.push_back(ColorPaletteName[i]);
	// }
	// json["ColorPaletteName"] = paletteName;
}

QString GamePanelConfig::JsonSerializer()
{
	static const QStringList Keys = {
		"Name", "Rows", "Cols", "MaxStep", "TargetColorIndex", "GameGridColors", "ColorPalette", "ColorPaletteName"
	};
	static const QStringList SpecialSerializerKeys = {
		"GameGridColors"
	};
	static const QString JsonStringOriginal = R"({
	"Name": "%1",
	"Rows": %2,
	"Cols": %3,
	"MaxStep": %4,
	"TargetColorIndex": %5,
	"GameGridColors": %6,
	"ColorsInfo": %7
})";

	QString JsonString = JsonStringOriginal;

	// Normal members' serialization.
	JsonString = JsonString.arg(Name).arg(Rows).arg(Cols).arg(MaxStep).arg(TargetColorIndex)
		/* GameGridColors */.arg(GameGridColorListSerializer())
		/* ColorsInfo */.arg(ColorInfoSerializer());


	// QByteArray(InKeyName.toStdString().c_str());
	// QString GameGridColorsJsonBytes = Serialize(InJsonObj.toArray(), InElementEachRow);

	return JsonString;

}

QString GamePanelConfig::ColorInfoSerializer() const
{
	static const QString JsonStringOriginal = R"(		"%1": {"r": %2, "g": %3, "b": %4}, 
)";

	QString ColorsInfoJsonString = "{\n";

	for ( int i = 0; i < ColorPalette.size(); i++ )
	{
		QString SingleColorJsonString = JsonStringOriginal;
		SingleColorJsonString = SingleColorJsonString.arg(ColorPaletteName[i]).arg(ColorPalette[i].red()).arg(ColorPalette[i].green()).arg(ColorPalette[i].blue());
		ColorsInfoJsonString += SingleColorJsonString;
	}

	// Remove the last comma and space to get a valid JSON string.
	ColorsInfoJsonString.remove(ColorsInfoJsonString.size() - 3, 2);

	ColorsInfoJsonString += "\t}\n";

	return ColorsInfoJsonString;
}


