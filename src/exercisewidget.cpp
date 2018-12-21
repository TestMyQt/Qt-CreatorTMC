#include "exercisewidget.h"
#include "exercisedelegate.h"

ExerciseWidget::ExerciseWidget(QWidget *parent) : QWidget(parent)
{
    m_exerciseWindow = new Ui::ExerciseWindow;
    m_exerciseWindow->setupUi(this);

    m_tableView = m_exerciseWindow->exerciseList;

    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setMinimumSectionSize(60);
    m_tableView->setWordWrap(false);
    m_tableView->setItemDelegateForColumn(1, new ExerciseDelegate(this));

    m_selectAll = m_exerciseWindow->selectAll;

    connect(m_exerciseWindow->cancelButton, &QPushButton::clicked, this, &QWidget::close);
}

void ExerciseWidget::setModel(ExerciseModel *model)
{
    m_tableView->setModel(model);
    connect(m_tableView, &QTableView::clicked, model, &ExerciseModel::onTableClicked);
    connect(m_selectAll, &QCheckBox::stateChanged, model, &ExerciseModel::onSelectAll);
    connect(m_exerciseWindow->downloadButton, &QPushButton::clicked, model, &ExerciseModel::triggerDownload);
}

