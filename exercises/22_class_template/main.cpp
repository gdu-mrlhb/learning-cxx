#include "../exercise.h"
#include <cstring>
// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = shape_[0]*shape_[1]*shape_[2]*shape_[3];
        // TODO: 填入正确的 shape 并计算 size
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
        std::memcpy(shape, shape_, 4 * sizeof(unsigned int));
    }
    ~Tensor4D() {
        delete[] data;
    }

    // 为了保持简单，禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。
    Tensor4D &operator+=(Tensor4D const &others) {
        // TODO: 实现单向广播的加法

// ========== 第一步：合法性校验（单向广播的核心约束，必做） ==========
        // 规则：others的任意维度，要么和this的维度长度相等，要么等于1
        for (int dim = 0; dim < 4; ++dim) {
            bool valid = (this->shape[dim] == others.shape[dim]) || (others.shape[dim] == 1);
            ASSERT(valid, "Tensor4D broadcast += error: others dim must be 1 or equal to this dim!");
        }

        // ========== 第二步：预计算维度的步长，加速内存偏移计算 ==========
        const unsigned int* s = this->shape;
        const unsigned int* o_s = others.shape;
        const unsigned int total0 = s[0];
        const unsigned int total1 = s[1];
        const unsigned int total2 = s[2];
        const unsigned int total3 = s[3];

        // ========== 第三步：遍历this的所有元素，执行广播加法 ==========
        // 四重循环遍历4维索引 [i0, i1, i2, i3]，覆盖this的全部元素
        for (unsigned int i0 = 0; i0 < total0; ++i0) {
            // 广播映射：others的dim0索引，长度为1则取0，否则取i0
            unsigned int o0 = (o_s[0] == 1) ? 0 : i0;
            for (unsigned int i1 = 0; i1 < total1; ++i1) {
                unsigned int o1 = (o_s[1] == 1) ? 0 : i1;
                for (unsigned int i2 = 0; i2 < total2; ++i2) {
                    unsigned int o2 = (o_s[2] == 1) ? 0 : i2;
                    for (unsigned int i3 = 0; i3 < total3; ++i3) {
                        unsigned int o3 = (o_s[3] == 1) ? 0 : i3;

                        // 计算当前元素在this中的一维偏移量
                        unsigned int self_offset = i0*s[1]*s[2]*s[3] + i1*s[2]*s[3] + i2*s[3] + i3;
                        // 计算广播后，对应元素在others中的一维偏移量
                        unsigned int other_offset = o0*o_s[1]*o_s[2]*o_s[3] + o1*o_s[2]*o_s[3] + o2*o_s[3] + o3;

                        // 核心运算：广播加法
                        data[self_offset] += others.data[other_offset];
                    }
                }
            }
        }
        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}
