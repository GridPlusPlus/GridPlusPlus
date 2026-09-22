/** @file GridSQLite.h
 *  @brief 定義素材包使用的唯讀 SQLite 讀取器。
 */
#ifndef GRID_PLUS_PLUS_GRID_SQLITE_H_
#define GRID_PLUS_PLUS_GRID_SQLITE_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace gridpp::internal {

/** SQLite record 解碼後的單一欄位。 */
struct Column {
    /** 欄位類型：0 整數、1 文字或 BLOB、2 NULL、3 浮點數。 */
    int kind = 2;
    std::int64_t integer = 0;
    std::vector<unsigned char> bytes;
};

/**
 * 直接從記憶體中讀取 SQLite 3 table B-tree 的最小化唯讀 reader。
 *
 * 此 reader 只實作 Grid++ 素材包所需的頁面與 record 格式，不是通用 SQL 引擎。
 */
class SqliteReader {
public:
    /** @param data SQLite 檔案內容；其生命週期必須長於 reader。 */
    explicit SqliteReader(const std::vector<unsigned char>& data);
    SqliteReader(std::vector<unsigned char>&&) = delete;

    /**
     * 以 rowid 順序走訪 table B-tree，並將每筆解碼後的 record 傳給 callback。
     * @param root_page table B-tree 的根頁編號；SQLite 頁碼從 1 開始。
     */
    void WalkTable(std::uint32_t root_page,
                   const std::function<void(std::int64_t, const std::vector<Column>&)>& callback) const;

private:
    static void Fail(const std::string& message);
    void RequireRange(std::size_t offset, std::size_t length, std::size_t end, const char* what) const;
    std::size_t PageCount() const;
    std::size_t PageOffset(std::uint32_t page_number) const;
    std::uint16_t ReadBigEndianUint16(std::size_t offset) const;
    std::uint32_t ReadBigEndianUint32(std::size_t offset) const;
    std::uint64_t ReadVarint(std::size_t& offset, std::size_t end) const;
    static std::int64_t UnsignedBitsToInt64(std::uint64_t value);
    static std::int64_t ReadBigEndianInt64(const std::vector<unsigned char>& bytes, std::size_t offset,
                                           std::size_t length);
    static std::uint64_t ReadRecordVarint(const std::vector<unsigned char>& record, std::size_t& offset,
                                          std::size_t end);
    static std::vector<Column> DecodeRecord(const std::vector<unsigned char>& record);
    void MarkPage(std::uint32_t page_number, std::vector<bool>& visited) const;
    void WalkTablePage(std::uint32_t page_number,
                       const std::function<void(std::int64_t, const std::vector<Column>&)>& callback,
                       std::vector<bool>& visited, std::size_t depth) const;

    const std::vector<unsigned char>& data_;
    std::size_t page_size_ = 0;
    std::size_t usable_size_ = 0;
};

// Inline definitions ---------------------------------------------------------

inline SqliteReader::SqliteReader(const std::vector<unsigned char>& data) : data_(data) {
    static constexpr unsigned char kMagic[] = "SQLite format 3";
    if (data_.size() < 100 || std::memcmp(data_.data(), kMagic, 16) != 0) Fail("不是有效的 SQLite 3 檔案");

    page_size_ = ReadBigEndianUint16(16);
    if (page_size_ == 1) page_size_ = 65536;
    if (page_size_ < 512 || page_size_ > 65536 || (page_size_ & (page_size_ - 1)) != 0) Fail("page size 無效");
    if (data_.size() < page_size_ || data_.size() % page_size_ != 0) Fail("檔案大小不是完整頁面");

    const std::size_t reserved = data_[20];
    if (reserved >= page_size_ || page_size_ - reserved < 480) Fail("usable page size 無效");
    usable_size_ = page_size_ - reserved;

    if (data_[21] != 64 || data_[22] != 32 || data_[23] != 32) Fail("payload fraction 無效");
    const std::uint32_t schema_format = ReadBigEndianUint32(44);
    if (schema_format < 1 || schema_format > 4) Fail("schema format 無效");
    if (ReadBigEndianUint32(56) != 1) Fail("只支援 UTF-8 SQLite 資料庫");
    if (data_[18] == 2 || data_[19] == 2) Fail("不支援 WAL 格式的 SQLite 資料庫");
    if (data_[18] != 1 || data_[19] != 1) Fail("SQLite file format version 無效");

    const std::uint32_t declared_pages = ReadBigEndianUint32(28);
    if (declared_pages != 0 && ReadBigEndianUint32(24) == ReadBigEndianUint32(92) && declared_pages != PageCount()) {
        Fail("檔頭頁數與檔案大小不一致");
    }
}

inline void SqliteReader::WalkTable(
    std::uint32_t root_page, const std::function<void(std::int64_t, const std::vector<Column>&)>& callback) const {
    std::vector<bool> visited(PageCount() + 1, false);
    WalkTablePage(root_page, callback, visited, 0);
}

inline void SqliteReader::Fail(const std::string& message) {
    throw std::runtime_error("Grid++ SQLite 錯誤：" + message);
}

