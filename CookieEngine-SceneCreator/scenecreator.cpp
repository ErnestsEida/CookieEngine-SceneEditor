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
#include "globals.h"

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
    for(long unsigned int i = 0; i < takenBrushes.size(); i++) {
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
    this->DrawGrid();
    this->ui->gridSize->setDisabled(!this->ui->toggleGrid->isChecked());
    this->ui->toggleGrid->setChecked(Globals::snapToGrid);
    this->ui->gridSize->setValue(Globals::CELL_SIZE);
    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &SceneCreator::UpdateStatus);
    this->timer->start(100);
}

void SceneCreator::DrawGrid() {
    for(int i = 0; i < (int) this->gridLines.size(); i++) {
        this->scene->removeItem(this->gridLines[i]);
    }
    this->gridLines.clear();

    QPen pen(Qt::gray);
    int GridSize = Globals::CELL_SIZE;
    for(int i = 0; i < 100; i++){
        this->gridLines.push_back(this->scene->addLine(-1800, -1800 + (i*GridSize), 1800, -1800 + (i*GridSize), pen));
        this->gridLines.push_back(this->scene->addLine(-1800 + (i*GridSize), -1800, -1800 + (i*GridSize), 1800, pen));
    }
    QPen constructLines(Qt::black);
    constructLines.setWidth(2);
    this->gridLines.push_back(this->scene->addLine(0, 0, 1800, 0, constructLines));
    this->gridLines.push_back(this->scene->addLine(0, 0, 0, 1800, constructLines));
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

void SceneCreator::UpdateStatus() {
    if (SceneObject::selectedObject != NULL) {
        QString objectType = SceneObject::selectedObject->getObjectType();
        QString xPos = QString::number(SceneObject::selectedObject->pos().x());
        QString yPos = QString::number(SceneObject::selectedObject->pos().y());
        QString status = "OBJECT: " + objectType + " \t X: " + xPos + "\t Y: " + yPos;
        this->ui->statusLabel->setText(status);
    } else {
        this->ui->statusLabel->setText("Nothing selected");
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
        selectedGameobject = "";
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
            if (selectedGameobject != "") {
                this->gameObjects.insert(selectedGameobject, *params);
                Brush *brush = getBrush();
                this->objectBrushes.insert(selectedGameobject, brush);
            }
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
}

void SceneCreator::ClearScene() {
    foreach(SceneObject *item, this->objectsInScene){
        this->scene->removeItem(item);
    }
    this->objectsInScene.clear();
}

void SceneCreator::on_insertButton_clicked()
{
    if (this->selectedObject >= 0) {
        Brush *drawingBrush = new Brush(this->objectBrushes[this->gameObjects.keys()[this->selectedObject]]);
        QPen pen(drawingBrush->color);
        QBrush brush(drawingBrush->color, drawingBrush->pattern);
        SceneObject* object = new SceneObject(this->gameObjects.keys()[this->selectedObject] ,this->gameObjects.value(this->gameObjects.keys()[this->selectedObject])["ColliderWidth"].toInt(), this->gameObjects.value(this->gameObjects.keys()[this->selectedObject])["ColliderHeight"].toInt(), pen, brush);
        this->scene->addItem(object);
        this->objectsInScene.push_back(object);
    }
}

void SceneCreator::on_clearButton_clicked()
{
    this->ClearScene();
}


void SceneCreator::on_eraserButton_clicked() // ERASER
{
    Globals::isEraserOn = !Globals::isEraserOn;
    QString backgroundColor = Globals::isEraserOn ? "red" : "white";
    this->ui->eraserButton->setStyleSheet("background:" + backgroundColor);
    this->ui->insertButton->setDisabled(Globals::isEraserOn);
    this->ui->clearButton->setDisabled(Globals::isEraserOn);
}


void SceneCreator::on_toggleGrid_stateChanged(int arg1) // Grid enable/disable
{
    Globals::snapToGrid = arg1 == 0 ? false : true;
    this->ui->gridSize->setDisabled(!Globals::snapToGrid); // Subtract 2 because Checkbox status returns 2 when is off
}


void SceneCreator::on_gridSize_valueChanged(int arg1)
{
    Globals::CELL_SIZE = arg1;
    this->DrawGrid();
}

void SceneCreator::on_pushButton_clicked()
{
   bool ok;
   QString path_to_cookie = QFileDialog::getExistingDirectory(this, "Select CookieEngine folder", QDir::homePath());
   if (QDir(path_to_cookie).entryList().contains("Scene.hpp")) {
       if (QMessageBox::question(this, "Are you sure?", "Are you finished working on this scene and want to save it?") == QMessageBox::StandardButton::Yes) {
           QString sceneName = QInputDialog::getText(this, "Scene name", "Enter Scene name: ", QLineEdit::Normal, "", &ok);
           if (ok && !sceneName.isEmpty()) {
               // Get all objects before creating the scene
               QDir dir(path_to_gameobjects);
               dir.setNameFilters(QStringList() << "*.cpp");
               QStringList objects = dir.entryList();

               // Create scene
               QFile file(this->path_to_gameobjects + "/" + sceneName +".cpp");
               if (file.open(QIODevice::ReadWrite)) {
                   QTextStream s(&file);
                   // Some safety :)
                   s << "#pragma once" << "\n";

                   // Include all gameobjects ( might be useless, but for now will do )
                   foreach(QString gameobject, objects) {
                       s << "#include \"" << gameobject << "\"" << "\n";
                   }

                   s << "#include \"" << path_to_cookie+"/Scene.hpp" << "\"\n";

                   // Create class
                   s << "class " << sceneName << "{" << "\n";
                   s << "public:" << "\n";
                   s << "Scene* scene = new Scene(\"" << sceneName << "\", 1280, 720);" << "\n"; // PAY ATTENTION!! MIGHT HAVE TO ADD DIMENSION CUSTOMIZATION TOO!
                   s << sceneName << "() {\n"; // START OF CONSTRUCTOR

                   QList<QString> classes = this->gameObjects.keys();
                   foreach(QString xClass, classes) {
                       s << xClass << "* dynamic_" << xClass << ";\n";
                   }

                   for(int i = 0; i < this->objectsInScene.size(); i++) {
                       QString className = this->objectsInScene[i]->getObjectType();
                       s << "dynamic_" << className << " = new " << className << "(" << QString::number(this->objectsInScene[i]->pos().x()) << ", " << QString::number(this->objectsInScene[i]->pos().y()) << ");\n";
                       s << "this->scene->InsertObject(dynamic_" << className << ");\n";
                   }

                   s << "}\n"; // END OF CONSTRUCTOR

                   // Close class
                   s << "};";

                   file.close();
               }
           }
       }
   }
}

