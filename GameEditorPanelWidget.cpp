#include "GameEditorPanelWidget.h"
#include "OverflowPaletteBackEnd.h"
#include "ui_OverflowPaletteGamePanelWidget.h"
#include <algorithm>
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstyle.h>
#include <QMouseEvent>

GameEditorPanelWidget::GameEditorPanelWidget(QWidget* InParent)
	: OverflowPaletteGamePanelWidget(InParent)
{
	ui->Btn_ResetGame->hide();
};

void GameEditorPanelWidget::Handle_OnConfigChanged(GamePanelConfig* InConfigPtr)
{
	ConfigPtr = InConfigPtr;
	ui->Label_Help->hide();
	GenerateGamePanel(*InConfigPtr);
}

void GameEditorPanelWidget::OnGridButtonCreated(QPushButton* InButton)
{
	// InButton->
	// connect(InButton, &QPushButton::enterEvent, this, [this, InButton]()
	// 	{
	// 		if (ColorSettingMethod == EColorSettingMethod::SingleCell)
	// 		{
	// 			InButton->setStyleSheet("background-color: rgba(255, 255, 255, 0.5);");
	// 		}
	// 	}
	// );
	// InButton->installEventFilter(this);
}

bool GameEditorPanelWidget::eventFilter(QObject* InObject, QEvent* InEvent)
{
	if (InEvent->type() != QEvent::Enter)
	{
		return QWidget::eventFilter(InObject, InEvent);
	}

	Qt::MouseButtons MouseState = QGuiApplication::mouseButtons();
	qDebug() << "MouseState: " << MouseState;
	bool bLeftBtnPressing = MouseState.testAnyFlag(Qt::LeftButton);
	if (!bLeftBtnPressing)
	{
		return QWidget::eventFilter(InObject, InEvent);
	}

	QPushButton* Button = qobject_cast<QPushButton*>(InObject);
	if (!Button)
	{
		return QWidget::eventFilter(InObject, InEvent);
	}

	if (ColorSettingMethod != EColorSettingMethod::SingleCell)
	{
		return QWidget::eventFilter(InObject, InEvent);
	}

	Button->click();
	return QWidget::eventFilter(InObject, InEvent);
}
// {
// 	if (InEvent->type() != QEvent::MouseButtonPress)
// 	{
// 		return QWidget::eventFilter(InObject, InEvent);
// 	}
	
// 	QPushButton* Button = qobject_cast<QPushButton*>(InObject);
	
// 	Button->click();
// 	return QWidget::eventFilter(InObject, InEvent);
// }

void GameEditorPanelWidget::mousePressEvent(QMouseEvent* InEvent)
{
	if ( InEvent->button() == Qt::LeftButton )
	{
		IsMousePressed = true;
	}
}

void GameEditorPanelWidget::mouseReleaseEvent(QMouseEvent* InEvent)
{
	if ( InEvent->button() == Qt::LeftButton )
	{
		IsMousePressed = false;
	}
}

void GameEditorPanelWidget::mouseMoveEvent(QMouseEvent* InEvent)
{
	if ( IsMousePressed )
	{
		QWidget* Child = childAt(InEvent->pos());
		QPushButton* Button = qobject_cast<QPushButton*>(Child);
		if (Child && !Button)
		{
			// Reset after leave the button.
			HoveringButton = nullptr;
			return;
		}
		if ( Button && !HoveringButton )
		{
			// Only triggering once.
			Button->click();
			HoveringButton = Button;
		}
	}
	else
	{
		// Reset after release the mouse button.
		HoveringButton = nullptr;
	}
}

