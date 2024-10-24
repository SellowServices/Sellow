#ifndef HUB_H
#define HUB_H

#include <QMainWindow>
#include "injectortool.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Hub;
}
QT_END_NAMESPACE

class Hub : public QMainWindow
{
    Q_OBJECT

public:
    Hub(QWidget *parent = nullptr);
    ~Hub();

private slots:
    void on_injectButton_clicked();

    void on_fileSelectButton_clicked();

private:
    void exit_app();
    void loadActiveApplications();

    Ui::Hub *ui;
};
#endif // HUB_H
