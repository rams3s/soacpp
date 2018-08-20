#include "soa.h"
#include <array>
#include <chrono>
#include <iostream>
#include <vector>

void example_1()
{
    using vec3 = std::array<float, 3>;

    using is_active = bool;
    using position = vec3;
    struct velocity : public vec3
    {
    };
    using color = uint32_t;

    soacpp::soa_vector<is_active, position, velocity, color> particles( 128 );

    is_active current_is_active = true;

    for ( auto particle : particles )
    {
        current_is_active ^= 1;

        // :TRICKY: cannot use bool or alias because type is vector<bool>::reference
        std::get<0>( particle ) = current_is_active;

        std::get<position &>( particle )[0] = 0.0f;
        std::get<position &>( particle )[1] = 1.0f;
        std::get<position &>( particle )[2] = 2.0f;

        std::get<velocity &>( particle )[0] = 3.0f;
        std::get<velocity &>( particle )[1] = 4.0f;
        std::get<velocity &>( particle )[2] = 5.0f;

        std::get<color &>( particle ) = 0xFF0000FF;
    }
}

struct Particle
{
    using vec3 = std::array<float, 3>;

    vec3 position;
    uint32_t color;
    bool is_active;
    unsigned char _padding[3];
};

DECLARE_SOA_TYPE( Particle, is_active, position, color )

void example_2()
{
    static constexpr auto count = 100'000;

    auto test = []( auto particles ) {
        for ( std::size_t i = 0; i < count; ++i )
        {
            decltype( auto ) particle = particles[i];

            particle.is_active = i % 3;

            if ( particle.is_active )
            {
                particle.position[0] = 0.0f;
                particle.position[1] = 1.0f;
                particle.position[2] = 2.0f;
                particle.color = 0xFF0000FF;
            }
        }

        auto start = std::chrono::steady_clock::now();

        constexpr auto loop_count = 10'000;

        for ( std::size_t j = 0; j < loop_count; ++j )
        {
            for ( decltype( auto ) particle : particles )
            {
                if ( particle.is_active )
                {
                    particle.position[0] += 3.0f;
                }

                // std::cout << i << ". pos: {" << particle.position[0] << ", " << particle.position[1] << ", " <<
                // particle.position[2] << "}, color: " << particle.color << std::endl;
            }
        }

        auto end = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count() / loop_count
                  << std::endl;
    };

    test( std::vector<Particle>( count ) );
    test( soacpp::soa_vector<Particle>( count ) );
    test( soacpp::soa_array<count, Particle>() );
}

int main()
{
    example_1();
    example_2();

    return 0;
}
