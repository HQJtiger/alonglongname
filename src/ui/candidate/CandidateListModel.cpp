#include "CandidateListModel.h"

namespace uim {

CandidateListModel::CandidateListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

CandidateListModel::~CandidateListModel() = default;

void CandidateListModel::setCandidates(const CandidateList& candidates)
{
    beginResetModel();
    m_candidates = candidates;
    endResetModel();
}

int CandidateListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_candidates.size();
}

QVariant CandidateListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_candidates.size()) {
        return QVariant();
    }

    const Candidate& candidate = m_candidates[index.row()];

    switch (role) {
    case Qt::DisplayRole:
    case TextRole:
        return candidate.text;
    case PinyinRole:
        return candidate.pinyin;
    case IndexRole:
        return candidate.index;
    case FrequencyRole:
        return candidate.frequency;
    case IsFirstRole:
        return index.row() == 0;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CandidateListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[PinyinRole] = "pinyin";
    roles[IndexRole] = "index";
    roles[FrequencyRole] = "frequency";
    roles[IsFirstRole] = "isFirst";
    return roles;
}

Candidate CandidateListModel::candidateAt(int index) const
{
    if (index < 0 || index >= m_candidates.size()) {
        return Candidate();
    }
    return m_candidates[index];
}

void CandidateListModel::clear()
{
    beginResetModel();
    m_candidates.clear();
    endResetModel();
}

} // namespace uim
