#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui
{
class MainWindow;
}

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
         m_IsAppFolderValid;
    int m_OFVersion;
    QStringList m_SelectedAddons;

private slots:
    void getAddonNames();
    void checkAppNameValidity(QString str);
    void checkAppFolderValidity(QString str);
    void browseOFPath();
    void browseAppPath();
    void getSelectedAddons(QListWidgetItem *selectedItem);
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
};

#endif // MAINWINDOW_H
