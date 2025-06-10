#include "PointsModel.h"

PointsModel::PointsModel(QObject *parent) : QAbstractListModel(parent)
{
}

void PointsModel::addPoint(const QPointF &point)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_points.append(point);
    endInsertRows();
}

int PointsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_points.size();
}

QVariant PointsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const QPointF &point = m_points.at(index.row());

    if (role == XRole)
        return point.x();  // x verisini döndür
    else if (role == YRole)
        return point.y();  // y verisini döndür

    return QVariant();
}

QHash<int, QByteArray> PointsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[XRole] = "x";  // x verisini tutacak rol
    roles[YRole] = "y";  // y verisini tutacak rol
    return roles;
}

void PointsModel::clear()
{
    beginResetModel();
    m_points.clear();
    endResetModel();
}
