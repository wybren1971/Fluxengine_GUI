#ifndef SHOWLAYOUT_H
#define SHOWLAYOUT_H

#include <QDialog>

namespace Ui {
class showlayout;
}

class showlayout : public QDialog
{
    Q_OBJECT

public:
    explicit showlayout(QWidget *parent = nullptr);
    ~showlayout();

    void LoadFile(QString File);
private:
    Ui::showlayout *ui;
};

#endif // SHOWLAYOUT_H
