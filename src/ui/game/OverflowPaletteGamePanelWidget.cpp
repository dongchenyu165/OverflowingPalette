#include "OverflowPaletteGamePanelWidget.h"
#include "OverflowPaletteBackEnd.h"
#include <QTimer>
#include <qboxlayout.h>
#include <qframe.h>
#include <qlayoutitem.h>
#include <qlogging.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qscopedpointer.h>
#include "ui_OverflowPaletteGamePanelWidget.h"
#include <QMessageBox>
#include <qsharedpointer.h>
#include <qtmetamacros.h>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QThread>




OverflowPaletteGamePanelWidget::OverflowPaletteGamePanelWidget(QWidget* InParent)
	: QWidget(InParent),
	ui(new Ui::OverflowPaletteGamePanelWidget)
{
	ui->setupUi(this);

	OuterVerticalLayout = new QVBoxLayout(this);
	GridButtonContainer = new QGridLayout(this);
	ColorPickerContainer = new QHBoxLayout(this);


	ui->GameContainer->setLayout(OuterVerticalLayout);
	ui->GameContainer->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

	OuterVerticalLayout->addLayout(ColorPickerContainer);
	OuterVerticalLayout->addLayout(GridButtonContainer);

	connect(ui->Btn_ResetGame, &QPushButton::clicked, this, &OverflowPaletteGamePanelWidget::ResetGame);
}

void OverflowPaletteGamePanelWidget::Handle_OnConfigChanged(GamePanelConfig* InConfigPtr)
{
	ui->Label_Help->hide();
	ConfigPtr = InConfigPtr;
	GenerateGamePanel(*InConfigPtr);
	PlayingGameConfig = *ConfigPtr;

	QTimer::singleShot(400, [this]()
	{
		ConfigPtr->PrintGridColors("Init");
		ResetGame();
		ConfigPtr->PrintGridColors("After Init");
		PlayingGameConfig.PrintGridColors("PlayingGameConfig After Init");
	});
}

void OverflowPaletteGamePanelWidget::GenerateGamePanel(GamePanelConfig& InConfig)
{
	ConfigPtr = &InConfig;
	GenerateButtons(InConfig);

	Handle_OnMaxStepChanged(ConfigPtr->MaxStep);
	QString& ColorDisplayName = ConfigPtr->ColorPaletteName[ConfigPtr->TargetColorIndex];
	QColor& Color = ConfigPtr->ColorPalette[ConfigPtr->TargetColorIndex];
	Handle_OnTargetColorChanged(ColorDisplayName, Color);
}

void OverflowPaletteGamePanelWidget::__RemoveWidgetInContainer(QLayout* InOutContainerPtr)
{
	for (int i = 0; i < InOutContainerPtr->count(); ++i)
	{
		QLayoutItem* Item = InOutContainerPtr->takeAt(i);
		if (Item->widget())
		{
			Item->widget()->setParent(nullptr);
			delete Item->widget();
		}
		delete Item;
	}
}
void OverflowPaletteGamePanelWidget::__GenGridButtonsByConfig(const GamePanelConfig &InConfig, QLayout* InContainerPtr)
{
	for (int Row = 0; Row < ConfigPtr->Rows; ++Row)
	{
		for (int Col = 0; Col < ConfigPtr->Cols; ++Col)
		{
			int Index = ConfigPtr->CoordinateToIndex(Row, Col);

			QSharedPointer<QPushButton> Button = QSharedPointer<QPushButton>(new QPushButton(QString("%1 %2").arg(Row + 1).arg(Col + 1), this));
			Button->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
			Button->setFixedSize(GridButtonSize);
			// Button->setBaseSize(GridButtonSize);
			// Button->resize(GridButtonSize);

			connect(Button.get(), &QPushButton::clicked, this, &OverflowPaletteGamePanelWidget::Handle_OnPanelButtonClicked);
			OnGridButtonCreated(Button.get());
			GridButtonList.push_back(Button);
			GridButtonContainer->addWidget(Button.get(), Row, Col);

			if (InConfig.GameGridColors[Index] == INVALID_COLOR_INDEX)
			{
				SetGridButtonColor(Button, QColor(255, 255, 255, 0));
				Button->setText("No\nColor");
				continue;
			}
			SetGridButtonColor(Button, InConfig.ColorPalette[InConfig.GameGridColors[Index]]);
		}
	}

	// Update the grid column size.
	GridButtonCols = ConfigPtr->Cols;
}


