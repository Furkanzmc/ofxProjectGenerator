#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    QList<QListWidgetItem *> m_AddonItems;
    QString m_OFPath,
            m_OFAddonsPath,
            m_OFAppTemplatePath,
            m_AppPath,
            m_AddonsPath/*addons folder in the app directory*/,
            m_PriFile;

    bool m_IsAppNameValid,
         m_IsOFPathValid,
         m_IsAppFolderValid,
         m_IsUpdateProject;

    int m_OFVersion;
    QStringList m_SelectedAddons;
    QJsonArray m_RecentProjectArray;

private slots:
    void getAddonNames();
    void checkAppNameValidity(const QString &str);
    void checkAppFolderValidity(QString str);

    void browseOFPath();
    void browseAppPath();
    void updateSelectedAddons(QListWidgetItem *selectedItem = nullptr);

    void generateProject();
    void generateQMakeProject();
    void generateCMakeProject();

    void changeOfVersion(int currentIndex);

private:
    QString getErrorString() const;
    void insertAddonsQMake(QString &priContent);
    void insertAddonsCMake();

    bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);
    void copyOFTemplateFiles();
    void saveProjectToRecents();

    void fillRecentsMenu();
    QJsonObject getRecentProject(const QString &appPath);
    void recentProjectSelected(QAction *selected);
};

#endif // MAINWINDOW_H
