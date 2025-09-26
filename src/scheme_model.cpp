#include "scheme_model.h"

#include <QFont>
#include <QString>
#include <QVariant>

SchemeModel::SchemeModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    for (auto& row : m_) {
        row.fill(0.0);
    }
}

QVariant SchemeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const int r = index.row();
    const int c = index.column();

    switch (role) {
    case Qt::DisplayRole:
        return QString::number(m_[r][c], 'f', 4); // 4 знака после запятой
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::FontRole: {
        QFont f;
        f.setFamilies({"Consolas", "Cascadia Mono", "DejaVu Sans Mono", "Monospace"});
        return f;
    }
    default:
        return {};
    }
}

QVariant SchemeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if (role == Qt::DisplayRole) {
        return QStringLiteral("s%1").arg(section + 1);
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    return {};
}

void SchemeModel::setMatrix(const Matrix& m)
{
    beginResetModel();
    m_ = m;
    endResetModel();
}