void OverflowPaletteGamePanelWidget::SetGridButtonColor(QSharedPointer<QPushButton> InButton, const QColor& InColor)
{
	InButton->setStyleSheet(QString("QPushButton {"
									"    border-width: 3px;"
									"    border-style: solid;"
									"    border-color: %1;"
									"    border-radius: 10px;"
									"    background-color: %2;"
									"    color: black;"
									"}"
									"QPushButton:hover {"
									"    background-color: %3;"
									"}")
								.arg(InColor.darker(160).name())
								.arg(InColor.name())
								.arg(InColor.lighter(160).name()));
}

void ChangeWidgetStyleSheet(QWidget* InWidget, const std::map<QString, QString>& InStyleStringMap)
{
	QString StyleString = InWidget->styleSheet();

	for (const auto& [Key, Value] : InStyleStringMap)
	{
		const QString PropertyString = QString("%1: ").arg(Key);
		int ReplaceStart = StyleString.indexOf(PropertyString) + PropertyString.size();
		int ReplaceEnd = StyleString.indexOf(";", ReplaceStart);
		StyleString.erase(StyleString.cbegin() + ReplaceStart, StyleString.cbegin() + (ReplaceEnd));
		StyleString.insert(ReplaceStart, Value);
	}

	InWidget->setStyleSheet(StyleString);
}

void OverflowPaletteGamePanelWidget::__SetColorPickerBtnStyleToNormal(QPushButton* InButton)
{
	static const std::map<QString, QString> StyleMap = {
		{ "border-width",  "2px"  },
        { "border-radius", "30px" }
	};
	InButton->setFixedSize(ColorPickerButtonNormalSize);
	// InButton->resize(ColorPickerButtonNormalSize);

	ChangeWidgetStyleSheet(InButton, StyleMap);
}

void OverflowPaletteGamePanelWidget::__SetColorPickerBtnStyleToSelecting(QPushButton* InButton)
{
	static const std::map<QString, QString> StyleMap = {
		{ "border-width",  "8px"  },
        { "border-radius", "40px" }
	};
	InButton->setFixedSize(ColorPickerButtonSelectingSize);
	// InButton->resize(ColorPickerButtonSelectingSize);
	// InButton->setBaseSize(ColorPickerButtonSelectingSize);

	QString OriginStyleString = InButton->styleSheet();
	ChangeWidgetStyleSheet(InButton, StyleMap);
}

void OverflowPaletteGamePanelWidget::SetColorPickerButtonColor(
	QSharedPointer<QPushButton> InButton, const QColor& InColor
)
{
	InButton->setStyleSheet(QString("QPushButton {"
									"    border-width: 2px;"
									"    border-style: solid;"
									"    border-color: %1;"
									"    border-radius: 30px;"
									"    background-color: %2;"
									"    color: black;"
									"}"
									"QPushButton:hover {"
									"    background-color: %3;"
									"}")
								.arg(InColor.darker(160).name())
								.arg(InColor.name())
								.arg(InColor.lighter(160).name()));
}

