#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include <QtWidgets>

class wizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Intro , Page_Read, Page_Write, Page_Advanced,
                 Page_Conclusion};

    wizard(QWidget *parent, int intDrive);
public slots:
    QString getData();

private slots:
    void showHelp();


private:


};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = nullptr);

    int nextId() const override;

private:
    QLabel *topLabel;
    QRadioButton *readRadioButton;
    QRadioButton *writeRadioButton;
};

class ReadPage : public QWizardPage
{
    Q_OBJECT

public:
    ReadPage(QWidget *parent = nullptr);

    int nextId() const override;
    void initializePage() override;
    bool isComplete() const override;

private slots:
    void browse();
    void browseflux();
    void updatedirectorybox(int index);
    void browsereadflux();
    void editDirectoryBox(QString dir);
    void editFluxBox(QString dir);
    void editFlux1Box(QString dir);
    void editLineBox(QString dir);
private:
    QLabel *nameLabel;
    QLabel *trackLabel;
    QLabel *HeadLineLabelStart;
    QLabel *trackLabelStart;
    QLabel *trackLabelStop;
    QLabel *headLabelStop;
    QLabel *label;
    QLabel *label1;
    QLabel *label2;

    QPushButton *button;
    QPushButton *button1;
    QPushButton *button2;
    QComboBox *readFormatbox;
    QLineEdit *directoryComboBox;
    QLineEdit *fluxComboBox;
    QLineEdit *flux1ComboBox;
    QLineEdit *nameLineEdit;
    QLineEdit *trackLineEdit;
    QLineEdit *HeadLineEditStart;
    QLineEdit *HeadLineEditStop;
    QLineEdit *trackLineEditStart;
    QLineEdit *trackLineEditStop;    

};

class WritePage : public QWizardPage
{
    Q_OBJECT

public:
    WritePage(QWidget *parent = nullptr);

    int nextId() const override;
    void initializePage() override;
    bool isComplete() const override;

private slots:
    void browse();
    void Update(int intIndex);
    void editDirectoryBox(QString dir);

    void editLineBox(QString dir);
private:
    QLabel *nameLabel;
    QLabel *trackLabel;
    QLabel *HeadLabelStart;
    QLabel *headLabelStop;
    QLabel *label;
    QLabel *trackLabelStart;
    QLabel *trackLabelStop;
    QLabel *DiskLabel;
    QLabel *namePresetLabel;
    QPushButton *button;
    QComboBox *writeFormatbox;
    QLineEdit *directoryComboBox;
    QComboBox *writePresetbox;
    QCheckBox *DiskLineEdit;
    QLineEdit *nameLineEdit;
    QLineEdit *trackLineEdit;
    QLineEdit *HeadLineEditStart;
    QLineEdit *HeadLineEditStop;
    QLineEdit *trackLineEditStart;
    QLineEdit *trackLineEditStop;
};

class AdvancedPage : public QWizardPage
{
    Q_OBJECT

public:
    AdvancedPage(QWidget *parent = nullptr);

    int nextId() const override;
    void initializePage() override;

private slots:
private:
    QLabel *bottomLabel;
};

//! [6]
class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = nullptr);

    void initializePage() override;
    int nextId() const override;

    QString getData();
private:
    QCheckBox *writeFluxFilecheckBox;
    QCheckBox *overwriteFluxFilecheckBox;
    QLabel *bottomLabel;
};
//! [6]

#endif
