#include "showlayout.h"
#include "ui_showlayout.h"

showlayout::showlayout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showlayout)
{
    ui->setupUi(this);
}

showlayout::~showlayout()
{
    delete ui;
}

void showlayout::LoadFile(QString File)
{
    QPixmap pixmapTarget = QPixmap(File);
    //pixmapTarget = pixmapTarget.scaled(size-5, size-5, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->lbldisplay->setPixmap(pixmapTarget );
}
