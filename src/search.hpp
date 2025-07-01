#include <stdexcept>

// Our custom error
struct SearchAbort : public std::exception {
    const char* what() const noexcept override {
        return "Abort Search!!!! Ahhh!!";
    }
};