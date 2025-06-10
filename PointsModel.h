#ifndef POINTSMODEL_H
#define POINTSMODEL_H

#include <QAbstractListModel>
#include <QPointF>
#include <QList>

class PointsModel : public QAbstractListModel
{
    Q_OBJECT
public:

    explicit PointsModel(QObject *parent = nullptr);  // Constructor

    enum RoleNames {
        XRole = Qt::UserRole + 1,  // x verisini tutacak rol
        YRole                       // y verisini tutacak rol
    };

    void addPoint(const QPointF &point);  // Yeni bir nokta eklemek için fonksiyon

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;  // Satır sayısını döndüren fonksiyon
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;  // Verileri döndüren fonksiyon

    Q_INVOKABLE void clear();  // QML tarafında çağrılabilir olan fonksiyon

private:
    QList<QPointF> m_points;  // Verileri tutacak liste

    QHash<int, QByteArray> roleNames() const override;  // roleNames fonksiyonunu ekledik
};

#endif // POINTSMODEL_H
