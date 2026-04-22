#pragma once
#include <string>

namespace Core::Platform {

    /**
     * @brief Opens the default text editor for the given file path.
     * Supports Windows (notepad), macOS (open -t), and Linux (xdg-open).
     */
    void openTextEditor(const std::string& filePath);

} // namespace Core::Platform
