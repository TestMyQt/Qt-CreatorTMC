#ifndef TMCOUTPUTPANE_H
#define TMCOUTPUTPANE_H

#include "tmctestresult.h"
#include "tmcresultmodel.h"

#include <coreplugin/ioutputpane.h>

#include <utils/itemviews.h>
#include <utils/treemodel.h>

QT_BEGIN_NAMESPACE
class QAction;
class QFrame;
class QKeyEvent;
class QLabel;
class QModelIndex;
class QMenu;
class QPlainTextEdit;
class QStackedWidget;
class QToolButton;
QT_END_NAMESPACE

namespace Core {
class IContext;
}

class TmcOutputPane : public Core::IOutputPane
{
    Q_OBJECT
public:
    virtual ~TmcOutputPane();
    static TmcOutputPane *instance();

    // IOutputPane interface
    QWidget *outputWidget(QWidget *parent) override;
    QList<QWidget *> toolBarWidgets() const override;
    QString displayName() const override;
    int priorityInStatusBar() const override;
    void clearContents() override;
    void visibilityChanged(bool visible) override;
    void setFocus() override;
    bool hasFocus() const override;
    bool canFocus() const override;
    bool canNavigate() const override;
    bool canNext() const override;
    bool canPrevious() const override;
    void goToNext() override;
    void goToPrev() override;

    void addTestResults(const QList<TmcTestResult> &results);

private:
    explicit TmcOutputPane(QObject *parent = 0);

    void onRunAllTriggered();
    void onRunSelectedTriggered();
    void enableAllFilter();
    void filterMenuTriggered(QAction *action);

    void initializeFilterMenu();
    void updateSummaryLabel();
    void createToolButtons();
    void onTestRunStarted();
    void onTestRunFinished();
    void onScrollBarRangeChanged(int, int max);
    void updateRunActions();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onCopyItemTriggered(const QModelIndex &idx);
    void onCopyWholeTriggered();
    void onSaveWholeTriggered();
    void toggleOutputStyle();

    QStackedWidget *m_outputWidget;
    QFrame *m_summaryWidget;
    QLabel *m_summaryLabel;
    Utils::ListView *m_listView;
    TmcResultModel *m_model;
    Core::IContext *m_context;
    QToolButton *m_runAll;
    bool m_wasVisibleBefore = false;
};

#endif // TMCOUTPUTPANE_H
