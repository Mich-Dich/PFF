
#include "util/pffpch.h"

#include "random.h"

namespace PFF::util {

    glm::vec3 random::get_vec3(f32 min, f32 max) { return glm::vec3(get<f32>(min, max), get<f32>(min, max), get<f32>(min, max)); }

    bool random::get_percent(f32 percentage) { return get<f32>(0.f, 1.f) < percentage; }

    //f32 random::get_f32(f32 min, f32 max) {
    //    std::uniform_real_distribution<f32> dist(min, max);
    //    return dist(engine);
    //}

    //f64 random::get_f64(f64 min, f64 max) {
    //    std::uniform_real_distribution<f64> dist(min, max);
    //    return dist(engine);
    //}

    //u32 random::get_u32(u32 min, u32 max) {
    //    std::uniform_int_distribution<u32> dist(min, max);
    //    return dist(engine);
    //}

    //u64 random::get_u64(u64 min, u64 max) {
    //    std::uniform_int_distribution<u64> dist(min, max);
    //    return dist(engine);
    //}


    std::string random::get_string(const size_t length) {

        const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);

        std::string result;
        result.resize(length);
        std::generate(result.begin(), result.end(), [&]() { return charset[dist(engine)]; });
        return result;
    }

}