void OverflowPaletteGamePanelWidget::__GenColorPickerButtonsByConfig(const GamePanelConfig &InConfig, QLayout* InContainerPtr)
{
	int Index = 0;
	for (auto ColorObj : InConfig.ColorPalette)
	{
		// Create button and set signal.
		QSharedPointer<QPushButton> Button = QSharedPointer<QPushButton>(new QPushButton(this));
		Button->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

		SetColorPickerButtonColor(Button, InConfig.ColorPalette[Index]);
		__SetColorPickerBtnStyleToNormal(Button.get());

		connect(Button.get(), &QPushButton::clicked, this, &OverflowPaletteGamePanelWidget::Handle_OnColorPickerButtonClicked);

		// Insert to container and save button pointers to list.
		ColorPickerButtonList.push_back(Button);
		ColorPickerContainer->addWidget(Button.get(), 0, Qt::AlignmentFlag::AlignCenter);

		Index++;
	}
	NewColorIndex = 0;
	__SetColorPickerBtnStyleToSelecting(ColorPickerButtonList[0].get());
}

void ReleaseWidgetFromLayout(QLayout* InLayout)
{
	if ( !InLayout )
	{
		return; // Validate the inputs
	}

	const int ChildWidgetCount = InLayout->count();

	// 遍历布局，查找目标控件
	for ( int Index = 0; Index < ChildWidgetCount; Index++ )
	{
		// 从布局中移除控件的管理权
		QLayoutItem* RemovedItem = InLayout->takeAt(0);
		if ( !RemovedItem )
		{
			continue; // 无效的布局项
		}

		delete RemovedItem; // 删除布局项（不删除控件）
	}
}

void OverflowPaletteGamePanelWidget::GenerateButtons(const GamePanelConfig &InConfig)
{
	ReleaseWidgetFromLayout(GridButtonContainer);
	GridButtonList.clear();
	__GenGridButtonsByConfig(InConfig, GridButtonContainer);

	ReleaseWidgetFromLayout(ColorPickerContainer);
	ColorPickerButtonList.clear();
	__GenColorPickerButtonsByConfig(InConfig, ColorPickerContainer);
}

void OverflowPaletteGamePanelWidget::Handle_OnPanelButtonClicked()
{
	if (this->bDisableAllBtns)
	{
		qDebug("FAILED, Changing color!!");
		return;
	}

	QPushButton* SenderButton = qobject_cast<QPushButton*>(sender());
	if (!SenderButton)
		return;

	this->bDisableAllBtns = true;

	for (int Index = 0; Index < GridButtonList.size(); ++Index)
	{
		if (GridButtonList[Index] != SenderButton)
			continue;

		int Row = Index / ConfigPtr->Cols;
		int Col = Index % ConfigPtr->Cols;
		qDebug("Button at (%d, %d) clicked!", Row + 1, Col + 1);

		OverflowPaletteBackEnd::BoardLevelListType LevelList;
		OverflowPaletteBackEnd::BFS_Iterator(PlayingGameConfig, Row, Col, LevelList, [](const int& InRow, const int& InCol, const int& InGridIndex, const bool& bIsEndLevel){});

		int IterateCount = 0;
		int ReferenceColorIndex = this->ConfigPtr->GameGridColors[this->ConfigPtr->CoordinateToIndex(Row, Col)];
		for (std::vector<int> LevelItemIndexList : LevelList)
		{
			const double ChangeTime = TIMER_TIMEOUT_TIME_LIST[IterateCount];
			TimerList.push_back(QSharedPointer<QTimer>::create());
			TimerList.back()->setProperty("ID", TimerList.size() - 1);
			qDebug() << QString("===== ChangeTime: [%1]s").arg(ChangeTime);

			QVariantList _LevelItemIndexList;
			for (int Index : LevelItemIndexList)
			{
				_LevelItemIndexList.append(Index);
			}
			TimerList.back()->setProperty("LevelItemIndexList", _LevelItemIndexList); // 绑定属性

			connect(TimerList.back().get(), &QTimer::timeout, this, &OverflowPaletteGamePanelWidget::OnTimerFinished);

			TimerList.back()->start(ChangeTime * 1000);

			IterateCount++;
		}
		return;
	}
}

