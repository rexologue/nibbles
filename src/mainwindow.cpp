#include "mainwindow.h"
#include "ui_mainwindow.h"          

#include <QAction>
#include <QDir>
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

// core
#include "nibble.h"
#include "scheme.h"
#include "nibbles_io.h"
#include "nibble_intervals.h"

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

    // Метрики (указатели приходят напрямую из ui)
    m_lblN    = ui->lblN;
    m_lblH    = ui->lblH;
    m_lblHmax = ui->lblHmax;
    m_lblHref = ui->lblHref;

    // Начальные значения
    if (m_lblN)    m_lblN->setText("–");
    if (m_lblH)    m_lblH->setText("–");
    if (m_lblHmax) m_lblHmax->setText("–");
    if (m_lblHref) m_lblHref->setText("–");

    statusBar()->showMessage(tr("Выберите файл с данными нибблов…"));

    // Сигналы действий меню/тулбара
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionPack, &QAction::triggered, this, &MainWindow::packFile);
    connect(ui->actionUnpack, &QAction::triggered, this, &MainWindow::unpackFile);
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

void MainWindow::packFile()
{
    const QString sourcePath = QFileDialog::getOpenFileName(
        this, tr("Выбор файла для упаковки"), QString(), tr("Все файлы (*.*)")
    );
    if (sourcePath.isEmpty()) {
        return;
    }

    const QString saveDir = QFileDialog::getExistingDirectory(
        this, tr("Выбор папки для сохранения"), QString()
    );
    if (saveDir.isEmpty()) {
        return;
    }

    try
    {
        const std::vector<Nibble> nibbles = nibble_io::file_to_nibbles(sourcePath.toStdString());

        NibbleIntervalArchiever archiever;
        const QString archiveName = QFileInfo(sourcePath).completeBaseName() + QStringLiteral(".nibble");
        const QString archivePath = QDir(saveDir).filePath(archiveName);

        archiever.pack(nibbles, archivePath.toStdString());

        statusBar()->showMessage(tr("Файл упакован: %1").arg(QFileInfo(archivePath).fileName()), 4000);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не удалось упаковать файл:\n%1")
                                  .arg(QString::fromLocal8Bit(e.what())));
    }
    catch (...)
    {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Неизвестная ошибка при упаковке файла."));
    }
}

void MainWindow::unpackFile()
{
    const QString archivePath = QFileDialog::getOpenFileName(
        this, tr("Выбор архива .nibble"), QString(), tr("Nibble архивы (*.nibble);;Все файлы (*.*)")
    );
    if (archivePath.isEmpty()) {
        return;
    }

    const QString saveDir = QFileDialog::getExistingDirectory(
        this, tr("Выбор папки для распаковки"), QString()
    );
    if (saveDir.isEmpty()) {
        return;
    }

    try
    {
        NibbleIntervalArchiever archiever;
        const std::vector<Nibble> nibbles = archiever.unpack(archivePath.toStdString());

        const QString unpackedName = QFileInfo(archivePath).completeBaseName();
        const QString targetPath = QDir(saveDir).filePath(unpackedName);

        nibble_io::write_nibbles_to_file(targetPath.toStdString(), nibbles);

        statusBar()->showMessage(tr("Файл распакован: %1").arg(QFileInfo(targetPath).fileName()), 4000);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не удалось распаковать файл:\n%1")
                                  .arg(QString::fromLocal8Bit(e.what())));
    }
    catch (...)
    {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Неизвестная ошибка при распаковке файла."));
    }
}

void MainWindow::loadFile(const QString& path)
{
    try 
    {
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
    } 
    catch (const std::exception& e) 
    {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не удалось обработать файл:\n%1")
                              .arg(QString::fromLocal8Bit(e.what())));
    } 
    catch (...) 
    {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Неизвестная ошибка при обработке файла."));
    }
}
