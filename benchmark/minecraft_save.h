#pragma once
#include <random>
#include <string>
#include <vector>

namespace alpaca {

namespace benchmark {

enum class GameType {
    SURVIVAL,
    CREATIVE,
    ADVENTURE,
    SPECTATOR
};

struct Item {
    int32_t count;
    uint32_t slot;
    std::string id;
};

struct Abilities {
    float walk_speed;
    float fly_speed;
    bool may_fly;
    bool flying;
    bool invulnerable;
    bool may_build;
    bool instabuild;
};

struct Vector3d {
    double x;
    double y;
    double z;
};

struct Vector2f {
    float x;
    float y;
};

struct Uuid {
    uint32_t x0;
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
};

struct Entity {
    std::string id;
    Vector3d pos;
    Vector3d motion;
    Vector2f rotation;
    float fall_distance;
    uint32_t fire;
    uint32_t air;
    bool on_ground;
    bool no_gravity;
    bool invulnerable;
    int32_t portal_cooldown;
    Uuid uuid;
    std::string custom_name;
    bool custom_name_visible;
    bool silent;
    bool glowing;
};

struct RecipeBook {
    std::vector<std::string> recipes;
    std::vector<std::string> to_be_displayed;
    bool is_filtering_craftable;
    bool is_gui_open;
    bool is_furnace_filtering_craftable;
    bool is_furnace_gui_open;
    bool is_blasting_furnace_filtering_craftable;
    bool is_blasting_furnace_gui_open;
    bool is_smoker_filtering_craftable;
    bool is_smoker_gui_open;
};

struct Vehicle {
    Uuid uuid;
    Entity entity;
};

struct Player {
    GameType game_type;
    GameType previous_game_type;
    int64_t score;
    std::string dimension;
    uint32_t selected_item_slot;
    Item selected_item;
    std::string spawn_dimension;
    int64_t spawn_x;
    int64_t spawn_y;
    int64_t spawn_z;
    bool spawn_forced;
    uint32_t sleep_timer;
    float food_exhaustion_level;
    float food_saturation_level;
    uint32_t food_tick_timer;
    uint32_t xp_level;
    float xp_p;
    int32_t xp_total;
    int32_t xp_seed;
    std::vector<Item> inventory;
    std::vector<Item> ender_items;
    Abilities abilities;
    Vector3d entered_nether_position;
    Vehicle root_vehicle;
    Entity shoulder_entity_left;
    Entity shoulder_entity_right;
    bool seen_credits;
    RecipeBook recipe_book;
};

struct Players {
    std::vector<Player> players;
};

static inline
std::string generate_string(std::size_t N) {
    auto randchar = []() -> char
    {
        const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(N, 0);
    std::generate_n(str.begin(), N, randchar);
    return str;
}

template <typename Engine>
auto generate_game_type(Engine& eng) {
    std::uniform_int_distribution<> distr(0, 3);
    auto index = distr(eng);
    if (index == 0) {
        return GameType::SURVIVAL;
    }
    else if (index == 1) {
        return GameType::CREATIVE;
    }
    else if (index == 2) {
        return GameType::CREATIVE;
    }
    else if (index == 3) {
        return GameType::CREATIVE;
    }
    else {
        return GameType::SURVIVAL;
    }
}

template <typename Engine>
auto generate_item(Engine& eng) {
    std::uniform_int_distribution<uint32_t> uint_distr(0, 20);
    std::uniform_int_distribution<int32_t> int_distr(0, 100);

    return Item {
        int_distr(eng),
        uint_distr(eng),
        generate_string(32)
    };
}

template <typename Engine>
auto generate_item_vec(Engine& eng) {
    std::vector<Item> result{};
    std::uniform_int_distribution<uint32_t> uint_distr(0, 20);
    constexpr auto size = 15;

    for (std::size_t i = 0; i < size; ++i) {
        result.push_back(generate_item(eng));
    }

    return result;
}

template <typename Engine>
auto generate_abilities(Engine& eng) {
    std::uniform_real_distribution<float> float_distr(0, 100);
    std::uniform_int_distribution<uint32_t> uint_distr(0, 60);  

    return Abilities {
        float_distr(eng),
        float_distr(eng),
        uint_distr(eng) % 2 == 0,
        uint_distr(eng) % 3 == 0,
        uint_distr(eng) % 5 == 0,
        uint_distr(eng) % 7 == 0,
        uint_distr(eng) % 13 == 0
    };
}

template <typename Engine>
auto generate_vector3d(Engine& eng) {
    std::uniform_real_distribution<> double_distr(-100, 100);

    return Vector3d {
        double_distr(eng),
        double_distr(eng),
        double_distr(eng)
    };
}

template <typename Engine>
auto generate_vector2f(Engine& eng) {
    std::uniform_real_distribution<float> float_distr(-100, 100);

    return Vector2f {
        float_distr(eng),
        float_distr(eng)
    };
}

template <typename Engine>
auto generate_uuid(Engine& eng) {
    std::uniform_int_distribution<uint32_t> distr(0, 10000);

    return Uuid {
        distr(eng),
        distr(eng),
        distr(eng),
        distr(eng)
    };
}

template <typename Engine>
auto generate_entity(Engine& eng) {
    std::uniform_real_distribution<float> float_distr(-100, 100);
    std::uniform_int_distribution<uint32_t> uint_distr(0, 10000);
    std::uniform_int_distribution<int32_t> int_distr(0, 200);

    return Entity {
        generate_string(32),
        generate_vector3d(eng),
        generate_vector3d(eng),
        generate_vector2f(eng),
        float_distr(eng),
        uint_distr(eng),
        uint_distr(eng),
        uint_distr(eng) % 14 == 0,
        uint_distr(eng) % 5 == 0,
        uint_distr(eng) % 9 == 0,
        int_distr(eng),
        generate_uuid(eng),
        generate_string(16),
        uint_distr(eng) % 3 == 0,
        uint_distr(eng) % 5 == 0,
        uint_distr(eng) % 7 == 0
    };
}

template <typename Engine>
auto generate_recipe_book(Engine& eng) {
    RecipeBook result{};

    std::uniform_int_distribution<uint32_t> uint_distr(0, 50);
    constexpr auto size = 30;
    for (std::size_t i = 0; i < size; ++i) {
        result.recipes.push_back(generate_string(12));
    }

    constexpr auto size2 = 10;
    for (std::size_t i = 0; i < size2; ++i) {
        result.to_be_displayed.push_back(generate_string(12));
    }

    result.is_filtering_craftable = uint_distr(eng) % 2 == 0;
    result.is_gui_open = uint_distr(eng) % 2 == 0;
    result.is_furnace_filtering_craftable = uint_distr(eng) % 2 == 0;
    result.is_furnace_gui_open = uint_distr(eng) % 2 == 0;
    result.is_blasting_furnace_filtering_craftable = uint_distr(eng) % 2 == 0;
    result.is_blasting_furnace_gui_open = uint_distr(eng) % 2 == 0;
    result.is_smoker_filtering_craftable = uint_distr(eng) % 2 == 0;
    result.is_smoker_gui_open = uint_distr(eng) % 2 == 0;

    return result;
}

template <typename Engine>
auto generate_vehicle(Engine& eng) {
    return Vehicle {
        generate_uuid(eng),
        generate_entity(eng)
    };
}

template <typename Engine>
auto generate_player(Engine& eng) {
    std::uniform_real_distribution<float> float_distr(-100, 100);
    std::uniform_int_distribution<uint32_t> uint32_distr(0, 100000);
    std::uniform_int_distribution<int32_t> int32_distr(-2000, 2000);
    std::uniform_int_distribution<uint64_t> uint64_distr(0, 10000);
    std::uniform_int_distribution<int64_t> int64_distr(-10000, 10000);

    return Player {
        generate_game_type(eng),
        generate_game_type(eng),
        int64_distr(eng),
        generate_string(16),
        uint32_distr(eng),
        generate_item(eng),
        generate_string(16),
        int64_distr(eng),
        int64_distr(eng),
        int64_distr(eng),
        uint32_distr(eng) % 2 == 0,
        uint32_distr(eng),
        float_distr(eng),
        float_distr(eng),
        uint32_distr(eng),
        uint32_distr(eng),
        float_distr(eng),
        int32_distr(eng),
        int32_distr(eng),
        generate_item_vec(eng),
        generate_item_vec(eng),
        generate_abilities(eng),
        generate_vector3d(eng),
        generate_vehicle(eng),
        generate_entity(eng),
        generate_entity(eng),
        uint32_distr(eng) % 2 == 0,
        generate_recipe_book(eng)
    };
}

}

}