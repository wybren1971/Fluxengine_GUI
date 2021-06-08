#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "wizard.h"
#include <qsettings.h>
#include <dialogpreferences.h>
#include <QApplication>

/*
 * To Do:
 * Aanpassen aan protobuf redesign V
 * inlezen van otpies uit protobuf tekstfiles
 * Vastleggen van type drives in preferences zodat goede settings gebruikt worden (40track HD 96tpi, 48 tpi ertc)
 *
 *
 *
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    m_fluxengine.setWorkingDirectory(settings.value("fluxengine").toString());
    restoreGeometry(settings.value("myWidget/geometry").toByteArray());
    restoreState(settings.value("myWidget/windowState").toByteArray());
    ui->setupUi(this);
    //m_fluxengine.setAddress(ui->plainTextEdit->text());
    connect(ui->btnStop,&QPushButton::clicked,&m_fluxengine,&fluxengine::stop);
    connect(&m_fluxengine,&fluxengine::output,this,&MainWindow::output);
    connect(ui->Fluxengineinput,&QLineEdit::returnPressed,this,&MainWindow::on_pushButton_clicked);
    connect(ui->Fluxengineinput, &QLineEdit::textChanged, this, &MainWindow::buttonenable);
    connect(&m_fluxengine,&fluxengine::enableFluxengineCommands,this,&MainWindow::enableFluxengineCommands);
    createActions();
    createMenus();
    setDrive();
    setWindowTitle(tr("Fluxengine-GUI"));
    int width = settings.value("WindowWidth").toInt();
    int height = settings.value("WindowHeight").toInt();
    this->resize(width, height);
    ui->btnReadDisk->setFocus();
    ui->plainTextEdit_2->completer();
    ReadItemList();
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(cutAct);
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::newFile()
{

}

void MainWindow::readdisk()
{
    int intDrive;
    if (m_fluxengine.busy())
         m_fluxengine.stop();
    if (ui->btnDrive0->isChecked())
    {
        intDrive= 0;
    } else
    {
        intDrive= 1;
    }

    wizard Fwizard(this, intDrive);


    if (Fwizard.exec() == QDialog::Accepted)
    {
        if (Fwizard.hasVisitedPage(3))
        {
            if (ui->plainTextEdit_2->findText(Fwizard.getData()) == -1)
            {
                ui->plainTextEdit_2->addItem(Fwizard.getData());
                ui->plainTextEdit_2->setCurrentIndex(ui->plainTextEdit_2->findText(Fwizard.getData()));
                m_fluxengine.setAddress(ui->plainTextEdit_2->currentText());
                WriteItemList();
            } else
            {
               ui->plainTextEdit_2->setCurrentIndex(ui->plainTextEdit_2->findText(Fwizard.getData()));
            }
           m_fluxengine.start();
        }
    }
}


void MainWindow::preference()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    DialogPreferences *form = new DialogPreferences();
    form->setWindowTitle("Preferences Fluxengine_GUI");
    form->exec();
    setDrive();
    m_fluxengine.setWorkingDirectory(settings.value("fluxengine").toString());

}

void MainWindow::undo()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Undo</b>"));
}

void MainWindow::redo()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Redo</b>"));
}

void MainWindow::cut()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Cut</b>"));
}

void MainWindow::copy()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Copy</b>"));
}

void MainWindow::paste()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Paste</b>"));
}

void MainWindow::bold()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Bold</b>"));
}

void MainWindow::italic()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Italic</b>"));
}

void MainWindow::leftAlign()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Left Align</b>"));
}

void MainWindow::rightAlign()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Right Align</b>"));
}

void MainWindow::justify()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Justify</b>"));
}

void MainWindow::center()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Center</b>"));
}

void MainWindow::setLineSpacing()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Format|Set Line Spacing</b>"));
}

void MainWindow::setParagraphSpacing()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Format|Set Paragraph Spacing</b>"));
}

void MainWindow::about()
{
    QString yourAppVersion = QCoreApplication::applicationVersion();
    QString datetime = QStringLiteral(__DATE__) + QStringLiteral(" ") + QStringLiteral(__TIME__);
 //   infoLabel->setText(tr("Invoked <b>Help|About</b>"));
    QMessageBox::about(this, tr("About Menu"),
            tr("This is the fluxengine-GUI created by Wybren van Duinen. "
               "Application version: ") +yourAppVersion + "\nBuilddate and time: " + datetime);
}

void MainWindow::setDrive()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    if (settings.value("drive0").toString() != "")
    {
        ui->btnDrive0->setText("0: " + settings.value("drive0").toString());
    }
    if (settings.value("drive1").toString() != "")
    {
        if (settings.value("drive1").toString() == "No drive 1 present")
        {
            ui->groupBox->hide();
        } else
        {
            ui->groupBox->show();
            ui->btnDrive1->setText("1: " + settings.value("drive1").toString());
        }
    }
}

void MainWindow::createActions()
{
//    newAct = new QAction(tr("&Set location of Fluxengine"), this);
//    newAct->setShortcuts(QKeySequence::Save);
//    newAct->setStatusTip(tr("Set the working directory of fluxengine"));
//    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Fluxengine wizard..."), this);
    openAct->setShortcuts(QKeySequence::Find);
    openAct->setStatusTip(tr("Read or write a disk"));
    connect(openAct, &QAction::triggered, this, &MainWindow::readdisk);

    preferenceAct = new QAction(tr("&Preference..."), this);
    preferenceAct->setShortcuts(QKeySequence::Print);
    preferenceAct->setStatusTip(tr("Set the preferences"));
    connect(preferenceAct, &QAction::triggered, this, &MainWindow::preference);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last operation"));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);

    boldAct = new QAction(tr("&Bold"), this);
    boldAct->setCheckable(true);
    boldAct->setShortcut(QKeySequence::Bold);
    boldAct->setStatusTip(tr("Make the text bold"));
    connect(boldAct, &QAction::triggered, this, &MainWindow::bold);

    QFont boldFont = boldAct->font();
    boldFont.setBold(true);
    boldAct->setFont(boldFont);

    italicAct = new QAction(tr("&Italic"), this);
    italicAct->setCheckable(true);
    italicAct->setShortcut(QKeySequence::Italic);
    italicAct->setStatusTip(tr("Make the text italic"));
    connect(italicAct, &QAction::triggered, this, &MainWindow::italic);

    QFont italicFont = italicAct->font();
    italicFont.setItalic(true);
    italicAct->setFont(italicFont);

    setLineSpacingAct = new QAction(tr("Set &Line Spacing..."), this);
    setLineSpacingAct->setStatusTip(tr("Change the gap between the lines of a "
                                       "paragraph"));
    connect(setLineSpacingAct, &QAction::triggered, this, &MainWindow::setLineSpacing);

    setParagraphSpacingAct = new QAction(tr("Set &Paragraph Spacing..."), this);
    setParagraphSpacingAct->setStatusTip(tr("Change the gap between paragraphs"));
    connect(setParagraphSpacingAct, &QAction::triggered,
            this, &MainWindow::setParagraphSpacing);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    leftAlignAct = new QAction(tr("&Left Align"), this);
    leftAlignAct->setCheckable(true);
    leftAlignAct->setShortcut(tr("Ctrl+L"));
    leftAlignAct->setStatusTip(tr("Left align the selected text"));
    connect(leftAlignAct, &QAction::triggered, this, &MainWindow::leftAlign);

    rightAlignAct = new QAction(tr("&Right Align"), this);
    rightAlignAct->setCheckable(true);
    rightAlignAct->setShortcut(tr("Ctrl+R"));
    rightAlignAct->setStatusTip(tr("Right align the selected text"));
    connect(rightAlignAct, &QAction::triggered, this, &MainWindow::rightAlign);

    justifyAct = new QAction(tr("&Justify"), this);
    justifyAct->setCheckable(true);
    justifyAct->setShortcut(tr("Ctrl+J"));
    justifyAct->setStatusTip(tr("Justify the selected text"));
    connect(justifyAct, &QAction::triggered, this, &MainWindow::justify);

    centerAct = new QAction(tr("&Center"), this);
    centerAct->setCheckable(true);
    centerAct->setShortcut(tr("Ctrl+E"));
    centerAct->setStatusTip(tr("Center the selected text"));
    connect(centerAct, &QAction::triggered, this, &MainWindow::center);

    alignmentGroup = new QActionGroup(this);
    alignmentGroup->addAction(leftAlignAct);
    alignmentGroup->addAction(rightAlignAct);
    alignmentGroup->addAction(justifyAct);
    alignmentGroup->addAction(centerAct);
    leftAlignAct->setChecked(true);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
//    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(preferenceAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    formatMenu = editMenu->addMenu(tr("&Format"));
    formatMenu->addAction(boldAct);
    formatMenu->addAction(italicAct);
    formatMenu->addSeparator()->setText(tr("Alignment"));
    formatMenu->addAction(leftAlignAct);
    formatMenu->addAction(rightAlignAct);
    formatMenu->addAction(justifyAct);
    formatMenu->addAction(centerAct);
    formatMenu->addSeparator();
    formatMenu->addAction(setLineSpacingAct);
    formatMenu->addAction(setParagraphSpacingAct);
}

MainWindow::~MainWindow()
{

    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("WindowWidth", MainWindow::width());
    settings.setValue("WindowHeight", MainWindow::height());
    delete ui;
    m_fluxengine.stop();
}

void MainWindow::output(QString data)
{
    ui->txtOutput->appendPlainText(data);
}

void MainWindow::enableFluxengineCommands(bool blnStarted)
{
//    qInfo() << Q_FUNC_INFO;
//    qInfo() << blnStarted;

    if (blnStarted)
    {
        ui->Fluxengineinput->setEnabled(true);
        ui->btnStop->setEnabled(true);
    } else
    {
        ui->Fluxengineinput->setEnabled(false);
        ui->btnStop->setEnabled(false);
    }
}

void MainWindow::on_btnReadDisk_clicked()
{
   MainWindow::readdisk();
}


void MainWindow::on_btntestVoltages_clicked()
{
    if (m_fluxengine.busy())
        m_fluxengine.stop();
    m_fluxengine.setAddress("test voltages");
    m_fluxengine.start();
}


void MainWindow::on_btntestbandwidth_clicked()
{
    if (m_fluxengine.busy())
         m_fluxengine.stop();
    m_fluxengine.setAddress("test bandwidth");
    m_fluxengine.start();

}


void MainWindow::on_btnRPM_clicked()
{
    if (ui->btnDrive0->isChecked())
    {
        m_fluxengine.setAddress("rpm -s drive:0");
    } else
    {
        m_fluxengine.setAddress("rpm -s drive:1");
    }

    if (m_fluxengine.busy())
         m_fluxengine.stop();
    m_fluxengine.start();
}


void MainWindow::on_btnDrive0_clicked()
{
}


void MainWindow::on_btnDrive1_clicked()
{
}

void MainWindow::ReadItemList()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    for (unsigned i = 0;i<10;i++)
    {
        QString setting = "Fluxengine.command";
        QString s = QString::number(i);
        setting = setting + s;
        if (settings.value(setting).toString() != "")
        {
            ui->plainTextEdit_2->addItem(settings.value(setting).toString());
            ui->plainTextEdit_2->setCurrentIndex(i);
        }
    }

}

void MainWindow::WriteItemList()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    //write to settings
    for (unsigned i = 0;i<10;i++)
    {
        QString setting = "Fluxengine.command";
        QString s = QString::number(i);
        setting = setting + s;
        if (settings.value(setting).toString() == "")
        {//i kleiner dan 10 and nog geen command.
            settings.setValue(setting,m_fluxengine.getAddress());
            break;
        } else
        {
            if (i == 9)
            {
                QString settingold = "Fluxengine.command";
                //overwrite move everything up and loose first command.
                //1 overschrijft 0, 2 overschrijft 1 3 overschrijft 2 etc 9 wordt toegevoegd
                for (unsigned i = 0;i<9;i++)
                {
                    QString s = QString::number(i);
                    QString t = QString::number(i+1);
                    settings.setValue(settingold +s,settings.value(settingold +t));
                }
                settings.setValue(setting,m_fluxengine.getAddress());
                ui->plainTextEdit_2->clear();
                ReadItemList();
                ui->plainTextEdit_2->setCurrentIndex(i);
            }
        }
    }

}
void MainWindow::on_bntStartFluxengine_clicked()
{
    if (m_fluxengine.busy())
         m_fluxengine.stop();
    //als command nog niet voorkomt voeg hem toe.
    if (ui->plainTextEdit_2->findText(m_fluxengine.getAddress()) == -1)
    {
        ui->plainTextEdit_2->addItem(m_fluxengine.getAddress());
        ui->plainTextEdit_2->setCurrentIndex(ui->plainTextEdit_2->findText(m_fluxengine.getAddress()));
        WriteItemList();
    }
    m_fluxengine.start();

}

void MainWindow::on_pushButton_clicked()
{
   // QString input;
    QByteArray Input;
    //input = ui->Fluxengineinput->text();
    if ((ui->Fluxengineinput->text() != "") and  (m_fluxengine.busy()))
    {
        Input = ui->Fluxengineinput->text().toUtf8();
        m_fluxengine.write(Input);
        ui->Fluxengineinput->clear();
    }
}


void MainWindow::on_Fluxengineinput_returnPressed()
{
    //action is handled in the on_pushbutton_clicked()
}

void MainWindow::buttonenable()
{
    if (((ui->Fluxengineinput->text()) != "")  and  (m_fluxengine.busy()))
    {
        ui->pushButton->setEnabled(true);
     } else
    {
        ui->pushButton->setEnabled(false);
    }
}

void MainWindow::on_plainTextEdit_2_editTextChanged(const QString &arg1)
{
    m_fluxengine.setAddress(arg1);
}

