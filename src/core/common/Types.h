#pragma once

#include <QString>
#include <QVector>
#include <QStringList>

namespace uim {

/**
 * @brief 候选词结构
 */
struct Candidate {
    QString text;       ///< 候选词文本
    QString pinyin;     ///< 对应的拼音
    int frequency;      ///< 词频
    int index;          ///< 候选序号（1-9）

    Candidate() : frequency(0), index(0) {}
    Candidate(const QString& t, const QString& p, int freq = 0)
        : text(t), pinyin(p), frequency(freq), index(0) {}
};

/**
 * @brief 候选词列表
 */
using CandidateList = QVector<Candidate>;

/**
 * @brief 输入模式
 */
enum class InputMode {
    Pinyin,     ///< 拼音输入
    English,    ///< 英文输入
    Voice       ///< 语音输入
};

/**
 * @brief 主题类型
 */
enum class ThemeType {
    Light,      ///< 浅色主题
    Dark,       ///< 深色主题
    System      ///< 跟随系统
};

/**
 * @brief 剪贴板内容类型
 */
enum class ClipboardContentType {
    Text,       ///< 文本
    Image,      ///< 图片
    File,       ///< 文件
    Html,       ///< HTML
    Unknown     ///< 未知类型
};

/**
 * @brief 剪贴板条目
 */
struct ClipboardItem {
    QString id;                  ///< 唯一标识
    ClipboardContentType type;   ///< 内容类型
    QString content;             ///< 文本内容（文本类型时）
    QByteArray binaryData;       ///< 二进制数据（图片/文件等）
    qint64 timestamp;            ///< 时间戳
    QString deviceId;            ///< 来源设备ID
    QString preview;             ///< 预览文本
    bool isFavorite;             ///< 是否收藏

    ClipboardItem()
        : type(ClipboardContentType::Unknown)
        , timestamp(0)
        , isFavorite(false) {}
};

/**
 * @brief 同步状态
 */
enum class SyncStatus {
    Disconnected,   ///< 未连接
    Connecting,     ///< 连接中
    Connected,      ///< 已连接
    Error           ///< 错误
};

/**
 * @brief 设备信息
 */
struct DeviceInfo {
    QString deviceId;      ///< 设备ID
    QString deviceName;    ///< 设备名称
    QString platform;      ///< 平台（Windows/macOS/Linux/Android/iOS）
    QString ipAddress;     ///< IP地址
    int port;              ///< 端口号
    bool isOnline;         ///< 是否在线

    DeviceInfo() : port(0), isOnline(false) {}
};

} // namespace uim
