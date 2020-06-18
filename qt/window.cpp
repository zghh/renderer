#include "window.h"
#include <QDebug>

Window::Window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Window)
{
    ui->setupUi(this);

    connect(ui->imageLabel, SIGNAL(wheel(QWheelEvent *)), this, SLOT(Scale(QWheelEvent *)));
    connect(ui->imageLabel, SIGNAL(pressed(QMouseEvent *)), this, SLOT(MoveOrRotateStart(QMouseEvent *)));
    connect(ui->imageLabel, SIGNAL(moved(QMouseEvent *)), this, SLOT(MoveOrRotate(QMouseEvent *)));

    ui->globalIlluminationLabel->setAlignment(Qt::AlignCenter);
    ui->localIlluminationLabel->setAlignment(Qt::AlignCenter);

    this->picture = new Picture();
    this->isLoaded = false;
}

Window::~Window()
{
    if (this->isLoaded)
        delete this->picture;
    delete ui;
}

void Window::Draw()
{
    QRgb *pixels = this->picture->GetPixels();
    QImage *image = this->picture->GetImage();

    this->camera->GetPicture(pixels);

    ui->imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void Window::Scale(QWheelEvent *event)
{
    this->camera->Sacle(event->delta());
    this->Draw();
}

void Window::MoveOrRotateStart(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
        this->lastMousePosition = event->globalPos();
}

void Window::MoveOrRotate(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::RightButton))
    {
        const QPoint position = event->globalPos() - this->lastMousePosition;
        if (abs(position.x()) < 10 && abs(position.y()) < 10)
            return;
        this->lastMousePosition = event->globalPos();

        this->camera->Move(position.x() / 10, position.y() / 10);
    }
    else if (event->buttons().testFlag(Qt::LeftButton))
    {
        const QPoint position = event->globalPos() - this->lastMousePosition;
        if (abs(position.x()) < 10 && abs(position.y()) < 10)
            return;
        this->lastMousePosition = event->globalPos();

        this->camera->Rotate(position.x() / 10, position.y() / 10);
    }
    this->Draw();
}
void Window::on_loadObjectButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(NULL, "选择需要导入的模型", ".", "*.obj");
    if (fileName.isEmpty())
        return;
    if (this->isLoaded)
    {
        delete this->parser;
        delete this->kdTree;
        delete this->camera;
    }
    this->parser = new Parser(fileName.toStdString());
    this->parser->Load();
    this->kdTree = new KDTree(*this->parser);
    this->camera = new Camera(this->kdTree, this->parser);

    this->isLoaded = true;
    ui->projectionButton->setText(this->camera->GetProjectionState() ? "透视投影" : "正交投影");

    this->Draw();
}

void Window::on_renderButton_clicked()
{
    if (!this->isLoaded)
        return;

    QRgb *pixels = this->picture->GetPixelsWithGlobal();
    QImage *image = this->picture->GetImageWithGlobal();

    this->camera->GetPicture(pixels, true);

    ui->imageWithGlobalIlluminationLabel->setPixmap(QPixmap::fromImage(*image));
}

void Window::on_projectionButton_clicked()
{
    if (!this->isLoaded)
        return;
    this->camera->ChangeProjectionState();
    ui->projectionButton->setText(this->camera->GetProjectionState() ? "透视投影" : "正交投影");
    this->Draw();
}
