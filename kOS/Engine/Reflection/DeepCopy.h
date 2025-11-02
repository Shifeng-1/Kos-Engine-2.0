#pragma once

template <typename T>
struct DeepCopyComponents {
    int count = 0;

    // Float
    void operator()(float& dest, float& source) {

        dest = source;
        count++;
    }

    // Int
    void operator()(int& dest, int& source) {
        dest = source;
        count++;
    }

    // Bool
    void operator()(bool& dest, bool& source) {
        dest = source;
        count++;
    }

    // String
    void operator()(std::string& dest, std::string& source) {
        dest = source;
        count++;
    }

    // Enum
    template <typename EnumType>
        requires std::is_enum_v<EnumType>
    void operator()(EnumType& dest, EnumType& source) {
        dest = source;
        count++;
    }

    // glm::vec2
    void operator()(glm::vec2& dest, glm::vec2& source) {
        dest = source;
        count++;
    }

    // glm::vec3
    void operator()(glm::vec3& dest, glm::vec3& source) {
        dest = source;
        count++;
    }

    // glm::vec4
    void operator()(glm::vec4& dest, glm::vec4& source) {
        dest = source;
        count++;
    }

    // std::vector
    template <typename U>
    void operator()(std::vector<U>& dest, const std::vector<U>& source) {
        dest.resize(source.size());
        for (size_t i = 0; i < source.size(); ++i) {
            (*this)(dest[i], source[i]);
        }
        count++;
    }

    // std::unique_ptr
    template <typename U>
    void operator()(std::unique_ptr<U>& dest, const std::unique_ptr<U>& source) {
        if (source)
            dest = std::make_unique<U>(*source);
        else
            dest.reset();
        count++;
    }

    // Nested reflected classes
    template <typename K>
    void operator()(K& dest, const K& source) {
        if constexpr (std::is_class_v<K> && requires { dest.Names(); }) {
            dest.ApplyFunctionPairwise(*this, source); // recurse into reflected fields
        }
        else {
            dest = source; // fallback shallow copy
        }
        count++;
    }
};