#ifndef MENU_H
#define MENU_H

#include <QDialog>
#include <scenecreator.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Menu; }
QT_END_NAMESPACE

class Menu : public QDialog
{
    Q_OBJECT

public:
    Menu(QWidget *parent = nullptr);
    ~Menu();

private slots:
    void on_openButton_clicked();

private:
    Ui::Menu *ui;
    SceneCreator* sceneCreator;
};
#endif // MENU_H
