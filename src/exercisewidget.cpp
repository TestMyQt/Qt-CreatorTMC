#include "exercisewidget.h"
#include "exercisedelegate.h"

#include <QDebug>

ExerciseWidget::ExerciseWidget(QWidget *parent) : QWidget(parent)
{
    m_exerciseWindow = new Ui::ExerciseWindow;
    m_exerciseWindow->setupUi(this);

    m_tableView = m_exerciseWindow->exerciseList;

    m_model = new ExerciseModel;
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setItemDelegate(new ExerciseDelegate);

    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged, [](const QItemSelection &selected, const QItemSelection &deselected) {
        qDebug() << selected;
    });

    m_selectAll = m_exerciseWindow->selectAll;
    m_selectAll->setCheckState(Qt::Checked);
    connect(m_selectAll, &QCheckBox::stateChanged, m_model, &ExerciseModel::onSelectAll);

    connect(m_exerciseWindow->okButton, &QPushButton::clicked, this, &ExerciseWidget::selectExercises);
    connect(m_exerciseWindow->cancelButton, &QPushButton::clicked, this, &QWidget::close);
}

void ExerciseWidget::addExercises(const QList<Exercise> exercises)
{
    m_model->addExercises(exercises);
}

void ExerciseWidget::selectExercises()
{
    m_model->triggerDownload();
    //emit onExercisesSelected(m_model->selectedExercises());
}

