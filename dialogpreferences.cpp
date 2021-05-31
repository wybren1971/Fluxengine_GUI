#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"
#include <QtWidgets>

DialogPreferences::DialogPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPreferences)
{

    ui->setupUi(this);

    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    if (settings.value("datalocation").toString() != "")
    {
        ui->cmbdatalocation->addItem(settings.value("datalocation").toString());
    }

    if (settings.value("fluxlocation").toString() != "")
    {
        ui->cmbfluxlocation->addItem(settings.value("fluxlocation").toString());
    }

    QList<QRadioButton *> allRadioButtons = ui->groupBox_2->findChildren<QRadioButton*>();
    for (auto RadioButton : allRadioButtons) {
        if (settings.value("drive0").toString() == RadioButton->text())
        {
            RadioButton->setChecked(true);
        }
    }

    QList<QRadioButton *> allRadioButtons1 = ui->groupBox_3->findChildren<QRadioButton*>();
    for (auto RadioButton1 : allRadioButtons1) {
        if (settings.value("drive1").toString() == RadioButton1->text())
        {
            RadioButton1->setChecked(true);
        }
    }



    connect(ui->btndatalocation, SIGNAL(clicked()), SLOT(browse()));
    connect(ui->btnfluxlocation, SIGNAL(clicked()), SLOT(browseflux()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(save()));

}

DialogPreferences::~DialogPreferences()
{
    delete ui;
}


void DialogPreferences::browseflux()
{
//    QString strFilter;
//    QString strFile;

    QString directory = QFileDialog::getExistingDirectory(this,
                            tr("Set default data output directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!directory.isEmpty()) {
        if (ui->cmbfluxlocation->findText(directory) == -1)
            ui->cmbfluxlocation->addItem(directory);
        ui->cmbfluxlocation->setCurrentIndex(ui->cmbfluxlocation->findText(directory));
    }
}

void DialogPreferences::browse()
{
//    QString strFilter;
//    QString strFile;

    QString directory = QFileDialog::getExistingDirectory(this,
                            tr("Set default data output directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!directory.isEmpty()) {
        if (ui->cmbdatalocation->findText(directory) == -1)
            ui->cmbdatalocation->addItem(directory);
        ui->cmbdatalocation->setCurrentIndex(ui->cmbdatalocation->findText(directory));
    }
}

void DialogPreferences::save()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
   //do save action
//    qInfo() << "save dialog preferences";
//    qInfo() << ui->cmbdatalocation->currentText();

    QList<QRadioButton *> allRadioButtons = ui->groupBox_2->findChildren<QRadioButton*>();
    for (auto RadioButton : allRadioButtons) {
        if (RadioButton->isChecked())
        {
            settings.setValue("drive0", RadioButton->text());
            if (RadioButton->text().contains("40"))
            {
                //40 drive track
                settings.setValue("drive040track", true);
            } else
            {
                //not 40 drive track
                settings.setValue("drive040track", false);
            }
        }
    }

    QList<QRadioButton *> allRadioButtons1 = ui->groupBox_3->findChildren<QRadioButton*>();
    for (auto RadioButton1 : allRadioButtons1) {
        if (RadioButton1->isChecked())
        {
            settings.setValue("drive1", RadioButton1->text());
            if (RadioButton1->text().contains("40"))
            {
                //40 drive track
                settings.setValue("drive140track", true);
            } else
            {
                //not 40 drive track
                settings.setValue("drive140track", false);
            }
        }
    }

    settings.setValue("datalocation", ui->cmbdatalocation->currentText());
    settings.setValue("fluxlocation", ui->cmbfluxlocation->currentText());

}
