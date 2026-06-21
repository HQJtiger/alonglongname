#pragma once

#include <QWidget>
#include <QListView>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "CandidateListModel.h"
#include "core/pinyin/PinyinEngine.h"

namespace uim {

/**
 * @brief 候选词窗口
 *
 * 输入法候选词窗口，显示当前输入的拼音和候选词列表。
 * 窗口为无边框、置顶、半透明效果。
 */
class CandidateWindow : public QWidget {
    Q_OBJECT

public:
    explicit CandidateWindow(QWidget* parent = nullptr);
    ~CandidateWindow() override;

    /**
     * @brief 设置拼音引擎
     */
    void setPinyinEngine(PinyinEngine* engine);

    /**
     * @brief 更新候选词显示
     */
    void updateCandidates();

    /**
     * @brief 更新拼音显示
     */
    void updatePinyin();

    /**
     * @brief 在指定位置显示窗口
     * @param x X 坐标
     * @param y Y 坐标
     */
    void showAt(int x, int y);

    /**
     * @brief 隐藏窗口
     */
    void hideWindow();

    /**
     * @brief 设置主题
     * @param theme 主题类型
     */
    void setTheme(ThemeType theme);

    /**
     * @brief 设置字体大小
     */
    void setFontSize(int size);

    /**
     * @brief 设置候选词数量
     */
    void setCandidateCount(int count);

signals:
    /**
     * @brief 候选词被选中信号
     * @param index 选中的索引
     */
    void candidateSelected(int index);

protected:
    /**
     * @brief 鼠标点击事件
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief 鼠标移动事件（用于拖动窗口）
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief 鼠标释放事件
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    /**
     * @brief 列表项被点击
     */
    void onItemClicked(const QModelIndex& index);

    /**
     * @brief 候选词变化
     */
    void onCandidatesChanged();

    /**
     * @brief 预编辑文本变化
     */
    void onCompositionChanged();

private:
    /**
     * @brief 初始化 UI
     */
    void setupUi();

    /**
     * @brief 应用样式
     */
    void applyStyle();

    /**
     * @brief 更新页码显示
     */
    void updatePageInfo();

    QLabel* m_pinyinLabel;           ///< 拼音显示标签
    QListView* m_candidateListView;  ///< 候选词列表视图
    QLabel* m_pageLabel;             ///< 页码标签
    CandidateListModel* m_listModel; ///< 候选词列表模型

    PinyinEngine* m_pinyinEngine;    ///< 拼音引擎

    ThemeType m_currentTheme;        ///< 当前主题
    int m_fontSize;                  ///< 字体大小
    int m_candidateCount;            ///< 候选词数量

    bool m_isDragging;               ///< 是否正在拖动
    QPoint m_dragPosition;           ///< 拖动起始位置
};

} // namespace uim