void OverflowPaletteGamePanelWidget::OnTimerFinished()
{
	QTimer* SenderTimer = qobject_cast<QTimer*>(QObject::sender());
	assert(SenderTimer);
	auto LevelItemIndexList = SenderTimer->property("LevelItemIndexList").toList();

	for (int i = 0; i < LevelItemIndexList.size(); i++)
	{
		const int& IndexRef = LevelItemIndexList[i].toInt();
		SetGridButtonColor(GridButtonList[IndexRef], this->ConfigPtr->ColorPalette[this->NewColorIndex]);
		ExecButtonAnim(GridButtonList[IndexRef].get(), 0);

		PlayingGameConfig.GameGridColors[IndexRef] = this->NewColorIndex;
	}

	int PendingRemoveTimerIndex = -1;
	for (int i = 0; i < this->TimerList.size(); i++)
	{
		QSharedPointer<QTimer>& SearchingTimer = this->TimerList[i];
		if (SenderTimer->property("ID").toInt() == SearchingTimer->property("ID").toInt())
		{
			PendingRemoveTimerIndex = i;
			break;
		}
	}
	this->TimerList.erase(this->TimerList.begin() + PendingRemoveTimerIndex);

	if (this->TimerList.size() == 0)
	{
		qDebug("Finish change color!");
		this->bDisableAllBtns = false;
		OnUserOperationFinished();
	}
}

void OverflowPaletteGamePanelWidget::OnUserOperationFinished()
{
	ConfigPtr->PrintGridColors("After Clicked");
	PlayingGameConfig.PrintGridColors("PlayingGameConfig After Clicked");

	bool bIsWin = true;
	for (int i = 0; i < ConfigPtr->GameGridColors.size(); i++)
	{
		if (PlayingGameConfig.GameGridColors[i] != ConfigPtr->TargetColorIndex)
		{
			bIsWin = false;
			break;
		}
	}

	RemainStep--;
	Handle_OnMaxStepChanged(RemainStep);

	if (!bIsWin && RemainStep > 0)
	{
		return;
	}

	this->bDisableAllBtns = true;
	QString Message = bIsWin ? "You Win!" : "You Lose! The game'll reset.";
	QMessageBox::information(this, "Game Finish", Message);
	ResetGame();
	this->bDisableAllBtns = false;
}

QRect ScaleGeometryFromCenter(const QRect& InGeometry, float ScaleFactor)
{
	const int NewWidth = (InGeometry.width() * ScaleFactor);
	const int NewHeight = (InGeometry.height() * ScaleFactor);
	auto ress = QRect{
		InGeometry.left() - (NewWidth - InGeometry.width()) / 2,
		InGeometry.top() - (NewHeight - InGeometry.height()) / 2,
		NewWidth,
		NewHeight
	};
	// qDebug() << QString("Old: %1 %2 %3 %4").arg(InGeometry.left()).arg(InGeometry.top()).arg(InGeometry.width()).arg(InGeometry.height());
	// qDebug() << QString("New: %1 %2 %3 %4").arg(ress.left()).arg(ress.top()).arg(ress.width()).arg(ress.height());

	return ress;
}


void OverflowPaletteGamePanelWidget::ExecButtonAnim(QPushButton* InBtn, int InStartTime, std::function<void()> InStartAnimCallback )
{
	if ( bool bHasAnimObj =
				InBtn->children().size() > 0 &&
				qobject_cast<QPropertyAnimation*>(InBtn->children()[0]) )
	{
		QPropertyAnimation* AnimObj = qobject_cast<QPropertyAnimation*>(InBtn->children()[0]);
		QTimer::singleShot(InStartTime, [AnimObj, InBtn, InStartAnimCallback]() {
			if (InStartAnimCallback)
			{
				InStartAnimCallback();
			}
			AnimObj->start();
		});
	}
	else
	{
		QPropertyAnimation* AnimObj =
			new QPropertyAnimation(InBtn, "geometry");
		AnimObj->setDuration(200);                       // 动画持续时间

		AnimObj->setStartValue(InBtn->geometry()); // 起始大小
		AnimObj->setEndValue(ScaleGeometryFromCenter(InBtn->geometry(), 1.2f));     // 目标大小

		AnimObj->setEasingCurve(QEasingCurve::InOutQuad); // Amin curve
		// Make animation forward and backward once.
		QObject::connect(
			AnimObj,
			&QPropertyAnimation::finished,
			[AnimObj]()
			{
				if ( AnimObj->direction() == QAbstractAnimation::Forward )
				{
					AnimObj->setDirection(QAbstractAnimation::Backward);
					AnimObj->start();
				}
				else if ( AnimObj->direction() == QAbstractAnimation::Backward )
				{
					AnimObj->setDirection(QAbstractAnimation::Forward);  // Reset the direction.
				}
			}
		);
		AnimObj->setLoopCount(1);                        // 无限循环
		// AnimObj->start(); // 启动动画
		// qDebug() << "Timer: " << InStartTime;
		QTimer::singleShot(InStartTime, [AnimObj, InBtn, InStartAnimCallback]() {
			if (InStartAnimCallback)
			{
				InStartAnimCallback();
			}
			AnimObj->start();
		});
		AnimObj->setParent(InBtn);
	}
}


