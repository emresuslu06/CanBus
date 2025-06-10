#include "fileRead.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMap>
#include <QList>

fileRead::fileRead() {}

QList<QString> fileRead::readFile()
{
    QList<QString> lines;
    QString fileName = "C:/Users/memre/Desktop/Id_Oku.txt";

    qDebug() << "Dosya okunuyor: " << fileName;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Dosya açılamadı" << fileName;
        return lines;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            lines.append(line);
            qDebug() << "Okunan satır: " << line;
        }
    }

    file.close();
    return lines;
}


QMap<QString, QPair<QList<int>, QList<QPair<int, int>>>> fileRead::parseLines(const QList<QString>& lines)
{
    QMap<QString, QPair<QList<int>, QList<QPair<int, int>>>> idToData;

    for (const QString &line : lines) {
        QStringList parts = line.split(" > ");

        QString id = parts[0].trimmed();  // ID kısmını al

        QStringList elements = parts[1].split(" . ");  // Byte ve bit bilgilerini ayır

        QList<int> byteList;
        QList<QPair<int, int>> bitList;

        for (const QString &elem : elements) {
            QString trimmed = elem.trimmed();

            if (trimmed.startsWith("[") && trimmed.endsWith("]")) {
                // Bit aralığı: [start, length] şeklinde olan kısımları al
                QString inner = trimmed.mid(1, trimmed.length() - 2); // [6,4] -> "6,4"
                QStringList bitParts = inner.split(",");
                if (bitParts.size() == 2) {
                    bool ok1, ok2;
                    int start = bitParts[0].toInt(&ok1);
                    int len = bitParts[1].toInt(&ok2);
                    if (ok1 && ok2) {
                        bitList.append(qMakePair(start, len));
                    }
                }
            } else {
                // Byte verisini al
                bool ok;
                int byteVal = trimmed.toInt(&ok);
                if (ok) {
                    byteList.append(byteVal);
                }
            }
        }

        // ID'yi ve byte'ları, bit aralıklarını ilişkilendirerek map'e ekle
        idToData.insert(id, qMakePair(byteList, bitList));
    }

    qDebug() << "parseLines çıktı içeriği:";
    for (const auto &key : idToData.keys()) {
        qDebug() << key << " -> Bytes:" << idToData[key].first << "Bits:" << idToData[key].second;
    }

    return idToData;
}

