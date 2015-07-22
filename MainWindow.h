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
    QString m_OFPath, m_OFAddonsPath, m_OFAppTemplatePath, m_AppPath, m_AddonsPath;
    bool m_IsAppNameValid;
    QStringList m_SelectedAddons;

private slots:
    void getAddonNames();
    void checkAppNameValidity(QString str);
    void browseOFPath();
    void browseAppPath();
    void getSelectedAddons(QListWidgetItem *selectedItem);
    void generateProject();

private:
    void insertAddons(QString &priContent);
    bool copyAddon(const QString &addonName);

    bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);
};

#endif // MAINWINDOW_H