void OverflowPaletteGamePanelWidget::ResetGame()
{
	if (ConfigPtr == nullptr)
	{
		return;
	}
	ConfigPtr->PrintGridColors("Before IN ResetGame()");
	PlayingGameConfig.PrintGridColors("PlayingGameConfig Before RESET ResetGame()");

	PlayingGameConfig = *ConfigPtr;  // Copy(Reset) the config.
	RemainStep = PlayingGameConfig.MaxStep;

	PlayingGameConfig.PrintGridColors("PlayingGameConfig After COPY RESET ConfigPtr");
	
	for (int i = 0; i < PlayingGameConfig.GameGridColors.size(); i++)
	{
		// SetGridButtonColor(GridButtonList[i], ConfigPtr->ColorPalette[ConfigPtr->GameGridColors[i]]);
		QColor ResetColor = ConfigPtr->ColorPalette[ConfigPtr->GameGridColors[i]]; 
		auto Btn = GridButtonList[i];
		ExecButtonAnim(GridButtonList[i].get(), (i / PlayingGameConfig.Cols) * 20 + i * 15, 
			[this, i, Btn, ResetColor]()
			{
				qDebug() << QString("Resetting %1; ").arg(i) << "Addr: 0x" << (GridButtonList[i].get());
				SetGridButtonColor(GridButtonList[i], ResetColor);
			}
		);
	}

	ConfigPtr->PrintGridColors("【ConfigPtr】 After RESET ResetGame()");
	PlayingGameConfig.PrintGridColors("PlayingGameConfig After RESET ResetGame()");
	Handle_OnMaxStepChanged(RemainStep);
}


void OverflowPaletteGamePanelWidget::Handle_OnColorPickerButtonClicked()
{
	QPushButton* SenderButton = qobject_cast<QPushButton*>(sender());
	if (!SenderButton)
		return;

	for (int Index = 0; Index < ColorPickerButtonList.size(); ++Index)
	{
		if (ColorPickerButtonList[Index] != SenderButton)
		{
			__SetColorPickerBtnStyleToNormal(ColorPickerButtonList[Index].get());
			continue;
		}

		qDebug("Button at (%d) clicked!", Index);
		NewColorIndex = Index;
		__SetColorPickerBtnStyleToSelecting(ColorPickerButtonList[Index].get());
	}
	return;
}


void OverflowPaletteGamePanelWidget::Handle_OnTargetColorChanged(const QString& InColorName, QColor InColor)
{
	// Btn_Color
	ui->Btn_Color->setFixedSize(60 ,30);
	ui->Label_ColorName->setText(InColorName);
	ui->Btn_Color->setStyleSheet(
		QString("background-color: %1; border-radius: 7px;").arg(InColor.name())
	);
}

void OverflowPaletteGamePanelWidget::Handle_OnMaxStepChanged(int InNewMaxStep)
{ 
	ui->Label_MaxStep->setText(QString::number(InNewMaxStep));
}


