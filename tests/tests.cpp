#include "catch.hpp"

#include "soa.h"

TEST_CASE( "constructors", "[soa]" )
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
    soacpp::soa_vector<uint8_t, float, bool> soa( 4 );

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

    SECTION( "operator[] const" )
    {
        const auto & const_soa = soa;

        REQUIRE( std::get<0>( const_soa[0] ) == 0 );
        REQUIRE( std::get<0>( const_soa[1] ) == 1 );
        REQUIRE( std::get<0>( const_soa[2] ) == 2 );
        REQUIRE( std::get<0>( const_soa[3] ) == 3 );

        REQUIRE( std::get<1>( const_soa[0] ) == 0.0f );
        REQUIRE( std::get<1>( const_soa[1] ) == 2.0f );
        REQUIRE( std::get<1>( const_soa[2] ) == 4.0f );
        REQUIRE( std::get<1>( const_soa[3] ) == 6.0f );

        REQUIRE( std::get<2>( const_soa[0] ) == false );
        REQUIRE( std::get<2>( const_soa[1] ) == true );
        REQUIRE( std::get<2>( const_soa[2] ) == false );
        REQUIRE( std::get<2>( const_soa[3] ) == true );
    }
}

TEST_CASE( "Capacity", "[soa]" )
{
    soacpp::soa_vector<uint8_t, float, bool> soa;

    SECTION( "empty" )
    {
        soacpp::soa_vector<uint8_t, float, bool> soa_with_size( 4 );

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

TEST_CASE( "iterators", "[soa]" )
{
    soacpp::soa_vector<uint8_t, float, bool> soa( 4 );

    SECTION( "begin" )
    {
        auto soa_it = soa.begin();

        REQUIRE( std::get<0>( soa_it ) == soa.array<0>().begin() );
        REQUIRE( std::get<1>( soa_it ) == soa.array<1>().begin() );
        REQUIRE( std::get<2>( soa_it ) == soa.array<2>().begin() );
    }

    SECTION( "end" )
    {
        auto soa_it = soa.end();

        REQUIRE( std::get<0>( soa_it ) == soa.array<0>().end() );
        REQUIRE( std::get<1>( soa_it ) == soa.array<1>().end() );
        REQUIRE( std::get<2>( soa_it ) == soa.array<2>().end() );
    }
}

TEST_CASE( "soa_iterator", "[soa]" )
{
    soacpp::soa_vector<uint8_t, float, bool> soa( 4 );

    SECTION( "operator++" )
    {
        auto soa_it = soa.begin();
        auto it_0 = soa.array<0>().begin();
        auto it_1 = soa.array<1>().begin();
        auto it_2 = soa.array<2>().begin();

        soa_it++;
        it_0++;
        it_1++;
        it_2++;

        REQUIRE( std::get<0>( soa_it ) == it_0 );
        REQUIRE( std::get<1>( soa_it ) == it_1 );
        REQUIRE( std::get<2>( soa_it ) == it_2 );

        ++soa_it;
        ++it_0;
        ++it_1;
        ++it_2;

        REQUIRE( std::get<0>( soa_it ) == it_0 );
        REQUIRE( std::get<1>( soa_it ) == it_1 );
        REQUIRE( std::get<2>( soa_it ) == it_2 );

        soa_it++;
        ++soa_it;

        REQUIRE( std::get<0>( soa_it ) == soa.array<0>().end() );
        REQUIRE( std::get<1>( soa_it ) == soa.array<1>().end() );
        REQUIRE( std::get<2>( soa_it ) == soa.array<2>().end() );
    }

    SECTION( "operator==/!=" )
    {
        auto soa_it = soa.begin();

        REQUIRE( soa_it == soa.begin() );
        REQUIRE( soa_it != soa.end() );

        ++soa_it;

        REQUIRE( soa_it != soa.begin() );
        REQUIRE( soa_it != soa.end() );

        ++soa_it;
        ++soa_it;
        ++soa_it;

        REQUIRE( soa_it != soa.begin() );
        REQUIRE( soa_it == soa.end() );
    }

    SECTION( "operator*" )
    {
        // :TODO:
    }
}

// :TODO: other types of iterator
