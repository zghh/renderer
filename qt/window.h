#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include "ui_window.h"
#include "picture.h"
#include "pictureLabel.h"
#include "../core/parser.h"
#include "../core/kdTree.h"
#include "../core/shader.h"
#include "../core/camera.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Window;
}
QT_END_NAMESPACE

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();
    void Draw();

public slots:
    void Scale(QWheelEvent *event);
    void MoveOrRotateStart(QMouseEvent *event);
    void MoveOrRotate(QMouseEvent *event);

private slots:
    void on_loadObjectButton_clicked();
    void on_renderButton_clicked();
    void on_projectionButton_clicked();

private:
    Ui::Window *ui;
    bool isLoaded;
    Picture *picture;
    Parser *parser;
    KDTree *kdTree;
    Camera *camera;
    QPoint lastMousePosition;
};
#endif // WINDOW_H
