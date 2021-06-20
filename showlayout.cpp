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
    ui->lbldisplay->setScaledContents(true);
    ui->lbldisplay->setPixmap(pixmapTarget);
//    int w = ui->lbldisplay->width();
//    int h = ui->lbldisplay->height();
//    ui->lbldisplay->setPixmap(pixmapTarget.scaled(w,h,Qt::KeepAspectRatio));
}
