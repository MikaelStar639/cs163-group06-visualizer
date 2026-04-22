#include "Core/Platform.hpp"
#include <cstdlib>
#include <iostream>

namespace Core::Platform {

    void openTextEditor(const std::string& filePath) {
#if defined(_WIN32)
        // Windows: Use 'start notepad'
        std::string command = "start notepad \"" + filePath + "\"";
        std::system(command.c_str());
#elif defined(__APPLE__)
        // macOS: Use 'open -t' (Default text editor)
        std::string command = "open -t \"" + filePath + "\"";
        std::system(command.c_str());
#elif defined(__linux__)
        // Linux: Use 'xdg-open'
        std::string command = "xdg-open \"" + filePath + "\"";
        std::system(command.c_str());
#else
        std::cout << "[PLATFORM] Unsupported OS for opening editor: " << filePath << std::endl;
#endif
    }

} // namespace Core::Platform
