/** @file GridValue.h
 *  @brief GridObject 與 Overlay 使用的輕量鍵值儲存。
 */
#ifndef GRID_PLUS_PLUS_GRID_VALUE_H_
#define GRID_PLUS_PLUS_GRID_VALUE_H_

#include <any>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace gridpp {

class GridValueStore {
public:
    template <typename T>
    void set(const std::string& key, T value) {
        const auto found = values_.find(key);
        if (found != values_.end() && found->second.type() != typeid(T)) {
            throw std::runtime_error("Grid++ Error: Value '" + key + "' was already stored with a different type");
        }
        values_[key] = std::move(value);
    }

    template <typename T>
    int get(const std::string& key, T& value) const {
        const auto found = values_.find(key);
        if (found == values_.end()) {
            value = T{};
            return 1;
        }

        const T* stored = std::any_cast<T>(&found->second);
        if (stored == nullptr) {
            throw std::runtime_error("Grid++ Error: Value '" + key + "' was requested with the wrong type");
        }
        value = *stored;
        return 0;
    }

private:
    std::unordered_map<std::string, std::any> values_;
};

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_VALUE_H_
