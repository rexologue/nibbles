#pragma once
#include <QAbstractTableModel>
#include <array>


class SchemeModel : public QAbstractTableModel {
Q_OBJECT
public:
using Matrix = std::array<std::array<double, 16>, 16>;


explicit SchemeModel(QObject* parent = nullptr);


int rowCount(const QModelIndex& parent = QModelIndex()) const override { Q_UNUSED(parent); return 16; }
int columnCount(const QModelIndex& parent = QModelIndex()) const override { Q_UNUSED(parent); return 16; }


QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
QVariant headerData(int section, Qt::Orientation orientation, int role) const override;


void setMatrix(const Matrix& m);


private:
Matrix m_{}; // инициализация нулями
};