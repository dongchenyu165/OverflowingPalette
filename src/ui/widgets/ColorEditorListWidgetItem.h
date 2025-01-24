#ifndef COLOREDITORLISTWIDGETITEM_H
#define COLOREDITORLISTWIDGETITEM_H

#include <QApplication>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QColor>

class ColorEditorListWidgetItem : public QWidget
{
public:
	explicit ColorEditorListWidgetItem(
		const QString& InText, const QColor& InColor, QWidget* InParent = nullptr
	)
		: DisplayColorText(InText), QWidget(InParent)
	{
		// Create layout
		auto* Layout = new QHBoxLayout(this);
		Layout->setContentsMargins(5, 5, 5, 5);
		Layout->setSpacing(10);

		// Create label for displaying text
		auto* Label = new QLabel(InText, this);
		Layout->addWidget(Label);

		// Create frame for displaying color
		auto* ColorFrame = new QFrame(this);
		ColorFrame->setFixedSize(20, 20); // Set the size of color frame
		ColorFrame->setStyleSheet(
			QString("background-color: %1; border: 1px solid black;").arg(InColor.name())
		);
		Layout->addWidget(ColorFrame);

		setLayout(Layout);
	}

	~ColorEditorListWidgetItem()
	{
		qDebug() << QString("Destory: [%1]").arg(DisplayColorText);
	}

private:
	QString DisplayColorText;
};

#endif // COLOREDITORLISTWIDGETITEM_H
