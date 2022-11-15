#ifndef SCENECREATOR_H
#define SCENECREATOR_H

#include <QDialog>
#include <QtCore>
#include <vector>
#include <QtWidgets>

#include <sceneobject.h>

using namespace std;

typedef QMap<QString, QString> Parameters;

struct Brush {
    QColor color;
    Qt::BrushStyle pattern;
    Brush(QColor color, Qt::BrushStyle pattern) {
        this->color = color;
        this->pattern = pattern;
    }

    Brush(Brush *brush) {
        this->color = brush->color;
        this->pattern = brush->pattern;
    }
};

namespace Ui {
class SceneCreator;
}

class SceneCreator : public QDialog
{
    Q_OBJECT

public:
    explicit SceneCreator(QWidget *parent = nullptr, QString path_to_gameobjects = "");
    ~SceneCreator();

private slots:
    void on_refreshButton_clicked();

    void on_objectList_currentRowChanged(int currentRow);

    void on_insertButton_clicked();

    void on_clearButton_clicked();

private:
    void LoadGameobjects(QString path_to_gameobjects);
    void ReloadObjectList(QMap<QString, Parameters> objects);
    void UpdateStatusbar();
    void ClearScene();

    vector<SceneObject*> objectsInScene;
    QMap<QString, Brush*> objectBrushes;
    int selectedObject = -1;
    QGraphicsScene* scene;
    QString path_to_gameobjects;
    QMap<QString, Parameters> gameObjects;
    Ui::SceneCreator *ui;
};

#endif // SCENECREATOR_H
