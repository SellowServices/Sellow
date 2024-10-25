#ifndef HUB_H
#define HUB_H

#include <QMainWindow>
#include <QPushButton>
#include <vector>
#include <string>
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
    std::string gameSelected = "None";
    void ensureFolderStructure();
    void ensureFolder(const QString &folderPath);
    void ensureConfigFile(const QString &filePath);

    void loadTrainerLibrary();
    void loadModsForTrainer(const QString &trainerFullPath);

    explicit Hub(QWidget *parent = nullptr);  // Use 'explicit' to avoid unintentional implicit conversions
    ~Hub();

private slots:

    void on_libraryButton_clicked();

    void on_shopButton_clicked();

    void on_settingsButton_clicked();

    void on_libraryLaunch_clicked();

private:

    Ui::Hub *ui;                              // UI pointer for managing widgets
};

#endif // HUB_H
