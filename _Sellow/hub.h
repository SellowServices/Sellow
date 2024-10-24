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
    std::string windowTitle = "None";

    explicit Hub(QWidget *parent = nullptr);  // Use 'explicit' to avoid unintentional implicit conversions
    ~Hub();

private slots:
    void on_injectButton_clicked();           // Slot for handling DLL injection
    void on_fileSelectButton_clicked();       // Slot for file selection
    void onAppButtonClicked(const std::string &appTitle); // Slot for handling app button clicks

private:
    void loadActiveApplications();            // Load active applications and update UI
    QPushButton* createAppButton(const std::string &title);
    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam); // Enumerate windows (static)

    Ui::Hub *ui;                              // UI pointer for managing widgets
};

#endif // HUB_H
