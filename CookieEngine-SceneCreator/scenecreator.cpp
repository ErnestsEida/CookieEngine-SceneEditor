#include "scenecreator.h"
#include "ui_scenecreator.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QListWidgetItem>
#include <QGraphicsRectItem>
#include <QStatusBar>
#include <vector>
#include <QColor>
#include <QRgb>
#include <QRandomGenerator>

vector<QColor> allColors = {
    Qt::black,
    Qt::red,
    Qt::blue,
    Qt::green,
};

vector<Qt::BrushStyle> allPatterns = {
    Qt::BrushStyle::CrossPattern,
    Qt::BrushStyle::SolidPattern,
    Qt::BrushStyle::DiagCrossPattern,
    Qt::BrushStyle::Dense1Pattern,
    Qt::BrushStyle::Dense2Pattern,
    Qt::BrushStyle::Dense3Pattern,
    Qt::BrushStyle::Dense4Pattern,
    Qt::BrushStyle::Dense5Pattern,
    Qt::BrushStyle::Dense6Pattern,
    Qt::BrushStyle::Dense7Pattern,
    Qt::BrushStyle::VerPattern,
    Qt::BrushStyle::HorPattern,
    Qt::BrushStyle::BDiagPattern,
    Qt::BrushStyle::FDiagPattern,
};

vector<Brush> takenBrushes;

bool hasDuplicateBrush(Brush brush) {
    bool duplicateFlag = false;
    for(int i = 0; i < takenBrushes.size(); i++) {
        if(brush.color == takenBrushes[i].color && brush.pattern == takenBrushes[i].pattern) {
            duplicateFlag = true;
            break;
        }
    }
    return duplicateFlag;
}

Brush* getBrush() {
    QColor tColor = allColors.at(QRandomGenerator::global()->bounded((int)allColors.size()));
    Qt::BrushStyle tPattern = allPatterns.at(QRandomGenerator::global()->bounded((int) allPatterns.size()));
    Brush *readyBrush = new Brush(tColor, tPattern);
    while(takenBrushes.size() > 0 && hasDuplicateBrush(readyBrush)) {
        tColor = allColors.at(QRandomGenerator::global()->bounded((int)allColors.size()));
        tPattern = allPatterns.at(QRandomGenerator::global()->bounded((int) allPatterns.size()));
        readyBrush->color = tColor;
        readyBrush->pattern = tPattern;
    }
    return readyBrush;
}

SceneCreator::SceneCreator(QWidget *parent, QString path_to_gameobjects) :
    QDialog(parent),
    ui(new Ui::SceneCreator)
{
    ui->setupUi(this);
    this->path_to_gameobjects = path_to_gameobjects;
    this->LoadGameobjects(path_to_gameobjects);
    this->scene = new QGraphicsScene(this->ui->sceneEditor);
    this->ui->sceneEditor->setScene(this->scene);
}

void CheckForParams(QString line, Parameters* params) {
    // setCollider(width, height) method gives us objects collider
    if (line.contains("setCollider")) {
        QString width = line.split("(").last().split(")").first().replace(" ", "").split(",")[0];
        QString height = line.split("(").last().split(")").first().replace(" ", "").split(",")[1];
        params->insert("ColliderWidth", width);
        params->insert("ColliderHeight", height);
    }
}

void SceneCreator::LoadGameobjects(QString path_to_gameobjects) {
    // This is necessary so that refresh actually refreshes
    this->gameObjects.clear();
    this->objectBrushes.clear();

    // Get all .cpp files stored in given directory
    QDir dir(path_to_gameobjects);
    dir.setNameFilters(QStringList() << "*.cpp");
    QStringList files = dir.entryList();

    // iterate through each file, while checking if the class is inheriting from GameObject
    QFile *file;
    QString selectedGameobject;
    Parameters *params;
    foreach(QString filename, files) {
        // Reset data on new file
        params = new Parameters();
        selectedGameobject = NULL;
        file = new QFile(path_to_gameobjects + "/" + filename);

        if (file->open(QIODevice::ReadWrite)) {
            QTextStream s(file);
            while(!s.atEnd()) {
                QString line = s.readLine();
                // Set value for selectedGameobject when class definition is found in file
                if (line.contains("public GameObject")) {
                    selectedGameobject = line.split(":")[0].split(" ")[1];
                }

                // Set parameters for class
                CheckForParams(line, params);
            }

            // Add finished gameobject to map
            this->gameObjects.insert(selectedGameobject, *params);
            Brush *brush = getBrush();
            this->objectBrushes.insert(selectedGameobject, brush);
        }
    }
    this->ReloadObjectList(this->gameObjects);
}

void SceneCreator::ReloadObjectList(QMap<QString, Parameters> objects) {
    this->ui->objectList->clear();
    for(int i = 0; i < objects.keys().size(); i++) {
        this->ui->objectList->insertItem(i, objects.keys()[i]);
    }
}

SceneCreator::~SceneCreator()
{
    delete ui;
}

void SceneCreator::on_refreshButton_clicked()
{
    this->LoadGameobjects(this->path_to_gameobjects);
}


void SceneCreator::on_objectList_currentRowChanged(int currentRow)
{
    this->selectedObject = currentRow;
    this->UpdateStatusbar();
}

void SceneCreator::UpdateStatusbar() {

}

void SceneCreator::on_insertButton_clicked()
{
    if (this->selectedObject >= 0) {
        Brush *drawingBrush = new Brush(this->objectBrushes[this->gameObjects.keys()[this->selectedObject]]);
        QPen pen(drawingBrush->color);
        QBrush brush(drawingBrush->color, drawingBrush->pattern);
        QGraphicsRectItem* item = this->scene->addRect(0, 0, this->gameObjects.value(this->gameObjects.keys()[this->selectedObject])["ColliderWidth"].toInt(), this->gameObjects.value(this->gameObjects.keys()[this->selectedObject])["ColliderHeight"].toInt(), pen, brush);
        item->setFlag(QGraphicsItem::ItemIsMovable);
    }
}
