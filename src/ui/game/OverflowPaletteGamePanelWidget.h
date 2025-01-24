#ifndef OVERFLOWPALETTEGAMEPANELWIDGET_H
#define OVERFLOWPALETTEGAMEPANELWIDGET_H

#include "GamePanelConfig.h"
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <qsize.h>
#include <vector>

/**
 * Computes the natural logarithm of a number using the series expansion.
 * 
 * The series expansion is:
 *  \[
 *  \ln(x) = 2 \cdot \sum_{n=1}^\infty \frac{1}{2n-1} \cdot \left( \frac{x-1}{x+1} \right)^{2n-1}, \quad x > 0
 *  \]
 * 
 * @param InValue The positive number to compute the natural logarithm of
 * @return The natural logarithm of InValue, or -1.0 if InValue <= 0
 * @note Precision is limited by the number of iterations [InMaxIter]
 */
constexpr double ConstexprLn(double InValue, int InMaxIter = 30)
{
	if (InValue <= 0.0)
		return -1.0;
	double Result = 0.0;
	double X = (InValue - 1) / (InValue + 1);
	double X2 = X * X;
	double Term = X;
	for (int N = 1; N < InMaxIter; N += 2)
	{
		// [ / N ] is [\frac{1}{2n-1}]
		Result += Term / N;

		// Calculate [\left( \frac{x-1}{x+1} \right)^{2n-1}]
		Term *= X2;
	}
	return 2 * Result;
}

constexpr double ConstexprLog(double InValue, double InBase)
{
	if (InValue <= 0.0 || InBase <= 0.0 || InBase == 1.0)
		return -1.0;
	return ConstexprLn(InValue) / ConstexprLn(InBase);
}

static constexpr double CalcTimeout(int i) {
	constexpr double EXP = 1.9;
	constexpr double BIAS = -8.9;
	constexpr double X_SCALE = 45.7;
	constexpr double Y_SCALE = 5.7;
	return ConstexprLog(((i + 1) * X_SCALE + EXP) / EXP, 10) * Y_SCALE + BIAS;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class OverflowPaletteGamePanelWidget;
}
QT_END_NAMESPACE

class OverflowPaletteGamePanelWidget : public QWidget
{
	Q_OBJECT

public:
	explicit OverflowPaletteGamePanelWidget(QWidget* InParent = nullptr);
	~OverflowPaletteGamePanelWidget() override = default;

	void GenerateGamePanel(GamePanelConfig& InConfig);
	void GenerateButtons(const GamePanelConfig &InConfig);
	void ResetGame();

protected:
	virtual void Handle_OnPanelButtonClicked();
	virtual void Handle_OnColorPickerButtonClicked();

	void SetGridButtonColor(QSharedPointer<QPushButton> InButton, const QColor& InColor);
	void SetColorPickerButtonColor(QSharedPointer<QPushButton> InButton, const QColor& InColor);
	void ExecButtonAnim(QPushButton* InBtn, int InStartTime = 0, std::function<void()> InStartAnimCallback = nullptr);

	void OnUserOperationFinished();

	virtual void OnGridButtonCreated(QPushButton* InButton) {};

public slots:

	virtual void Handle_OnConfigChanged(GamePanelConfig* InConfigPtr);
	void Handle_OnTargetColorChanged(const QString& InColorName, QColor InColor);
	void Handle_OnMaxStepChanged(int InNewMaxStep);

protected:
	QVBoxLayout* OuterVerticalLayout;
	QGridLayout* GridButtonContainer;
	QHBoxLayout* ColorPickerContainer;

	std::vector<QSharedPointer<QPushButton>> GridButtonList;
	std::vector<QSharedPointer<QPushButton>> ColorPickerButtonList;
	std::vector<QSharedPointer<QTimer>> TimerList;

	// EDITOR MODE: Point to editing config object.
	// PLAYING MODE: Point to game config for reseting game.
	GamePanelConfig* ConfigPtr = nullptr;

	// ONLY FOR PLAYING MODE: The game config object for playing.
	// Record Grid's color.
	GamePanelConfig PlayingGameConfig;
	int RemainStep = -1;

	int NewColorIndex = -1;
	bool bDisableAllBtns = false;

	QSize GridButtonSize = QSize(80, 80);
	QSize ColorPickerButtonNormalSize = QSize(60, 60);
	QSize ColorPickerButtonSelectingSize = QSize(80, 80);

	void __GenGridButtonsByConfig(const GamePanelConfig &InConfig, QLayout *InContainerPtr);
	void __RemoveWidgetInContainer(QLayout *InOutContainerPtr);
	void __GenColorPickerButtonsByConfig(const GamePanelConfig &InConfig, QLayout *InContainerPtr);
	void OnTimerFinished();

	void __SetColorPickerBtnStyleToSelecting(QPushButton* InButton);
	void __SetColorPickerBtnStyleToNormal(QPushButton* InButton);

	int GridButtonCols = 0;

	static constexpr int MAX_BFS_STEP = 512;

	static constexpr auto TIMER_TIMEOUT_TIME_LIST = []
	{
		std::array<double, MAX_BFS_STEP> arr{};
		for ( int i = 0; i < MAX_BFS_STEP; i++ )
		{
			arr[i] = std::max(0.01, CalcTimeout(i));
		}
		return arr;
	}();

protected:
	Ui::OverflowPaletteGamePanelWidget* ui;
};
#endif // OVERFLOWPALETTEGAMEPANELWIDGET_H
