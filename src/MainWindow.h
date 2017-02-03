#pragma once
#include <QMainWindow>
#include <QJsonArray>

namespace Ui
{
class MainWindow;
}
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString m_OFPath,
            m_OFAddonsPath,
            m_OFAppTemplatePath,
            m_AppPath,
            // addons folder in the app directory
            m_AddonsPath,
            m_PriFile;

    bool m_IsAppNameValid,
         m_IsOFPathValid,
         m_IsAppFolderValid,
         m_IsUpdateProject;

    int m_OFVersion;
    QStringList m_SelectedAddons;
    QJsonArray m_RecentProjectArray;

private slots:
    void changeOfVersion(int currentIndex);

private:
    /**
     * @brief Lists the addon names from the selected openFrameworks path
     */
    void listAddonNames();
    void checkAppNameValidity(const QString &str);
    void checkAppFolderValidity(const QString &str);

    void browseOFPath();
    void browseAppPath();
    /**
     * @brief If selectedItem equals to nullptr then m_SelectedAddons is used to determine which item in list widget should be checked.
     * If selectedItem is not nullptr, then the item is used to update m_SelectedAddons
     * @param selectedItem
     */
    void updateSelectedAddons(QListWidgetItem *selectedItem = nullptr);

    /**
     * @brief Uses the selected options to determine the type of project to generate
     */
    void generateProject();
    void generateQMakeProject();
    void generateCMakeProject();
    void generateVSProject();

    QString getErrorString() const;
    void insertAddonsQMake(QString &priContent);
    void insertAddonsCMake();
    void insertAddonsVS();

    bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath) const;
    void copyOFTemplateFiles();
    void saveProjectToRecents();

    void fillRecentsMenu();
    QJsonObject getRecentProject(const QString &appPath);
    void recentProjectSelected(QAction *selected);
};
