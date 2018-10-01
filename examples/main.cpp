#include "soa.h"

#include <array>
#include <chrono>
#include <iostream>

void example_v001()
{
    using vec3 = std::array<float, 3>;

    using is_active = bool;
    struct position : public vec3 {};
    struct velocity : public vec3 {};
    using color = uint32_t;

    constexpr auto particle_count = 100'000;

    soacpp::vector<is_active, position, velocity, color> particles( particle_count );

    for ( std::size_t i = 0, count = particles.size(); i < count; ++i )
    {
        auto particle = particles[i];

        // :TRICKY: cannot use is_active& because type is vector<bool>::reference
        std::get<0>( particle ) = true;

        std::get<position &>( particle )[0] = 0.0f;
        std::get<position &>( particle )[1] = 1.0f;
        std::get<position &>( particle )[2] = 2.0f;

        std::get<velocity &>( particle )[0] = 3.0f;
        std::get<velocity &>( particle )[1] = 4.0f;
        std::get<velocity &>( particle )[2] = 5.0f;

        std::get<color &>( particle ) = 0xFF0000FF;
    }

    auto start = std::chrono::steady_clock::now();

    constexpr auto loop_count = 100;
    constexpr float dt = 0.016f;

    for ( std::size_t j = 0; j < loop_count; ++j )
    {
        for ( std::size_t i = 0, count = particles.size(); i < count; ++i )
        {
            auto particle = particles[i];

            if ( std::get<0>( particle ) )
            {
                std::get<position &>( particle )[0] += std::get<velocity &>( particle )[0] * dt;
            }
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "Example 0.0.1: " << particle_count << " particles updated in "
              << std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count() / loop_count
              << " µs (average over " << loop_count << " loops)." << std::endl;
}

int main()
{
    example_v001();

    return 0;
}
