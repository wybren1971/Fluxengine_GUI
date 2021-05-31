#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <fluxengine.h>
#include "wizard.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QWizard;
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

public slots:
protected:
    #ifndef QT_NO_CONTEXTMENU
        void contextMenuEvent(QContextMenuEvent *event) override;
    #endif // QT_NO_CONTEXTMENU

    //! [1]
    private slots:
        void newFile();
        void readdisk();
        void preference();
        void undo();
        void redo();
        void cut();
        void copy();
        void paste();
        void bold();
        void italic();
        void leftAlign();
        void rightAlign();
        void justify();
        void center();
        void setLineSpacing();
        void setParagraphSpacing();
        void about();
        void output(QString data);
        void enableFluxengineCommands(bool blnStarted);

        void on_plainTextEdit_textChanged();
        void on_btnReadDisk_clicked();

        void on_btntestVoltages_clicked();

        void on_btntestbandwidth_clicked();

        void on_btnRPM_clicked();

        void on_btnDrive0_clicked();

        void on_btnDrive1_clicked();

        void on_bntStartFluxengine_clicked();

        void on_pushButton_clicked();

        void on_Fluxengineinput_returnPressed();

        void buttonenable();

private:
        void createActions();
        void createMenus();
        Ui::MainWindow *ui;
        fluxengine m_fluxengine;
//        wizard m_readwizard;
        QMenu *fileMenu;
        QMenu *editMenu;
        QMenu *formatMenu;
        QMenu *helpMenu;
        QActionGroup *alignmentGroup;
        QAction *newAct;
        QAction *openAct;
        QAction *preferenceAct;
        QAction *exitAct;
        QAction *undoAct;
        QAction *redoAct;
        QAction *cutAct;
        QAction *copyAct;
        QAction *pasteAct;
        QAction *boldAct;
        QAction *italicAct;
        QAction *leftAlignAct;
        QAction *rightAlignAct;
        QAction *justifyAct;
        QAction *centerAct;
        QAction *setLineSpacingAct;
        QAction *setParagraphSpacingAct;
        QAction *aboutAct;
        QAction *aboutQtAct;
        void setDrive();
};
#endif
