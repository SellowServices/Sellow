#ifndef HUB_H
#define HUB_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSettings>

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

    void onTrainerItemClicked(QListWidgetItem *item);
    void onLibraryItemClicked(QListWidgetItem *item);

    void loadTrainerShop();
    void loadTrainerLibrary();
    void loadModsForTrainer(const QString &trainerFullPath);

    explicit Hub(QWidget *parent = nullptr);  // Use 'explicit' to avoid unintentional implicit conversions
    ~Hub();

private slots:
    void on_libraryButton_clicked();
    void on_shopButton_clicked();
    void on_settingsButton_clicked();

    void on_useSConnect_2_clicked(bool checked);

    void on_settingsLocateGame_clicked();

    void on_gameLaunchButton_clicked();

private:
    QString gameConfigsPath;
    QString gameExecutablePath;

    Ui::Hub *ui;                              // UI pointer for managing widgets
};

#endif // HUB_H
