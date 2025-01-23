#ifndef COLORNAMEINPUTDIALOG_H
#define COLORNAMEINPUTDIALOG_H


#include <QApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QColor>

class ColorNameInputDialog : public QDialog {
	Q_OBJECT

public:
	// Constructor
	ColorNameInputDialog(QWidget *InParent = nullptr)
		: QDialog(InParent), InputLineEdit(nullptr), HintLabel(nullptr), ColorDisplayLabel(nullptr) {
		setWindowTitle("Custom Input Dialog");

		// Main layout
		QVBoxLayout *MainLayout = new QVBoxLayout(this);

		// Hint and color display layout
		QHBoxLayout *HintLayout = new QHBoxLayout();
		HintLabel = new QLabel(this);
		HintLayout->addWidget(HintLabel);

		ColorDisplayLabel = new QLabel(this);
		ColorDisplayLabel->setFixedSize(40, 20);
		HintLayout->addWidget(ColorDisplayLabel);
		HintLayout->addStretch();
		MainLayout->addLayout(HintLayout);

		// Input line edit
		InputLineEdit = new QLineEdit(this);
		MainLayout->addWidget(InputLineEdit);

		// Buttons layout
		QHBoxLayout *ButtonLayout = new QHBoxLayout();
		QPushButton *OkButton = new QPushButton("OK", this);
		QPushButton *CancelButton = new QPushButton("Cancel", this);
		ButtonLayout->addStretch();
		ButtonLayout->addWidget(OkButton);
		ButtonLayout->addWidget(CancelButton);
		MainLayout->addLayout(ButtonLayout);

		// Connect signals and slots
		connect(OkButton, &QPushButton::clicked, this, &ColorNameInputDialog::accept);
		connect(CancelButton, &QPushButton::clicked, this, &ColorNameInputDialog::reject);
	}

	static QString ShowDialog(QWidget *InParent, const QString &InTitle, const QString &InLabel, QLineEdit::EchoMode InEchoMode,
							  const QString &InDefaultText, bool& OutOk, const QColor &InColor) {
		ColorNameInputDialog Dialog(InParent);

		// Set title, label, and default text
		Dialog.setWindowTitle(InTitle);
		Dialog.HintLabel->setText(InLabel);
		Dialog.InputLineEdit->setText(InDefaultText);
		Dialog.InputLineEdit->setEchoMode(InEchoMode);

		// Set color display
		QPixmap ColorPixmap(40, 20);
		ColorPixmap.fill(InColor);
		QPainter Painter(&ColorPixmap);
		Painter.setPen(Qt::black);
		Painter.drawRect(0, 0, 39, 19); // Add border
		Dialog.ColorDisplayLabel->setPixmap(ColorPixmap);

		// Show dialog and get result
		QString Result;
		if ( Dialog.exec() == QDialog::Accepted )
		{
			OutOk  = true;
			Result = Dialog.InputLineEdit->text();
		}
		else
		{
			OutOk = false;
		}
		return Result;
	}

private:
	QLabel *HintLabel;            // Label for hint text
	QLabel *ColorDisplayLabel;    // Label for displaying the color
	QLineEdit *InputLineEdit;     // Line edit for user input
};


#endif // COLORNAMEINPUTDIALOG_H
