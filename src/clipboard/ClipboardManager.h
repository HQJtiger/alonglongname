#pragma once

#include <QObject>
#include <QVector>
#include "ClipboardTypes.h"

namespace uim {

/**
 * @brief 剪贴板管理器
 *
 * 负责监听系统剪贴板变化，管理剪贴板历史记录。
 * MVP 阶段：基础框架，纯内存存储
 */
class ClipboardManager : public QObject {
    Q_OBJECT

public:
    explicit ClipboardManager(QObject* parent = nullptr);
    ~ClipboardManager() override;

    /**
     * @brief 初始化剪贴板管理器
     */
    bool initialize();

    /**
     * @brief 关闭剪贴板管理器
     */
    void shutdown();

    /**
     * @brief 获取剪贴板历史记录
     */
    QVector<ClipboardItem> history() const;

    /**
     * @brief 获取历史记录数量
     */
    int historyCount() const;

    /**
     * @brief 清空历史记录
     */
    void clearHistory();

    /**
     * @brief 删除指定历史记录
     * @param id 记录 ID
     */
    void removeItem(const QString& id);

    /**
     * @brief 收藏/取消收藏
     * @param id 记录 ID
     * @param favorite 是否收藏
     */
    void setFavorite(const QString& id, bool favorite);

    /**
     * @brief 复制内容到剪贴板
     * @param item 剪贴板项
     */
    void copyToClipboard(const ClipboardItem& item);

    /**
     * @brief 搜索历史记录
     * @param keyword 关键词
     * @return 匹配的记录列表
     */
    QVector<ClipboardItem> search(const QString& keyword) const;

    /**
     * @brief 加载历史记录
     */
    bool loadHistory();

    /**
     * @brief 保存历史记录
     */
    bool saveHistory();

signals:
    /**
     * @brief 剪贴板内容变化
     * @param item 新的剪贴板项
     */
    void clipboardChanged(const ClipboardItem& item);

    /**
     * @brief 历史记录变化
     */
    void historyChanged();

private slots:
    /**
     * @brief 系统剪贴板变化
     */
    void onClipboardDataChanged();

private:
    /**
     * @brief 从系统剪贴板读取内容
     */
    ClipboardItem readFromClipboard() const;

    /**
     * @brief 生成唯一 ID
     */
    QString generateId() const;

    /**
     * @brief 限制历史记录数量
     */
    void limitHistorySize();

    QVector<ClipboardItem> m_history;   ///< 历史记录
    int m_maxHistorySize;               ///< 最大历史记录数
    bool m_enabled;                     ///< 是否启用
    bool m_ignoreNextChange;            ///< 忽略下一次变化（自己设置的）
};

} // namespace uim
