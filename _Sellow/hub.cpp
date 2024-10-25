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

#include <windows.h>
#include <tlhelp32.h>
#include <vector>

Hub::Hub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Hub)
{
    ui->setupUi(this);

    // Check the folder structure on startup
    ensureFolderStructure();

    // Scan the Trainers folder and load trainer information into the Library section
    loadTrainerLibrary();
}

Hub::~Hub()
{
    delete ui;
}

// Main function to check and create folder structure
void Hub::ensureFolderStructure() {
    ensureFolder("/Data");
    ensureFolder("/Data/Trainers");
    ensureFolder("/Data/Trainers/Mods");
    // ensureConfigFile("/Data/Trainers/Config.cfg");
}

// Ensure a folder exists or create it
void Hub::ensureFolder(const QString &folderPath) {
    QString fullPath = QDir::currentPath() + folderPath;
    if (!QDir(fullPath).exists()) {
        if (QDir().mkpath(fullPath)) {
            // QMessageBox::information(this, tr("Folder Created"), tr("%1 was missing and has been created.").arg(folderPath));
        } else {
            // QMessageBox::critical(this, tr("Error"), tr("Failed to create %1.").arg(folderPath));
        }
    }
}

// Ensure the config file exists, create it with default content if not
void Hub::ensureConfigFile(const QString &filePath) {
    QString fullPath = QDir::currentPath() + filePath;
    if (!QFile::exists(fullPath)) {
        QFile configFile(fullPath);
        if (configFile.open(QIODevice::WriteOnly)) {
            configFile.write("# Default config\n");
            configFile.close();
            // QMessageBox::information(this, tr("Config Created"), tr("%1 was missing and has been created.").arg(filePath));
        } else {
            // QMessageBox::critical(this, tr("Error"), tr("Failed to create %1.").arg(filePath));
        }
    }
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

    // Clear the library widget before populating
    ui->libraryListWidget->clear();

    // Loop through each trainer folder
    for (const QString &trainerFolder : trainerFolders) {
        QString trainerFullPath = trainersPath + "/" + trainerFolder;

        // Create a QSettings object for the trainer's Config.ini file
        QSettings settings(trainerFullPath + "/Config.ini", QSettings::IniFormat);

        // Check if the required configuration keys exist
        if (!settings.contains("General/appName")) {
            QMessageBox::information(nullptr, "Test", "The key 'General/appName' does not exist in " + trainerFolder + ".");
            continue;  // Skip this trainer if the appName doesn't exist
        }

        // Retrieve application information from the config file
        QString appName = settings.value("General/appName").toString();
        QString version = settings.value("General/version").toString();
        QString defaultLanguage = settings.value("General/defaultLanguage").toString();

        // Create an item for the trainer
        QListWidgetItem *trainerItem = new QListWidgetItem(trainerFolder);
        trainerItem->setText("Trainer: " + appName + "\nVersion: " + version);
        trainerItem->setSizeHint(QSize(200, 100));
        ui->libraryListWidget->addItem(trainerItem);

        // Optionally, load mods here in the future
        // loadModsForTrainer(trainerFullPath);
    }
}

// Future function for loading mods (to be implemented later)
void Hub::loadModsForTrainer(const QString &trainerFullPath) {
    // Example of how you might structure this in the future
    QDir modsDir(trainerFullPath + "/Mods");
    QStringList modFolders = modsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Loop through each mod folder and process
    for (const QString &modFolder : modFolders) {
        QString modFullPath = trainerFullPath + "/Mods/" + modFolder;

        // Example: Check for a config file in each mod
        QString configFilePath = modFullPath + "/Config.cfg";
        bool hasConfig = QFile::exists(configFilePath);

        // Create a list item for each mod
        QListWidgetItem *modItem = new QListWidgetItem(modFolder);
        QString status = hasConfig ? "Config: Found" : "Config: Missing";
        modItem->setText("Mod: " + modFolder + "\n" + status);
        modItem->setSizeHint(QSize(200, 100));

        // Add the item to the list widget for mods (if you're adding a separate widget for mods)
        // ui->modListWidget->addItem(modItem);
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


void Hub::on_settingsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


void Hub::on_libraryLaunch_clicked()
{

}

