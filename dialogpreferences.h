#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include <QDialog>
#include <fluxengine.h>

namespace Ui {
class DialogPreferences;
}

class DialogPreferences : public QDialog
{
    Q_OBJECT 
public:
    explicit DialogPreferences(QWidget *parent = nullptr);
    ~DialogPreferences();

private slots:
    void browse();
    void browseflux();
    void save();
    void browsefluxengine();
    void initializefluxengine();
    void output(QString data);
    void enablecommands(bool running);
    void browsecsv();
private:
    Ui::DialogPreferences *ui;
    fluxengine m_fluxengine;
    int State;
    QStringList formats;
    QStringList initializeformats(QString data);
    QStringList getConfig(QString data);
};

#endif // DIALOGPREFERENCES_H
