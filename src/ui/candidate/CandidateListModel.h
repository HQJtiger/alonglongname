#pragma once

#include <QAbstractListModel>
#include <QVector>

#include "core/common/Types.h"

namespace uim {

/**
 * @brief 候选词列表模型
 *
 * 用于在候选词窗口中显示候选词列表。
 */
class CandidateListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum CandidateRoles {
        TextRole = Qt::UserRole + 1,   ///< 候选词文本
        PinyinRole,                     ///< 拼音
        IndexRole,                      ///< 序号
        FrequencyRole,                  ///< 词频
        IsFirstRole                     ///< 是否第一个候选
    };

    explicit CandidateListModel(QObject* parent = nullptr);
    ~CandidateListModel() override;

    /**
     * @brief 设置候选词列表
     */
    void setCandidates(const CandidateList& candidates);

    /**
     * @brief 获取候选词数量
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief 获取数据
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief 获取角色名称
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief 获取指定索引的候选词
     */
    Candidate candidateAt(int index) const;

    /**
     * @brief 清空候选词
     */
    void clear();

private:
    CandidateList m_candidates;
};

} // namespace uim
