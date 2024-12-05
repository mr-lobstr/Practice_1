#ifndef CRYPTO_STATE_H_GUARD
#define CRYPTO_STATE_H_GUARD

#include <string>
#include <format>

struct State {
    static std::string is_init () {
        return "SELECT state.is_init FROM state";
    }

    static std::string get_default_quantity() {
        return "SELECT state.default_quantity FROM state";
    }

    static std::string init (double defaultQuantity) {
        return std::format (
            "INSERT INTO state VALUES ('init', '{}')",
            defaultQuantity
        );
    }
};

#endif