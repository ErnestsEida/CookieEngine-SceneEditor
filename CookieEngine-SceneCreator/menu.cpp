#include "menu.h"
#include "ui_menu.h"
#include <QFileDialog>
#include <QDir>

Menu::Menu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Menu)
{
    ui->setupUi(this);
}

Menu::~Menu()
{
    delete ui;
}


void Menu::on_openButton_clicked() // OPEN BUTTON CLICKED
{
    QString path = QFileDialog::getExistingDirectory(this, "Select folder with GameObjects", QDir::homePath());
    this->sceneCreator = new SceneCreator(NULL, path);
    this->sceneCreator->open();
    this->close();
}

