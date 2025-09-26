#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QTableView;
class QLabel;
class SchemeModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void openFile();

private:
    void loadFile(const QString& path);

    Ui::MainWindow* ui = nullptr;

    SchemeModel* m_model = nullptr;

    // Короткие алиасы на виджеты из .ui
    QTableView* m_table = nullptr;
    QLabel*     m_lblN  = nullptr;
    QLabel*     m_lblH  = nullptr;
    QLabel*     m_lblHmax = nullptr;
    QLabel*     m_lblHref = nullptr;
};
