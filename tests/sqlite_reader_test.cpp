#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "GridSQLite.h"

using gridpp::internal::Column;
using gridpp::internal::SqliteReader;

static std::vector<unsigned char> ReadFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

static bool IsRejected(const std::vector<unsigned char>& data) {
    try {
        SqliteReader reader(data);
        (void)reader;
        return false;
    } catch (const std::runtime_error&) {
        return true;
    }
}

static bool IsTableRejected(const std::vector<unsigned char>& data) {
    try {
        SqliteReader reader(data);
        reader.WalkTable(1, [](std::int64_t, const std::vector<Column>&) {});
        return false;
    } catch (const std::runtime_error&) {
        return true;
    }
}

int main(int argc, char** argv) {
    const std::vector<unsigned char> data = ReadFile(argc > 1 ? argv[1] : "examples/pacman/pacman.db");
    SqliteReader database(data);

    std::uint32_t sprites_root = 0;
    database.WalkTable(1, [&](std::int64_t, const std::vector<Column>& columns) {
        if (columns.size() < 4 || columns[0].kind != 1 || columns[2].kind != 1 || columns[3].kind != 0) return;
        const std::string type(columns[0].bytes.begin(), columns[0].bytes.end());
        const std::string name(columns[2].bytes.begin(), columns[2].bytes.end());
        if (type == "table" && name == "sprites") sprites_root = static_cast<std::uint32_t>(columns[3].integer);
    });
    assert(sprites_root != 0);

    std::size_t sprite_count = 0;
    database.WalkTable(sprites_root, [&](std::int64_t, const std::vector<Column>& columns) {
        assert(columns.size() >= 4);
        assert(columns[1].kind == 1);
        assert(columns[3].kind == 1);
        assert(columns[3].bytes.size() == 4096);
        ++sprite_count;
    });
    assert(sprite_count == 9);

    std::vector<unsigned char> bad_magic = data;
    bad_magic[0] = 0;
    assert(IsRejected(bad_magic));

    std::vector<unsigned char> truncated = data;
    truncated.pop_back();
    assert(IsRejected(truncated));

    std::vector<unsigned char> wal = data;
    wal[18] = wal[19] = 2;
    assert(IsRejected(wal));

    std::vector<unsigned char> bad_cell = data;
    bad_cell[108] = bad_cell[109] = 0;
    assert(IsTableRejected(bad_cell));
}