inline void SqliteReader::RequireRange(std::size_t offset, std::size_t length, std::size_t end,
                                       const char* what) const {
    if (offset > end || length > end - offset) Fail(std::string(what) + "超出檔案範圍");
}

inline std::size_t SqliteReader::PageCount() const { return data_.size() / page_size_; }

inline std::size_t SqliteReader::PageOffset(std::uint32_t page_number) const {
    if (page_number == 0 || page_number > PageCount()) Fail("頁碼超出檔案範圍");
    return static_cast<std::size_t>(page_number - 1) * page_size_;
}

inline std::uint16_t SqliteReader::ReadBigEndianUint16(std::size_t offset) const {
    RequireRange(offset, 2, data_.size(), "16-bit 整數");
    return static_cast<std::uint16_t>((static_cast<std::uint16_t>(data_[offset]) << 8) |
                                      static_cast<std::uint16_t>(data_[offset + 1]));
}

inline std::uint32_t SqliteReader::ReadBigEndianUint32(std::size_t offset) const {
    RequireRange(offset, 4, data_.size(), "32-bit 整數");
    return (static_cast<std::uint32_t>(data_[offset]) << 24) | (static_cast<std::uint32_t>(data_[offset + 1]) << 16) |
           (static_cast<std::uint32_t>(data_[offset + 2]) << 8) | static_cast<std::uint32_t>(data_[offset + 3]);
}

inline std::uint64_t SqliteReader::ReadVarint(std::size_t& offset, std::size_t end) const {
    std::uint64_t value = 0;
    for (int i = 0; i < 9; ++i) {
        RequireRange(offset, 1, end, "varint");
        const unsigned char byte = data_[offset++];
        if (i == 8) return (value << 8) | byte;
        value = (value << 7) | (byte & 0x7f);
        if ((byte & 0x80) == 0) return value;
    }
    Fail("varint 無效");
    return 0;
}

inline std::int64_t SqliteReader::UnsignedBitsToInt64(std::uint64_t value) {
    if (value <= static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
        return static_cast<std::int64_t>(value);
    }
    return -1 - static_cast<std::int64_t>(~value);
}

inline std::int64_t SqliteReader::ReadBigEndianInt64(const std::vector<unsigned char>& bytes, std::size_t offset,
                                                     std::size_t length) {
    if (length < 1 || length > 8 || offset > bytes.size() || length > bytes.size() - offset) {
        Fail("有號整數超出 record 範圍");
    }

    std::uint64_t value = 0;
    for (std::size_t i = 0; i < length; ++i) value = (value << 8) | bytes[offset + i];
    if (length < 8 && (bytes[offset] & 0x80)) value |= ~std::uint64_t{0} << (length * 8);
    return UnsignedBitsToInt64(value);
}

inline std::uint64_t SqliteReader::ReadRecordVarint(const std::vector<unsigned char>& record, std::size_t& offset,
                                                    std::size_t end) {
    std::uint64_t value = 0;
    for (int i = 0; i < 9; ++i) {
        if (offset >= end || offset >= record.size()) Fail("record varint 超出範圍");
        const unsigned char byte = record[offset++];
        if (i == 8) return (value << 8) | byte;
        value = (value << 7) | (byte & 0x7f);
        if ((byte & 0x80) == 0) return value;
    }
    Fail("record varint 無效");
    return 0;
}

inline std::vector<Column> SqliteReader::DecodeRecord(const std::vector<unsigned char>& record) {
    std::size_t header_offset = 0;
    const std::uint64_t header_size_64 = ReadRecordVarint(record, header_offset, record.size());
    if (header_size_64 < header_offset || header_size_64 > record.size()) Fail("record header size 無效");
    const std::size_t header_size = static_cast<std::size_t>(header_size_64);
    std::size_t content_offset = header_size;
    std::vector<Column> columns;

    while (header_offset < header_size) {
        const std::uint64_t serial = ReadRecordVarint(record, header_offset, header_size);
        Column column;
        std::size_t length = 0;

        if (serial == 0) {
            column.kind = 2;
        } else if (serial >= 1 && serial <= 6) {
            length = serial <= 4 ? static_cast<std::size_t>(serial) : (serial == 5 ? 6 : 8);
            column.kind = 0;
            column.integer = ReadBigEndianInt64(record, content_offset, length);
        } else if (serial == 7) {
            length = 8;
            column.kind = 3;
        } else if (serial == 8 || serial == 9) {
            column.kind = 0;
            column.integer = serial == 9 ? 1 : 0;
        } else if (serial == 10 || serial == 11) {
            Fail("遇到 SQLite 保留的 serial type");
        } else {
            const std::uint64_t length_64 = (serial - 12) / 2;
            if (length_64 > record.size() - content_offset) Fail("欄位內容超出 record 範圍");
            length = static_cast<std::size_t>(length_64);
            column.kind = 1;
            column.bytes.assign(record.begin() + content_offset, record.begin() + content_offset + length);
        }

        if (length > record.size() - content_offset) Fail("欄位內容超出 record 範圍");
        content_offset += length;
        columns.push_back(std::move(column));
    }

    if (content_offset != record.size()) Fail("record 內容長度不一致");
    return columns;
}

