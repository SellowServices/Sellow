#include "hub.h"
#include "./ui_hub.h"

#include <QFileDialog>
#include <QPushButton>
#include <QString>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSettings>
#include <QThread>

#include <windows.h>
#include <tlhelp32.h>
#include <vector>

Hub::Hub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Hub)
{
    ui->setupUi(this);

    connect(ui->trainerListWidget, &QListWidget::itemClicked, this, &Hub::onTrainerItemClicked);
    connect(ui->libraryListWidget, &QListWidget::itemClicked, this, &Hub::onLibraryItemClicked);

    ensureFolderStructure();
    loadTrainerLibrary();
}

Hub::~Hub()
{
    delete ui;
}

void Hub::ensureFolderStructure() {
    ensureFolder("/Data");
    ensureFolder("/Data/Trainers");

    // ensureConfigFile("/Data/Trainers/Config.cfg");
}

// Ensure a folder exists or create it
void Hub::ensureFolder(const QString &folderPath) {
    QString fullPath = QDir::currentPath() + folderPath;
    if (!QDir(fullPath).exists()) {
        if (QDir().mkpath(fullPath)) {
            QMessageBox::information(this, tr("Folder Created"), folderPath);
            // QMessageBox::information(this, tr("Folder Created"), tr("%1 was missing and has been created.").arg(folderPath));
        } else {
            // QMessageBox::critical(this, tr("Error"), tr("Failed to create %1.").arg(folderPath));
        }
    }
}

void Hub::loadTrainerShop() {

}

// Scan the Trainers folder and load trainers into the library view
void Hub::loadTrainerLibrary() {
    QString trainersPath = QDir::currentPath() + "/Data/Trainers";
    QDir trainersDir(trainersPath);
    QStringList trainerFolders = trainersDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (trainerFolders.isEmpty()) {
        QMessageBox::information(this, tr("No Trainers"), tr("No trainers found in the /Data/Trainers folder."));
        return;
    }

    ui->libraryListWidget->clear();

    for (const QString &trainerFolder : trainerFolders) {
        QString trainerFullPath = trainersPath + "/" + trainerFolder;
        QString configFilePath = trainerFullPath + "/Config.ini";

        if (!QFile::exists(configFilePath)) {
            qDebug() << "Config file missing for:" << trainerFolder;
            continue;  // Skip if Config.ini file doesn't exist
        }

        QSettings settings(configFilePath, QSettings::IniFormat);

        gameConfigsPath = configFilePath;

        qDebug() << "Loading config file from:" << configFilePath;
        QStringList settingsKeys = settings.allKeys();
        qDebug() << "Keys in config file:" << settingsKeys;

        if (!settings.contains("AppName")) {
            qDebug() << "AppName missing in Config.ini for:" << trainerFolder;
            continue;  // Skip if 'AppName' under 'General' section is missing
        }

        // Retrieve application information from the config file
        QString appName = settings.value("AppName").toString();  // Updated key to match INI file
        QString version = settings.value("Version").toString();  // Updated key to match INI file
        QString defaultLanguage = settings.value("DefaultLanguage").toString();  // Updated key to match INI file

        // Create an item for the trainer
        QListWidgetItem *trainerItem = new QListWidgetItem(trainerFolder);

        QString iconPath = QString(":/Resources/Cover/" + appName + ".jpg").arg(trainerFolder);
        QIcon trainerIcon(iconPath);

        trainerItem->setIcon(trainerIcon);
        trainerItem->setText(appName);
        trainerItem->setSizeHint(QSize(200, 100));
        ui->libraryListWidget->addItem(trainerItem);
    }
}

void Hub::loadModsForTrainer(const QString &trainerFullPath) {
    QDir modsDir(trainerFullPath + "/Mods");
    QStringList modFolders = modsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &modFolder : modFolders) {
        QString modFullPath = trainerFullPath + "/Mods/" + modFolder;

        QString configFilePath = modFullPath + "/Config.ini";
        bool hasConfig = QFile::exists(configFilePath);

        QListWidgetItem *modItem = new QListWidgetItem(modFolder);
        QString status = hasConfig ? "Config: Found" : "Config: Missing";
        modItem->setText("Mod: " + modFolder + "\n" + status);
        modItem->setSizeHint(QSize(200, 100));
        ui->modsListWidget->addItem(modItem);
    }
}

void Hub::on_libraryButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void Hub::on_shopButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void Hub::onLibraryItemClicked(QListWidgetItem *item) { // Game Opened
    ui->stackedWidget->setCurrentIndex(2);

    QSettings settings(gameConfigsPath, QSettings::IniFormat);

    if (settings.contains("Launcher/GameLauncher")) {
        QString fileLocation = settings.value("Launcher/GameLauncher").toString();

        ui->settingsGameLocation->setPlainText(fileLocation);
    }
    else {

    }

    ui->gamePreviewGameLabel->setText(item->text());
}

void Hub::on_settingsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Hub::onTrainerItemClicked(QListWidgetItem *item) { // Store Trainer Opened
    ui->stackedWidget->setCurrentIndex(4);

   ui->downloadItemNameLabel->setText(item->text());
}


void Hub::on_useSConnect_2_clicked(bool checked)
{
    ui->settingSellowUsername->setEnabled(checked);
}


void Hub::on_settingsLocateGame_clicked()
{
    QSettings settings(gameConfigsPath, QSettings::IniFormat);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Locate Game Executable"),
                                                    QString(),
                                                    tr("Executable Files (*.exe)"));

    if (!fileName.isEmpty()) {
        ui->settingsGameLocation->setPlainText(fileName);
        settings.setValue("Launcher/GameLauncher", fileName);
    } else {
        // None selected
    }
}

void Hub::on_gameLaunchButton_clicked() {
    qDebug() << "Launching game!";
    QSettings settings(gameConfigsPath, QSettings::IniFormat);
    QString fileLocation = settings.value("Launcher/GameLauncher").toString();
    QString gameTrainerDLL = settings.value("Launcher/GameTrainerDLL").toString();
    QString exactGameTitle = settings.value("Launcher/ExactGameTitle").toString();

    if (!fileLocation.isEmpty()) {
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (CreateProcess(fileLocation.toStdWString().c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            qDebug() << "Game launched successfully!";

            // TODO: Inject the game trainers DLL after setup

            // QThread::sleep(15);

            // if (!gameTrainerDLL.isEmpty() && QFile::exists(gameTrainerDLL)) {
            //     std::wstring dllPath = gameTrainerDLL.toStdWString();
            //     std::wstring windowTitle = exactGameTitle.toStdWString();

            //     InjectorTool injector(dllPath.c_str(), windowTitle.c_str());

            //     if (injector.inject()) {
            //         qDebug() << "DLL injection succeeded!";
            //     } else {
            //         qDebug() << "DLL injection failed!";
            //     }
            }

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

        } else {
            DWORD errorCode = GetLastError();
            qDebug() << "Failed to launch game! Error code:" << errorCode;
        }
    } else {
        qDebug() << "Game launcher path is empty!";
    }
}
