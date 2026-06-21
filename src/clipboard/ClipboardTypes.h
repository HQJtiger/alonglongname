#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QVector>

namespace uim {

/**
 * @brief 剪贴板内容类型
 */
enum class ClipboardContentType {
    Text,       ///< 文本
    Image,      ///< 图片
    File,       ///< 文件
    Html,       ///< HTML
    Unknown     ///< 未知
};

/**
 * @brief 剪贴板项
 */
struct ClipboardItem {
    QString id;                     ///< 唯一 ID
    ClipboardContentType type;      ///< 内容类型
    QString content;                ///< 文本内容
    QByteArray binaryData;          ///< 二进制数据（图片等）
    QDateTime timestamp;            ///< 时间戳
    QString deviceId;               ///< 来源设备 ID
    QString preview;                ///< 预览文本
    bool isFavorite;                ///< 是否收藏

    ClipboardItem()
        : type(ClipboardContentType::Text)
        , isFavorite(false)
    {}
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
    QString deviceId;       ///< 设备 ID
    QString deviceName;     ///< 设备名称
    QString platform;       ///< 平台（Windows/macOS/Linux）
    QString ipAddress;      ///< IP 地址
    int port;               ///< 端口
    bool isOnline;          ///< 是否在线

    DeviceInfo()
        : port(0)
        , isOnline(false)
    {}
};

} // namespace uim
