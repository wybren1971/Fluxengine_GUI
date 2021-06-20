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
    ui->lbldisplay->setScaledContents(true);
    ui->lbldisplay->setPixmap(pixmapTarget);
}
