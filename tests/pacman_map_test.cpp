#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>

#include "examples/pacman/LevelMap.h"

using pacman_example::LevelMap;
using pacman_example::LoadLevelMap;

static bool Rejects(const std::string& text) {
    try {
        std::istringstream input(text);
        (void)LoadLevelMap(input);
        return false;
    } catch (const std::runtime_error&) {
        return true;
    }
}

int main() {
    std::istringstream valid("2 3  1 2 0  1 3 0");
    const LevelMap level = LoadLevelMap(valid);
    assert(level.rows == 2);
    assert(level.cols == 3);
    assert(level.tiles[0][1] == 2);

    assert(Rejects("not a map"));
    assert(Rejects("0 2"));
    assert(Rejects("65 1"));
    assert(Rejects("1 2 2"));
    assert(Rejects("1 2 2 4"));
    assert(Rejects("1 2 0 1"));
    assert(Rejects("1 2 2 2"));
    assert(Rejects("1 1 2 extra"));

    bool missing_file_rejected = false;
    try {
        (void)LoadLevelMap("this-map-does-not-exist.txt");
    } catch (const std::runtime_error&) {
        missing_file_rejected = true;
    }
    assert(missing_file_rejected);
}
