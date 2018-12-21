#include "exercisedelegate.h"
#include "exercise.h"

#include <QDebug>
#include <QLabel>
#include <QStyle>
#include <QApplication>

ExerciseDelegate::ExerciseDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{

}

void ExerciseDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int progress = index.data().toInt();
    QStyleOptionButton opt;

    switch (progress) {
    case 0:
        opt.state |= QStyle::State_Active;
        opt.text =  "Download";
        opt.rect = option.rect;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &opt, painter);
        break;
    case -1:
        opt.state |= QStyle::State_Active;
        opt.text =  "Open";
        opt.rect = option.rect;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &opt, painter);
        break;
    default:
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = option.rect;
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = progress;
        progressBarOption.text = QString::number(progress) + "%";
        progressBarOption.textVisible = true;

        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                           &progressBarOption, painter);
        break;
    }
}
