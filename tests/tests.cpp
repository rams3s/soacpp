#include "catch.hpp"

#include "soa.h"

TEST_CASE( "constructors", "[soa]" )
{
    using soa_t = soacpp::vector<uint8_t, float, double, bool>;
    soa_t soa;
    soa_t soa_with_size( 16 );

    static_assert( std::tuple_size<soa_t::data_t>{} == 4, "soa internal tuple should contain 4 containers" );
    static_assert( std::is_same<uint8_t, std::tuple_element_t<0, soa_t::data_t>::value_type>{},
        "internal container at index 0 should be a uint8_t container" );
    static_assert( std::is_same<float, std::tuple_element_t<1, soa_t::data_t>::value_type>{},
        "internal container at index 1 should be a float container" );
    static_assert( std::is_same<double, std::tuple_element_t<2, soa_t::data_t>::value_type>{},
        "internal container at index 2 should be a double container" );
    static_assert( std::is_same<bool, std::tuple_element_t<3, soa_t::data_t>::value_type>{},
        "internal container at index 3 should be a bool container" );

    SECTION( "every sub vector contains the right item count" )
    {
        REQUIRE( soa.array<0>().empty() );
        REQUIRE( soa.array<1>().empty() );
        REQUIRE( soa.array<2>().empty() );
        REQUIRE( soa.array<3>().empty() );

        REQUIRE( soa_with_size.array<0>().size() == 16 );
        REQUIRE( soa_with_size.array<1>().size() == 16 );
        REQUIRE( soa_with_size.array<2>().size() == 16 );
        REQUIRE( soa_with_size.array<3>().size() == 16 );
    }
}

TEST_CASE( "element access", "[soa]" )
{
    soacpp::vector<uint8_t, float, bool> soa( 4 );

    for ( std::size_t i = 0; i < soa.size(); ++i )
    {
        std::get<0>( soa[i] ) = static_cast<uint8_t>( i );
        std::get<1>( soa[i] ) = 2.0f * i;
        std::get<2>( soa[i] ) = ( ( i & 1 ) != 0u );
    }

    SECTION( "operator[]" )
    {
        REQUIRE( std::get<0>( soa[0] ) == 0 );
        REQUIRE( std::get<0>( soa[1] ) == 1 );
        REQUIRE( std::get<0>( soa[2] ) == 2 );
        REQUIRE( std::get<0>( soa[3] ) == 3 );

        REQUIRE( std::get<1>( soa[0] ) == 0.0f );
        REQUIRE( std::get<1>( soa[1] ) == 2.0f );
        REQUIRE( std::get<1>( soa[2] ) == 4.0f );
        REQUIRE( std::get<1>( soa[3] ) == 6.0f );

        REQUIRE( std::get<2>( soa[0] ) == false );
        REQUIRE( std::get<2>( soa[1] ) == true );
        REQUIRE( std::get<2>( soa[2] ) == false );
        REQUIRE( std::get<2>( soa[3] ) == true );
    }
}

TEST_CASE( "Capacity", "[soa]" )
{
    soacpp::vector<uint8_t, float, bool> soa;

    SECTION( "empty" )
    {
        soacpp::vector<uint8_t, float, bool> soa_with_size( 4 );

        REQUIRE( soa.empty() );
        REQUIRE( soa.size() == 0 );

        REQUIRE( !soa_with_size.empty() );
        REQUIRE( soa_with_size.size() == 4 );
    }

    SECTION( "reserve" )
    {
        REQUIRE( soa.empty() );
        REQUIRE( soa.array<0>().capacity() == 0 );
        REQUIRE( soa.array<1>().capacity() == 0 );
        REQUIRE( soa.array<2>().capacity() == 0 );

        soa.reserve( 32 );

        REQUIRE( soa.empty() );
        REQUIRE( soa.array<0>().empty() );
        REQUIRE( soa.array<1>().empty() );
        REQUIRE( soa.array<2>().empty() );

        REQUIRE( soa.capacity() >= 32 );
        REQUIRE( soa.array<0>().capacity() >= 32 );
        REQUIRE( soa.array<1>().capacity() >= 32 );
        REQUIRE( soa.array<2>().capacity() >= 32 );
    }

    SECTION( "resize" )
    {
        REQUIRE( soa.empty() );
        REQUIRE( soa.array<0>().empty() );
        REQUIRE( soa.array<1>().empty() );
        REQUIRE( soa.array<2>().empty() );

        soa.resize( 32 );

        REQUIRE( soa.size() == 32 );
        REQUIRE( soa.array<0>().size() == 32 );
        REQUIRE( soa.array<1>().size() == 32 );
        REQUIRE( soa.array<2>().size() == 32 );
    }
}

TEST_CASE( "Modifiers", "[soa]" )
{
    soacpp::vector<uint8_t, float, bool> soa;

    SECTION( "emplace_back" )
    {
        REQUIRE( soa.empty() );
        REQUIRE( soa.array<0>().empty() );
        REQUIRE( soa.array<1>().empty() );
        REQUIRE( soa.array<2>().empty() );

        const auto & reference = soa.emplace_back( 1, 2.0f, true );

        REQUIRE( soa.size() == 1 );
        REQUIRE( soa.array<0>().size() == 1 );
        REQUIRE( soa.array<1>().size() == 1 );
        REQUIRE( soa.array<2>().size() == 1 );
        REQUIRE( std::get<0>( reference ) == 1 );
        REQUIRE( std::get<1>( reference ) == 2.f );
        REQUIRE( std::get<2>( reference ) == true );
    }
}
