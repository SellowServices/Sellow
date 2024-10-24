#include "hub.h"
#include "./ui_hub.h"

#include <QFileDialog>
#include <QPushButton>
#include <QString>
#include <QMessageBox>
#include <QVBoxLayout>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>

Hub::Hub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Hub)
{
    ui->setupUi(this);
    loadActiveApplications(); // Load active applications on startup
}

Hub::~Hub()
{
    delete ui;
}

void Hub::loadActiveApplications() {
    // Clear existing buttons in the layout
    QLayoutItem* item;
    while ((item = ui->verticalLayout->takeAt(0))) {
        delete item->widget();
        delete item;
    }

    // Set layout spacing and margins
    ui->verticalLayout->setContentsMargins(0, 3, 3, 3);  // (left, top, right, bottom)
    ui->verticalLayout->setSpacing(20);

    // Vector to hold window titles
    std::vector<std::string> windowTitles;

    // Enumerate windows
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windowTitles));

    // Add spacing of 5 pixels
    ui->verticalLayout->setSpacing(50); // Set spacing between buttons

    // Create buttons for each window
    for (const auto& title : windowTitles) {
        if (!title.empty()) {
            QPushButton *button = createAppButton(title);
            ui->verticalLayout->addWidget(button);
            ui->verticalLayout->addSpacing(20);
        }
    }
    ui->verticalLayout->update();
}

QPushButton* Hub::createAppButton(const std::string &title) {
    QPushButton *button = new QPushButton(QString::fromStdString(title), this);

    // Set button style for Discord-like theme
    QString buttonColor = "#40444B"; // Dark gray color
    QString textColor = "#FFFFFF"; // White text for contrast

    button->setStyleSheet(QString("QPushButton { background-color: %1; color: %2; height: 15px;}"
                                  "QPushButton:hover { background-color: #5865F2;}")
                              .arg(buttonColor, textColor));

    // Set the button to fill the available width
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setMinimumHeight(15); // Set a more prominent minimum height

    // Connect the button click event
    connect(button, &QPushButton::clicked, [this, title](){ onAppButtonClicked(title); });

    return button;
}

BOOL CALLBACK Hub::EnumWindowsProc(HWND hWnd, LPARAM lParam) // Define as static member
{
    // Get the vector of window titles from LPARAM
    std::vector<std::string>* windowTitles = reinterpret_cast<std::vector<std::string>*>(lParam);
    int length = GetWindowTextLengthA(hWnd);

    if (length > 0) {
        std::string windowTitle(length + 1, '\0');
        GetWindowTextA(hWnd, &windowTitle[0], length + 1);
        windowTitles->push_back(windowTitle); // Add title to the vector
    }

    return TRUE; // Continue enumeration
}

void Hub::onAppButtonClicked(const std::string &appTitle) {
    // Handle button click for the corresponding application
    QMessageBox::information(this, tr("Selected Application"), tr("You selected: %1").arg(QString::fromStdString(appTitle)));

    windowTitle = appTitle;
}

// String

std::wstring stringToWString(const std::string &str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

const wchar_t* stringToWCharPtr(const std::string &str) {
    static std::wstring wstr = stringToWString(str); // Make sure wstr stays in scope
    return wstr.c_str();
}

// String End

void Hub::on_injectButton_clicked()
{
    QString dllPath = ui->fileLineEdit->text();
    if (dllPath.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a DLL file first!"));
        return;
    }

    dllPath = QDir::toNativeSeparators(dllPath);

    // This can be modified to work with the selected button's title if necessary
    std::wstring dllName = dllPath.toStdWString();
    const wchar_t* thisWindowTitle = stringToWCharPtr(windowTitle);

    InjectorTool injector(dllName.c_str(), thisWindowTitle);
    if (!injector.inject()) {
        QMessageBox::critical(this, tr("Error"), tr("DLL injection failed! Please check the DLL and the target application."));
    } else {
        QMessageBox::information(this, tr("Success"), tr("DLL injected successfully!"));
    }
}

void Hub::on_fileSelectButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select DLL File"), "", tr("DLL Files (*.dll);;All Files (*)"));
    if (!fileName.isEmpty()) {
        ui->fileLineEdit->setText(fileName);
    }
}
