#ifndef GAMEEDITORMAINWIDGET_H
#define GAMEEDITORMAINWIDGET_H

#include <QWidget>

namespace Ui
{
class GameEditorMainWidget;
}

class GameEditorMainWidget : public QWidget
{
	Q_OBJECT

public:
	explicit GameEditorMainWidget(QWidget* parent = nullptr);
	~GameEditorMainWidget();

	QString GetWindowTitleForEditingTab();

private:
	Ui::GameEditorMainWidget* ui;
};

#endif // GAMEEDITORMAINWIDGET_H
