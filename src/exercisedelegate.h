#ifndef EXERCISEDELEGATE_H
#define EXERCISEDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class ExerciseDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ExerciseDelegate(QWidget *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

signals:

public slots:
};

#endif // EXERCISEDELEGATE_H
