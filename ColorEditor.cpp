#include "ColorEditor.h"
#include "ui_ColorEditor.h"
#include <QColorDialog>
#include <QInputDialog>
#include "ColorNameInputDialog.h"
#include "ColorEditorListWidgetItem.h"

ColorEditor::ColorEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ColorEditor)
{
    ui->setupUi(this);

	connect(
		ui->Btn_Add,
		&QPushButton::clicked,
		this,
		[this]()
		{
			this->PickedColor = QColorDialog::getColor(Qt::white, this, "Add New Color");

			QString ColorDefaultName = QString("R%1-G%2-B%3")
										   .arg(this->PickedColor.red())
										   .arg(this->PickedColor.green())
										   .arg(this->PickedColor.blue());
			QString PromptMsg = QString("Input the name of Picked Color):");

			bool bIsClickOK          = false;
			QString ColorDisplayName = ColorNameInputDialog::ShowDialog(
				this,
				"Input the name",
				PromptMsg,
				QLineEdit::Normal,
				ColorDefaultName,
				bIsClickOK,
				this->PickedColor
			);

			if ( !bIsClickOK )
			{
				qDebug() << "User canceled the input.";
				return;
			}

			this->__AddColorItemToList(ColorDisplayName, this->PickedColor);

			ConfigPtr->ColorPalette.append(this->PickedColor);
			ConfigPtr->ColorPaletteName.append(ColorDisplayName);

			emit OnNewColorAdded(ConfigPtr->ColorPalette.size() - 1, ColorDisplayName, this->PickedColor);
		}
	);

	connect(
		ui->Btn_Del,
		&QPushButton::clicked,
		this,
		[this]()
		{
			const int SelectingIndex = ui->List_Colors->currentRow();
			ConfigPtr->ColorPalette.removeAt(SelectingIndex);
			ConfigPtr->ColorPaletteName.removeAt(SelectingIndex);

			ui->List_Colors->takeItem(SelectingIndex);

			emit OnColorDeleted(SelectingIndex, PickedColorName, PickedColor);
		}
	);

	connect(ui->List_Colors, &QListWidget::clicked, this, &ColorEditor::OnItemClicked);
}

ColorEditor::~ColorEditor()
{
    delete ui;
}

void ColorEditor::__AddColorItemToList(const QString& InText, const QColor& InColor)
{
	// 1. 创建 QListWidgetItem（只作为占位）
	QListWidgetItem* Item = new QListWidgetItem(ui->List_Colors);

	// 2. 创建自定义的 Widget
	auto* ItemWidget = new ColorEditorListWidgetItem(InText, InColor);

	// 3. 将自定义控绑定到条目
	Item->setSizeHint(ItemWidget->sizeHint());
	ui->List_Colors->addItem(Item);
	ui->List_Colors->setItemWidget(Item, ItemWidget);
}

void ColorEditor::OnItemClicked(const QModelIndex& InClickedIndex)
{
	const int SelectingIndex = ui->List_Colors->currentRow();
	PickedColor = ConfigPtr->ColorPalette[SelectingIndex];
	PickedColorName = ConfigPtr->ColorPaletteName[SelectingIndex];

	emit OnColorItemClicked(SelectingIndex, PickedColorName, PickedColor);

	qDebug() << "Color Name:" << PickedColorName << "  Color:" << PickedColor.name();
}

void ColorEditor::InitFromConfig(GamePanelConfig* InConfigPtr) {
	assert(InConfigPtr);
	ConfigPtr = InConfigPtr;

	ui->List_Colors->clear();

	for (int i = 0; i < ConfigPtr->ColorPalette.size(); i++)
	{
		__AddColorItemToList(ConfigPtr->ColorPaletteName[i], ConfigPtr->ColorPalette[i]);
	}
}
