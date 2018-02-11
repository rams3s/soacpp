#include "catch.hpp"

#include "soa.h"

TEST_CASE( "struct-of-arrays are properly constructed", "[soa]" )
{
    soacpp::soa_vector<uint8_t, float, double, bool> soa;
    soacpp::soa_vector<uint8_t, float, double, bool> soa_with_size( 16 );

    static_assert( std::tuple_size< decltype( soa )::arrays >{} == 4, "soa internal tuple should contain 4 containers" );
    static_assert( std::is_same< uint8_t, std::tuple_element< 0, decltype( soa )::arrays >::type::value_type >{}, "internal container at index 0 should be a uint8_t container" );
    static_assert( std::is_same< float, std::tuple_element< 1, decltype( soa )::arrays >::type::value_type >{}, "internal container at index 1 should be a float container" );
    static_assert( std::is_same< double, std::tuple_element< 2, decltype( soa )::arrays >::type::value_type >{}, "internal container at index 2 should be a double container" );
    static_assert( std::is_same< bool, std::tuple_element< 3, decltype( soa )::arrays >::type::value_type >{}, "internal container at index 3 should be a bool container" );

    SECTION( "every sub vector contains the right item count" )
    {
        REQUIRE( soa.getContainer<0>().size() == 0 );
        REQUIRE( soa.getContainer<1>().size() == 0 );
        REQUIRE( soa.getContainer<2>().size() == 0 );
        REQUIRE( soa.getContainer<3>().size() == 0 );

        REQUIRE( soa_with_size.getContainer<0>().size() == 16 );
        REQUIRE( soa_with_size.getContainer<1>().size() == 16 );
        REQUIRE( soa_with_size.getContainer<2>().size() == 16 );
        REQUIRE( soa_with_size.getContainer<3>().size() == 16 );
    }
}

TEST_CASE( "struct-of-arrays are properly resized", "[soa]" )
{
    soacpp::soa_vector<uint8_t, float, double, bool> soa;

    SECTION( "every sub vector is properly resized" )
    {
        REQUIRE( soa.getContainer<0>().size() == 0 );
        REQUIRE( soa.getContainer<1>().size() == 0 );
        REQUIRE( soa.getContainer<2>().size() == 0 );
        REQUIRE( soa.getContainer<3>().size() == 0 );

        soa.resize( 32 );

        REQUIRE( soa.getContainer<0>().size() == 32 );
        REQUIRE( soa.getContainer<1>().size() == 32 );
        REQUIRE( soa.getContainer<2>().size() == 32 );
        REQUIRE( soa.getContainer<3>().size() == 32 );
    }
}