void GameEditorPanelWidget::Handle_OnPanelButtonClicked()
{
	QPushButton* SenderButton = qobject_cast<QPushButton*>(sender());
	if (!SenderButton)
		return;

	emit OnGameConfigEdited(ConfigPtr);

	int ButtonIndex = -1;
	for (int Index = 0; Index < GridButtonList.size(); Index++)
	{
		if (GridButtonList[Index].get() != SenderButton)
			continue;

		ButtonIndex = Index;
		break;
	}

	switch ( ColorSettingMethod )
	{
	default:
	case EColorSettingMethod::SingleCell:
		SetGridButtonColor(GridButtonList[ButtonIndex], ConfigPtr->ColorPalette[NewColorIndex]);
		SenderButton->setText(ConfigPtr->ColorPaletteName[NewColorIndex]);
		ConfigPtr->GameGridColors[ButtonIndex] = NewColorIndex;
		break;
	case EColorSettingMethod::SingleRandomGrow:
		{
			std::vector<int> ContourIndexList = OverflowPaletteBackEnd::RandomContour(
				*ConfigPtr, ButtonIndex / ConfigPtr->Cols, ButtonIndex % ConfigPtr->Cols, RandomGrowSize
			);

			for ( int InGridIndex : ContourIndexList )
			{
				SetGridButtonColor(GridButtonList[InGridIndex], ConfigPtr->ColorPalette[NewColorIndex]);
				GridButtonList[InGridIndex]->setText(ConfigPtr->ColorPaletteName[NewColorIndex]);
				ConfigPtr->GameGridColors[InGridIndex] = NewColorIndex;
			}
		}
		break;
	case EColorSettingMethod::FullyRandom:
		{ 
			// Clear the color of the clicked button.
			for ( int Index = 0; Index < ConfigPtr->GameGridColors.size(); Index++ )
			{
				ConfigPtr->GameGridColors[Index] = INVALID_COLOR_INDEX;
			}

			std::vector<int> UnVisitedIndexList;
			for ( int Index = 0; Index < ConfigPtr->GameGridColors.size(); Index++ )
			{
				UnVisitedIndexList.push_back(Index);
			}

			// Use [OverflowPaletteBackEnd::RandomContour] to fill all of grids with random color.
			while ( !UnVisitedIndexList.empty() )
			{
				// Randomly select an element from the [UnVisitedIndexMap].
				int RandomIndex = -1;
				{
					int RandomElementIndex = rand() % UnVisitedIndexList.size();
					RandomIndex            = UnVisitedIndexList[RandomElementIndex];
					UnVisitedIndexList.erase(UnVisitedIndexList.begin() + RandomElementIndex);
				}

				int RandomRow = RandomIndex / ConfigPtr->Cols;
				int RandomCol = RandomIndex % ConfigPtr->Cols;

				int RandomColorIndex = rand() % ConfigPtr->ColorPalette.size();

				std::vector<int> ContourIndexList = OverflowPaletteBackEnd::RandomContour(
					*ConfigPtr, RandomRow, RandomCol, NewColorIndex
				);

				// Set the color of the contour to the random color.
				for ( int InGridIndex : ContourIndexList )
				{
					SetGridButtonColor(
						GridButtonList[InGridIndex], ConfigPtr->ColorPalette[RandomColorIndex]
					);
					GridButtonList[InGridIndex]->setText(ConfigPtr->ColorPaletteName[RandomColorIndex]);
					ConfigPtr->GameGridColors[InGridIndex] = RandomColorIndex;
				}

				// Remove the visited index in [ContourIndexList] from the [UnVisitedIndexSet].
				for ( int InGridIndex : ContourIndexList )
				{
					auto FoundIt = std::find(UnVisitedIndexList.begin(), UnVisitedIndexList.end(), InGridIndex);
					if (FoundIt != UnVisitedIndexList.end())
					{
						UnVisitedIndexList.erase(FoundIt);
					}
				}
			}
			break;
		}
	}
}

void GameEditorPanelWidget::Handle_OnColorPickerButtonClicked()
{
	QPushButton* SenderButton = qobject_cast<QPushButton*>(sender());
	if (!SenderButton)
		return;

	if (ColorPickerButtonList.size() != ConfigPtr->ColorPalette.size())
	{
		QMessageBox::critical(
			this,
			"Color picker button list size not match",
			"Color picker button list size not match with color palette size!\n"
			"Consider to check the [ColorPalette] in the game config JSON."  // TODO: Add a link to the JSON file.
		);
		qDebug("Color picker button list size not match with color palette size!");
		return;
	}

	emit OnGameConfigEdited(ConfigPtr);

	// Find the index of the clicked color picker button.
	for (int Index = 0; Index < ColorPickerButtonList.size(); Index++)
	{
		if (ColorPickerButtonList[Index] != SenderButton)
		{
			__SetColorPickerBtnStyleToNormal(ColorPickerButtonList[Index].get());
			continue;
		}

		__SetColorPickerBtnStyleToSelecting(ColorPickerButtonList[Index].get());
		NewColorIndex = Index;
	}
}

void GameEditorPanelWidget::Handle_OnRemoveAllColor()
{
	for (int i = 0; i < GridButtonList.size(); i++)
	{
		SetGridButtonColor(GridButtonList[i], QColor(255, 255, 255, 0));
		GridButtonList[i]->setText("No\nColor");
		ConfigPtr->GameGridColors[i] = INVALID_COLOR_INDEX;
	}
	
	emit OnGameConfigEdited(ConfigPtr);
}

void GameEditorPanelWidget::Handle_OnColorItemClicked(
	int InIndex, const QString& InColorName, const QColor& InColor
)
{
	for (int Index = 0; Index < ColorPickerButtonList.size(); Index++)
	{
		__SetColorPickerBtnStyleToNormal(ColorPickerButtonList[Index].get());
	}
	__SetColorPickerBtnStyleToSelecting(ColorPickerButtonList[InIndex].get());



	NewColorIndex = InIndex;
}
