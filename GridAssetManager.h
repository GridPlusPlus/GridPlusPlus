/** @file GridAssetManager.h
 *  @brief 定義 GameEngineImpl 使用的素材管理器。
 */
#ifndef GRID_PLUS_PLUS_GRID_ASSET_MANAGER_H_
#define GRID_PLUS_PLUS_GRID_ASSET_MANAGER_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "GridSQLite.h"
#include "raylib.h"

namespace gridpp {

/** 從 Grid++ SQLite 素材包建立並管理 raylib texture。 */
class GridAssetManager {
public:
    GridAssetManager() = default;
    ~GridAssetManager();

    GridAssetManager(const GridAssetManager&) = delete;
    GridAssetManager& operator=(const GridAssetManager&) = delete;

    /**
     * 載入新素材包；全部素材成功後才取代目前內容。
     * @throws std::runtime_error 若檔案無法讀取、SQLite 格式無效或缺少 `sprites` 資料表。
     */
    void Load(const std::filesystem::path& path);

    /**
     * @return 名稱唯一的 texture。
     * @throws std::runtime_error 若名稱不存在或有重複記錄。
     */
    Texture2D Get(const std::string& name) const;

    /** @return 是否至少有一筆指定名稱的素材。 */
    bool Has(const std::string& name) const;

    /** 釋放全部 GPU texture；可重複呼叫。 */
    void Clear() noexcept;

private:
    static void Unload(std::multimap<std::string, Texture2D>& source) noexcept;

    /** 將 32×32 RGBA 資料上傳為 raylib texture。 */
    static Texture2D MakeTexture(const std::vector<unsigned char>& rgba);

    std::multimap<std::string, Texture2D> textures_;
};

// Inline definitions ---------------------------------------------------------

inline GridAssetManager::~GridAssetManager() { Clear(); }

inline void GridAssetManager::Load(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("GridAssetManager Error: Cannot open '" + path.string() + "'");
    const std::vector<unsigned char> data{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    internal::SqliteReader database(data);

    // sqlite_master 欄位依序為 type、name、tbl_name、rootpage、sql。
    std::uint32_t sprites_root = 0;
    database.WalkTable(1, [&](std::int64_t, const std::vector<internal::Column>& columns) {
        if (columns.size() >= 4 && columns[0].kind == 1 && columns[3].kind == 0) {
            const std::string type(columns[0].bytes.begin(), columns[0].bytes.end());
            const std::string name(columns[2].bytes.begin(), columns[2].bytes.end());
            if (type == "table" && name == "sprites") sprites_root = static_cast<std::uint32_t>(columns[3].integer);
        }
    });
    if (sprites_root == 0) {
        throw std::runtime_error("GridAssetManager Error: Table 'sprites' not found in '" + path.string() + "'");
    }

    std::multimap<std::string, Texture2D> loaded_textures;
    try {
        // sprites 欄位依序為 id、name、tags、image_data。
        database.WalkTable(sprites_root, [&](std::int64_t, const std::vector<internal::Column>& columns) {
            if (columns.size() < 4) return;
            const std::string name(columns[1].bytes.begin(), columns[1].bytes.end());
            const std::vector<unsigned char>& blob = columns[3].bytes;
            if (blob.size() != 4096) {
                std::cout << "Grid++ 警告：素材 '" << name << "' 不是 32x32 RGBA，已略過。\n";
                return;
            }

            const Texture2D texture = MakeTexture(blob);
            try {
                loaded_textures.emplace(name, texture);
            } catch (...) {
                UnloadTexture(texture);
                throw;
            }
        });

        // 延後檢查可以在離開 SQLite reader 後一次列出每個重複名稱。
        for (auto it = loaded_textures.begin(); it != loaded_textures.end();) {
            const std::string& key = it->first;
            const std::size_t count = loaded_textures.count(key);
            if (count > 1) {
                std::cout << "Grid++ 警告：素材名稱 '" << key << "' 重複了 " << count
                          << " 次！之後呼叫 Get(\"" << key << "\") 會直接報錯。\n";
            }
            it = loaded_textures.upper_bound(key);
        }
    } catch (...) {
        Unload(loaded_textures);
        throw;
    }

    // 新素材全部成功後才取代舊素材，避免載入失敗時失去原本資源。
    Clear();
    textures_.swap(loaded_textures);
}

inline Texture2D GridAssetManager::Get(const std::string& name) const {
    const std::size_t count = textures_.count(name);
    if (count == 0) throw std::runtime_error("Grid++ 錯誤：找不到素材 '" + name + "'");
    if (count > 1) {
        throw std::runtime_error("Grid++ 錯誤：素材名稱 '" + name + "' 重複出現 " + std::to_string(count) +
                                 " 次，無法分辨你要哪一個！請讓素材名稱保持唯一。");
    }
    return textures_.find(name)->second;
}

inline bool GridAssetManager::Has(const std::string& name) const { return textures_.count(name) != 0; }

inline void GridAssetManager::Clear() noexcept { Unload(textures_); }

inline void GridAssetManager::Unload(std::multimap<std::string, Texture2D>& source) noexcept {
    for (const auto& entry : source) UnloadTexture(entry.second);
    source.clear();
}

inline Texture2D GridAssetManager::MakeTexture(const std::vector<unsigned char>& rgba) {
    Image image = {};
    image.data = const_cast<unsigned char*>(rgba.data());
    image.width = 32;
    image.height = 32;
    image.mipmaps = 1;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    return LoadTextureFromImage(image);
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_ASSET_MANAGER_H_
