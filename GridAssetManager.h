/**
 * @file GridAssetManager.h
 * @brief GridEngine 內部使用的素材管理器。
 */
#ifndef GRIDASSETMANAGER_H
#define GRIDASSETMANAGER_H

#include "GridSQLite.h"
#include "raylib.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// 從素材資料庫建立 raylib Texture2D。
class GridAssetManager {
public:
    GridAssetManager() = default;
    ~GridAssetManager() { clear(); }

    GridAssetManager(const GridAssetManager&) = delete;
    GridAssetManager& operator=(const GridAssetManager&) = delete;

    void load(const std::string& path) {
        std::ifstream f(path, std::ios::binary);
        if (!f) throw std::runtime_error("Grid++ 錯誤：找不到素材檔 '" + path + "'");
        std::vector<unsigned char> data(
            (std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        gridpp_detail::SQLiteReader database(data);

        // sqlite_master 欄位依序為 type、name、tbl_name、rootpage、sql。
        uint32_t spritesRoot = 0;
        database.walkTable(1,
            [&](int64_t, const std::vector<gridpp_detail::Column>& cols) {
                if (cols.size() >= 4 && cols[0].kind == 1 && cols[3].kind == 0) {
                    std::string ttype(cols[0].bytes.begin(), cols[0].bytes.end());
                    std::string tname(cols[2].bytes.begin(), cols[2].bytes.end());
                    if (ttype == "table" && tname == "sprites")
                        spritesRoot = (uint32_t)cols[3].i;
                }
            });
        if (spritesRoot == 0)
            throw std::runtime_error("Grid++ 錯誤：'" + path + "' 裡找不到 sprites 資料表");

        std::multimap<std::string, Texture2D> loadedTextures;
        try {
            // sprites 欄位依序為 id、name、tags、image_data。
            database.walkTable(spritesRoot,
                [&](int64_t, const std::vector<gridpp_detail::Column>& cols) {
                    if (cols.size() < 4) return;
                    std::string name(cols[1].bytes.begin(), cols[1].bytes.end());
                    const std::vector<unsigned char>& blob = cols[3].bytes;
                    if (blob.size() != 4096) {
                        std::cout << "Grid++ 警告：素材 '" << name
                                  << "' 不是 32x32 RGBA，已略過。\n";
                        return;
                    }

                    Texture2D texture = makeTexture(blob);
                    try {
                        loadedTextures.emplace(name, texture);
                    } catch (...) {
                        UnloadTexture(texture);
                        throw;
                    }
                });

            // 載入後檢查重複名稱。
            for (auto it = loadedTextures.begin(); it != loadedTextures.end(); ) {
                const std::string key = it->first;
                size_t c = loadedTextures.count(key);
                if (c > 1)
                    std::cout << "Grid++ 警告：素材名稱 '" << key << "' 重複了 " << c
                              << " 次！之後呼叫 get(\"" << key << "\") 會直接報錯。\n";
                it = loadedTextures.upper_bound(key);
            }
        } catch (...) {
            unload(loadedTextures);
            throw;
        }

        // 新素材全部成功後才取代舊素材，避免載入失敗時失去原本資源。
        clear();
        textures.swap(loadedTextures);
    }

    void clear() noexcept { unload(textures); }

    // 名稱不存在或重複時丟出例外。
    Texture2D get(const std::string& name) {
        size_t c = textures.count(name);
        if (c == 0)
            throw std::runtime_error("Grid++ 錯誤：找不到素材 '" + name + "'");
        if (c > 1)
            throw std::runtime_error(
                "Grid++ 錯誤：素材名稱 '" + name + "' 重複出現 " + std::to_string(c) +
                " 次，無法分辨你要哪一個！請讓素材名稱保持唯一。");
        return textures.find(name)->second;
    }

    bool has(const std::string& name) const { return textures.count(name) >= 1; }

private:
    static void unload(std::multimap<std::string, Texture2D>& source) noexcept {
        for (const auto& entry : source) UnloadTexture(entry.second);
        source.clear();
    }

    // 將 32×32 RGBA 資料上傳為 raylib texture。
    static Texture2D makeTexture(const std::vector<unsigned char>& rgba) {
        Image img = {};
        img.data    = (void*)rgba.data();
        img.width   = 32;
        img.height  = 32;
        img.mipmaps = 1;
        img.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        return LoadTextureFromImage(img);
    }

    std::multimap<std::string, Texture2D> textures;
};

#endif // GRIDASSETMANAGER_H