inline void SqliteReader::MarkPage(std::uint32_t page_number, std::vector<bool>& visited) const {
    if (page_number == 0 || page_number >= visited.size()) Fail("頁碼超出檔案範圍");
    if (visited[page_number]) Fail("偵測到重複或循環頁面");
    visited[page_number] = true;
}

inline void SqliteReader::WalkTablePage(std::uint32_t page_number,
                                        const std::function<void(std::int64_t, const std::vector<Column>&)>& callback,
                                        std::vector<bool>& visited, std::size_t depth) const {
    if (depth > 64) Fail("B-tree 過深");
    MarkPage(page_number, visited);

    const std::size_t start = PageOffset(page_number);
    const std::size_t end = start + usable_size_;
    // 第 1 頁的 B-tree header 位於 100-byte database header 之後。
    const std::size_t header = start + (page_number == 1 ? 100 : 0);
    RequireRange(header, 8, end, "B-tree header");

    const unsigned char type = data_[header];
    if (type != 0x05 && type != 0x0d) Fail("不是 table B-tree 頁面");
    const std::size_t header_size = type == 0x05 ? 12 : 8;
    RequireRange(header, header_size, end, "B-tree header");

    const std::uint16_t cell_count = ReadBigEndianUint16(header + 3);
    const std::size_t cell_pointers = header + header_size;
    RequireRange(cell_pointers, static_cast<std::size_t>(cell_count) * 2, end, "cell pointer array");

    // Interior page 先依 cell 順序走訪左側 children，最後才走訪 right-most child。
    if (type == 0x05) {
        for (std::uint16_t i = 0; i < cell_count; ++i) {
            const std::size_t cell = start + ReadBigEndianUint16(cell_pointers + i * 2);
            if (cell < cell_pointers + static_cast<std::size_t>(cell_count) * 2) {
                Fail("interior table cell 與 header 重疊");
            }
            RequireRange(cell, 4, end, "interior table cell");
            WalkTablePage(ReadBigEndianUint32(cell), callback, visited, depth + 1);
        }
        WalkTablePage(ReadBigEndianUint32(header + 8), callback, visited, depth + 1);
        return;
    }

    // Leaf page 的每個 cell 依序儲存 payload length、rowid 與 record payload。
    for (std::uint16_t i = 0; i < cell_count; ++i) {
        std::size_t cell = start + ReadBigEndianUint16(cell_pointers + i * 2);
        if (cell < cell_pointers + static_cast<std::size_t>(cell_count) * 2) Fail("leaf table cell 與 header 重疊");
        RequireRange(cell, 1, end, "leaf table cell");
        const std::uint64_t payload_length_64 = ReadVarint(cell, end);
        const std::int64_t row_id = UnsignedBitsToInt64(ReadVarint(cell, end));
        if (payload_length_64 > data_.size()) Fail("payload length 無效");
        const std::size_t payload_length = static_cast<std::size_t>(payload_length_64);

        // 依 SQLite file format 的 table leaf 公式計算留在本頁的 payload 大小。
        const std::size_t max_local = usable_size_ - 35;
        std::size_t local = payload_length;
        if (payload_length > max_local) {
            const std::size_t min_local = ((usable_size_ - 12) * 32 / 255) - 23;
            const std::size_t candidate = min_local + (payload_length - min_local) % (usable_size_ - 4);
            local = candidate <= max_local ? candidate : min_local;
        }

        const std::size_t overflow_pointer = payload_length > local ? 4 : 0;
        RequireRange(cell, local + overflow_pointer, end, "leaf payload");
        std::vector<unsigned char> payload(data_.begin() + cell, data_.begin() + cell + local);

        std::uint32_t overflow_page = overflow_pointer != 0 ? ReadBigEndianUint32(cell + local) : 0;
        std::size_t remaining = payload_length - local;
        // 每個 overflow page 的前 4 bytes 指向下一頁，其餘空間才是 payload。
        while (remaining > 0) {
            if (overflow_page == 0) Fail("overflow chain 提早結束");
            MarkPage(overflow_page, visited);
            const std::size_t overflow_start = PageOffset(overflow_page);
            RequireRange(overflow_start, usable_size_, overflow_start + page_size_, "overflow page");
            const std::uint32_t next = ReadBigEndianUint32(overflow_start);
            const std::size_t take = std::min(usable_size_ - 4, remaining);
            payload.insert(payload.end(), data_.begin() + overflow_start + 4,
                           data_.begin() + overflow_start + 4 + take);
            remaining -= take;
            overflow_page = next;
        }
        if (overflow_page != 0) Fail("overflow chain 長度超過 payload");

        callback(row_id, DecodeRecord(payload));
    }
}

}  // namespace gridpp::internal

#endif  // GRID_PLUS_PLUS_GRID_SQLITE_H_
