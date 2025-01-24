#include "GameEditorSideToolbox.h"
#include "GameEditorPanelWidget.h"
#include "ui_GameEditorSideToolbox.h"
#include <cassert>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStandardItemModel>
#include <QStandardItem>
#include <qabstractitemview.h>
#include <qtmetamacros.h>
#include "OverflowPaletteBackEnd.h"


class TargetColorBlockDelegate : public QStyledItemDelegate
{
public:
	explicit TargetColorBlockDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		painter->save();

		// 获取模型数据
		QString text = index.data(Qt::DisplayRole).toString(); // 左侧的字符串
		QColor color = index.data(Qt::UserRole).value<QColor>(); // 右侧的颜色块

		// 绘制背景
		if (option.state & QStyle::State_Selected) {
			painter->fillRect(option.rect, option.palette.highlight());
		}

		// 绘制左侧文字
		QRect textRect = option.rect.adjusted(5, 0, -50, 0); // 留出右侧区域
		painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);

		// 绘制右侧颜色块
		QRect colorRect = option.rect.adjusted(option.rect.width() - 45, 5, -5, -5); // 调整矩形位置和大小
		painter->setBrush(QBrush(color));
		painter->setPen(Qt::NoPen);
		painter->drawRect(colorRect);

		painter->restore();
	}

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		Q_UNUSED(option);
		Q_UNUSED(index);
		return QSize(200, 30); // 每一项的默认大小
	}
};

// static QSharedPointer<TargetColorBlockDelegate> ColorBlockDelegatePtr;

void AddComboBoxItem(QComboBox* InComboBox, const QString& InText, const QColor& InColor)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(InComboBox->model());
	QStandardItem* item = new QStandardItem(InText);
	item->setData(InColor, Qt::UserRole);
	model->appendRow(item);
}

GameEditorSideToolbox::GameEditorSideToolbox(QWidget* parent)
	: QWidget(parent), ui(new Ui::GameEditorSideToolbox)
{
	ui->setupUi(this);
	// this->setFixedWidth(270);

	// ColorBlockDelegatePtr = QSharedPointer<TargetColorBlockDelegate>(new TargetColorBlockDelegate(ui->ComboBox_TargetColor));
	TargetColorBlockDelegate* ColorBlockDelegatePtr = new TargetColorBlockDelegate(ui->ComboBox_TargetColor);
	ui->ComboBox_TargetColor->setItemDelegate(ColorBlockDelegatePtr);
    QStandardItemModel *model = new QStandardItemModel(ui->ComboBox_TargetColor);
	ui->ComboBox_TargetColor->setModel(model);

	ui->ComboBox_ColorFillMethod->view()->setMinimumWidth(150);
	ui->ComboBox_ColorFillMethod->view()->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

	connect(ui->SpinBox_MaxStep, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int InNewValue)
		{
			assert(OperatingConfigPtr);
			OperatingConfigPtr->MaxStep = InNewValue;
    		emit OnMaxStepChanged(InNewValue);
			emit OnGameConfigEdited(OperatingConfigPtr);
		});
	connect(ui->ComboBox_TargetColor, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int InNewIndex)
		{
			if (InNewIndex < 0)
				return;

			assert(OperatingConfigPtr);
			OperatingConfigPtr->TargetColorIndex = InNewIndex;
			
			QString& ColorDisplayName = OperatingConfigPtr->ColorPaletteName[InNewIndex];
			QColor& Color = OperatingConfigPtr->ColorPalette[InNewIndex];
			emit OnTargetColorChanged(ColorDisplayName, Color);

			emit OnGameConfigEdited(OperatingConfigPtr);
		});

	connect(ui->SpinBox_Rows, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int InNewValue)
		{
			assert(OperatingConfigPtr);
			SizeChangedDataMove(InNewValue, OperatingConfigPtr->Cols);
			OperatingConfigPtr->Rows = InNewValue;
			if (ui->CheckBox_AutoRebuild->isChecked())
			{
				emit OnRequestRebuild();
			}
			emit OnGameConfigEdited(OperatingConfigPtr);
		});
	connect(ui->SpinBox_Cols, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int InNewValue)
		{
			assert(OperatingConfigPtr);
			SizeChangedDataMove(OperatingConfigPtr->Rows, InNewValue);
			OperatingConfigPtr->Cols = InNewValue;
			if (ui->CheckBox_AutoRebuild->isChecked())
			{
				emit OnRequestRebuild();
			}
			emit OnGameConfigEdited(OperatingConfigPtr);
		});
	connect(ui->Btn_EditorRebuild, &QPushButton::clicked, this, [this]()
		{
			emit OnRequestRebuild();
		});
	

	connect(ui->ComboBox_ColorFillMethod, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int InNewIndex)
		{
			emit OnColorFillingMethodChanged(static_cast<EColorSettingMethod>(InNewIndex));
		});
	connect(ui->SpinBox_GrowthSize, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int InNewValue)
		{
			emit OnRandomGrowSizeChanged(InNewValue);
		});
	connect(ui->Btn_RemoveBtnColor, &QPushButton::clicked, this, [this]()
		{
			emit OnRemoveAllButtonsColor();
		});

	connect(ui->ColorEditorPanel, &ColorEditor::OnColorItemClicked, this, [this](int InIndex, const QString& InColorName, const QColor& InColor)
		{
			emit OnColorItemClicked(InIndex, InColorName, InColor);
		});
	connect(ui->ColorEditorPanel, &ColorEditor::OnNewColorAdded, this, [this](int InIndex, const QString& InColorName, const QColor& InColor)
		{
			RefreshTargetColorComboBox();
			emit OnRequestRebuild();
			emit OnGameConfigEdited(OperatingConfigPtr);
		});
	connect(ui->ColorEditorPanel, &ColorEditor::OnColorDeleted, this, [this](int InIndex, const QString& InColorName, const QColor& InColor)
		{
			for (int i = 0; i < OperatingConfigPtr->GameGridColors.size(); ++i)
			{
				if (OperatingConfigPtr->GameGridColors[i] == InIndex)
				{
					OperatingConfigPtr->GameGridColors[i] = INVALID_COLOR_INDEX;
				}
				else if (OperatingConfigPtr->GameGridColors[i] > InIndex)
				{
					OperatingConfigPtr->GameGridColors[i] -= 1;
				}
			}
			RefreshTargetColorComboBox();
			emit OnRequestRebuild();
			emit OnGameConfigEdited(OperatingConfigPtr);
		});
}

