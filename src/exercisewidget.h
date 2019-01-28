#ifndef EXERCISEWIDGET_H
#define EXERCISEWIDGET_H

#include "exercise.h"
#include "exercisemodel.h"

#include <QWidget>
#include <QList>

#include <ui_exercisewidget.h>

class ExerciseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExerciseWidget(QWidget *parent = nullptr);
    void setModel(ExerciseModel *model);

Q_SIGNALS:
    void onExercisesSelected(QList<Exercise> selected);

private:
    Ui::ExerciseWindow *m_exerciseWindow;
    ExerciseModel *m_model;
    QTableView *m_tableView;
    QCheckBox *m_selectAll;

};

#endif // EXERCISEWIDGET_H
