#include "mainwindow.h"
#include "ui_mainwindow.h"          // генерируется из mainwindow.ui

#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QStatusBar>
#include <QTableView>

#include <utility>
#include <vector>

#include "scheme_model.h"

// Ваши существующие заголовки
#include "file_to_nibbles.h"
#include "nibble.h"
#include "scheme.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("Nibble Entropy GUI"));

    // Модель и таблица
    m_model = new SchemeModel(this);
    m_table = ui->tableView;
    m_table->setModel(m_model);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Метрики (берём по objectName из .ui)
    m_lblN    = ui->findChild<QLabel*>("lblN");
    m_lblH    = ui->findChild<QLabel*>("lblH");
    m_lblHmax = ui->findChild<QLabel*>("lblHmax");
    m_lblHref = ui->findChild<QLabel*>("lblHref");

    // Начальные значения
    if (m_lblN)    m_lblN->setText("–");
    if (m_lblH)    m_lblH->setText("–");
    if (m_lblHmax) m_lblHmax->setText("–");
    if (m_lblHref) m_lblHref->setText("–");

    statusBar()->showMessage(tr("Выберите файл с данными нибблов…"));

    // Сигналы действий меню/тулбара
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Выбор файла"), QString(), tr("Все файлы (*.*)")
    );
    if (!path.isEmpty()) {
        loadFile(path);
    }
}

void MainWindow::loadFile(const QString& path)
{
    try {
        // 1) читаем файл → нибблы
        std::vector<Nibble> nibbleSequence = nibble_io::file_to_nibbles(path.toStdString());

        // 2) считаем схему переходов
        Scheme sch(std::move(nibbleSequence));
        const auto& T = sch.table(); // std::array<std::array<double,16>,16>

        // 3) обновляем модель таблицы
        SchemeModel::Matrix M = T;
        m_model->setMatrix(M);

        // 4) метрики
        const double H    = sch.entropy_joint();
        const double Hmax = sch.entropy_max();
        const double Hrel = (Hmax > 0.0) ? (H / Hmax) : 0.0;

        if (m_lblN)    m_lblN->setText(QString::number(static_cast<qulonglong>(sch.transitions())));
        if (m_lblH)    m_lblH->setText(QString::number(H, 'f', 4));
        if (m_lblHmax) m_lblHmax->setText(QString::number(Hmax, 'f', 4));
        if (m_lblHref) m_lblHref->setText(QString::number(Hrel, 'f', 4));

        statusBar()->showMessage(tr("Загружено: %1").arg(QFileInfo(path).fileName()), 4000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не удалось обработать файл:\n%1")
                              .arg(QString::fromLocal8Bit(e.what())));
    } catch (...) {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Неизвестная ошибка при обработке файла."));
    }
}