GameEditorSideToolbox::~GameEditorSideToolbox() { delete ui; }

void GameEditorSideToolbox::SizeChangedDataMove(int InNewRows, int InNewCols)
{
	const int NewSize = InNewRows * InNewCols;
	const QVector<int> OldData = OperatingConfigPtr->GameGridColors;

	const int LoopMaxRows = std::min(OperatingConfigPtr->Rows, InNewRows);
	const int LoopMaxCols = std::min(OperatingConfigPtr->Cols, InNewCols);

	OperatingConfigPtr->GameGridColors.clear();
	OperatingConfigPtr->GameGridColors.resize(NewSize);
	// Copy the old data to the new data by intersected coordinate.
	for (int i = 0; i < LoopMaxRows; ++i)
	{
		for (int j = 0; j < LoopMaxCols; ++j)
		{
			const int OldIndex = OperatingConfigPtr->CoordinateToIndex(i, j);
			const int NewIndex = i * InNewCols + j;
			OperatingConfigPtr->GameGridColors[NewIndex] = OldData[OldIndex];
		}
	}
}

void GameEditorSideToolbox::RefreshTargetColorComboBox()
{
	ui->ComboBox_TargetColor->clear();
	assert(OperatingConfigPtr->ColorPaletteName.size() == OperatingConfigPtr->ColorPalette.size());
	for (int i = 0; i < OperatingConfigPtr->ColorPaletteName.size(); ++i)
	{
		const QString& ColorName = OperatingConfigPtr->ColorPaletteName[i];
		AddComboBoxItem(ui->ComboBox_TargetColor, ColorName, OperatingConfigPtr->ColorPalette[i]);
	}
	ui->ComboBox_TargetColor->setCurrentIndex(OperatingConfigPtr->TargetColorIndex);
}

void GameEditorSideToolbox::Handle_OnOperatingConfigChanged(GamePanelConfig* InConfigPtr)
{
	OperatingConfigPtr = InConfigPtr;

	// Load the config data to the UI.

	// PAGE 1: Game Basic Settings:
	ui->SpinBox_MaxStep->setValue(InConfigPtr->MaxStep);

	RefreshTargetColorComboBox();

	// PAGE 2: Game Grid Settings:
	ui->SpinBox_Rows->setValue(InConfigPtr->Rows);
	ui->SpinBox_Cols->setValue(InConfigPtr->Cols);

	// PAGE 3: Color Filling Settings:
	for (int i = 0; i < ColorSettingMethodMap.size(); ++i)
	{
		ui->ComboBox_ColorFillMethod->addItem(ColorSettingMethodMap[static_cast<EColorSettingMethod>(i)]);
	}

	// PAGE 4: Color Editor:
	ui->ColorEditorPanel->InitFromConfig(InConfigPtr);
}
