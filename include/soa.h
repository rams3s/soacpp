#ifndef SOA_H
#define SOA_H

#include <array>

#define WITH_STL 1

#if WITH_STL

#include <vector>

#endif

namespace soacpp
{
    namespace detail
    {
        template <class... T>
        void call( T &&... /*unused*/ )
        {
        }

        template <class F, std::size_t... Is>
        constexpr auto index_apply_impl( F f, std::index_sequence<Is...> /*unused*/ )
        {
            return f( std::integral_constant<size_t, Is>{}... );
        }

        template <std::size_t N, class F>
        constexpr auto index_apply( F f )
        {
            return index_apply_impl( f, std::make_index_sequence<N>{} );
        }
    } // namespace detail

    template <template <class...> class Container, typename... Attrs>
    class soa_helper
    {

    public:
        using reference = std::tuple<typename Container<Attrs>::reference...>;
        using const_reference = std::tuple<typename Container<Attrs>::const_reference...>;
        using arrays = std::tuple<Container<Attrs>...>;
    };

    template <template <class...> class Container, typename... Attrs>
    class soa
    {

    public:
        using helper = soa_helper<Container, Attrs...>;
        using size_type = std::size_t;
        using reference = typename helper::reference;
        using const_reference = typename helper::const_reference;
        using arrays = typename helper::arrays;

        soa() noexcept = default;

        reference operator[]( size_type idx )
        {
            return detail::index_apply<attribute_count>(
                    [this, idx]( auto... Is ) -> reference
                        { return {std::get<Is>( data )[idx]...}; } );
        }

        const_reference operator[]( size_type idx ) const
        {
            return detail::index_apply<attribute_count>(
                    [this, idx]( auto... Is ) -> reference
                        { return {std::get<Is>( data )[idx]...}; } );
        }

        template <std::size_t I>
        typename std::tuple_element<I, arrays>::type &array()
        {
            return std::get<I>( data );
        }

        bool empty() const noexcept
        {
            return std::get<0>( data ).empty();
        }

        size_type size() const noexcept
        {
            return std::get<0>( data ).size();
        }

    protected:
        arrays data;

        static constexpr std::size_t attribute_count = std::tuple_size<arrays>{};
    };

#if WITH_STL

    namespace detail
    {
        template <std::size_t count>
        struct std_array_helper
        {
            template <typename T>
            using type = std::array<T, count>;
        };
    } // namespace detail

    template <typename... Attrs>
    class soa_vector : public soa<std::vector, Attrs...>
    {

    public:
        using typename soa<std::vector, Attrs...>::size_type;
        using soa<std::vector, Attrs...>::attribute_count;

        soa_vector() noexcept = default;

        explicit soa_vector( size_type count )
        {
            resize( count );
        }

        size_type capacity() const noexcept
        {
            return std::get<0>( this->data ).capacity();
        }

        void reserve( size_type count )
        {
            detail::index_apply<attribute_count>(
                    [this, count]( auto... Is )
                        { detail::call(( std::get<Is>( this->data ).reserve( count ), true )... ); } );
        }

        void resize( size_type count )
        {
            detail::index_apply<attribute_count>(
                    [this, count]( auto... Is )
                        { detail::call(( std::get<Is>( this->data ).resize( count ), true )... ); } );
        }
    };

    template <std::size_t count, typename... Attrs>
    using soa_array = soa<detail::std_array_helper<count>::template type, Attrs...>;
#endif
} // namespace soacpp

// helper macros
// inspired from: https://stackoverflow.com/a/44479664/529915

#define EVAL( ... ) __VA_ARGS__
#define VARCOUNT( ... ) EVAL( VARCOUNT_I( __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, ) )
#define VARCOUNT_I( _, _9, _8, _7, _6, _5, _4, _3, _2, X_, ... ) X_
#define GLUE( X, Y ) GLUE_I( X, Y )
#define GLUE_I( X, Y ) X##Y
#define FIRST( ... ) EVAL( FIRST_I( __VA_ARGS__, ) )
#define FIRST_I( X, ... ) X
#define TUPLE_TAIL( ... ) EVAL( TUPLE_TAIL_I( __VA_ARGS__ ) )
#define TUPLE_TAIL_I( X, ... ) ( __VA_ARGS__ )

#define TRANSFORM( NAME_, ARGS_ ) GLUE( TRANSFORM_, VARCOUNT ARGS_ )( NAME_, ARGS_ )
#define TRANSFORM_1( NAME_, ARGS_ ) NAME_ ARGS_
#define TRANSFORM_2( NAME_, ARGS_ ) NAME_( FIRST ARGS_ ), TRANSFORM_1( NAME_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_3( NAME_, ARGS_ ) NAME_( FIRST ARGS_ ), TRANSFORM_2( NAME_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_4( NAME_, ARGS_ ) NAME_( FIRST ARGS_ ), TRANSFORM_3( NAME_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_5( NAME_, ARGS_ ) NAME_( FIRST ARGS_ ), TRANSFORM_4( NAME_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_6( NAME_, ARGS_ ) NAME_( FIRST ARGS_ ), TRANSFORM_5( NAME_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_7( NAME_, ARGS_ ) NAME_( FIRST ARGS_ ), TRANSFORM_6( NAME_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_8( NAME_, ARGS_ ) NAME_( FIRST ARGS_ ), TRANSFORM_7( NAME_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_9( NAME_, ARGS_ ) NAME_( FIRST ARGS_ ), TRANSFORM_8( NAME_, TUPLE_TAIL ARGS_ )

#define TRANSFORM_SEP( NAME_, SEP_, ARGS_ ) GLUE( TRANSFORM_SEP_, VARCOUNT ARGS_ )( NAME_, SEP_, ARGS_ )
#define TRANSFORM_SEP_1( NAME_, SEP_, ARGS_ ) NAME_ ARGS_
#define TRANSFORM_SEP_2( NAME_, SEP_, ARGS_ ) NAME_( FIRST ARGS_ ) SEP_ TRANSFORM_SEP_1( NAME_, SEP_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_SEP_3( NAME_, SEP_, ARGS_ ) NAME_( FIRST ARGS_ ) SEP_ TRANSFORM_SEP_2( NAME_, SEP_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_SEP_4( NAME_, SEP_, ARGS_ ) NAME_( FIRST ARGS_ ) SEP_ TRANSFORM_SEP_3( NAME_, SEP_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_SEP_5( NAME_, SEP_, ARGS_ ) NAME_( FIRST ARGS_ ) SEP_ TRANSFORM_SEP_4( NAME_, SEP_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_SEP_6( NAME_, SEP_, ARGS_ ) NAME_( FIRST ARGS_ ) SEP_ TRANSFORM_SEP_5( NAME_, SEP_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_SEP_7( NAME_, SEP_, ARGS_ ) NAME_( FIRST ARGS_ ) SEP_ TRANSFORM_SEP_6( NAME_, SEP_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_SEP_8( NAME_, SEP_, ARGS_ ) NAME_( FIRST ARGS_ ) SEP_ TRANSFORM_SEP_7( NAME_, SEP_, TUPLE_TAIL ARGS_ )
#define TRANSFORM_SEP_9( NAME_, SEP_, ARGS_ ) NAME_( FIRST ARGS_ ) SEP_ TRANSFORM_SEP_8( NAME_, SEP_, TUPLE_TAIL ARGS_ )

#define GET_CONTAINER_TYPE( ATTRIBUTE_ ) Container<decltype( adapted_type::ATTRIBUTE_ )>

#define MAKE_TUPLE_TYPE_LIST( ... ) < __VA_ARGS__ >

#define GET_TUPLE_TYPE( ... )                                                                                          \
    std::tuple EVAL( MAKE_TUPLE_TYPE_LIST( TRANSFORM( GET_CONTAINER_TYPE, ( __VA_ARGS__ ) ) ) )

#define DECLARE_REFERENCE_ATTRIBUTE( ATTRIBUTE_ ) typename GET_CONTAINER_TYPE( ATTRIBUTE_ )::reference ATTRIBUTE_
#define DECLARE_CONST_REFERENCE_ATTRIBUTE( ATTRIBUTE_ ) typename GET_CONTAINER_TYPE( ATTRIBUTE_ )::const_reference ATTRIBUTE_

#define DECLARE_REFERENCE_ATTRIBUTES( ... ) TRANSFORM_SEP( DECLARE_REFERENCE_ATTRIBUTE, ;, ( __VA_ARGS__ ) )
#define DECLARE_CONST_REFERENCE_ATTRIBUTES( ... ) TRANSFORM_SEP( DECLARE_CONST_REFERENCE_ATTRIBUTE, ;, ( __VA_ARGS__ ) )

#define DECLARE_SOA_TYPE( CLASS_, ... )                                                                                \
    template <template <class...> class Container>                                                                     \
    class soacpp::soa_helper<Container, CLASS_>                                                                        \
    {                                                                                                                  \
        using adapted_type = CLASS_;                                                                                   \
                                                                                                                       \
    public:                                                                                                            \
        struct reference                                                                                               \
        {                                                                                                              \
            DECLARE_REFERENCE_ATTRIBUTES( __VA_ARGS__ );                                                               \
        };                                                                                                             \
                                                                                                                       \
        struct const_reference                                                                                         \
        {                                                                                                              \
            DECLARE_CONST_REFERENCE_ATTRIBUTES( __VA_ARGS__ );                                                         \
        };                                                                                                             \
                                                                                                                       \
        using arrays = GET_TUPLE_TYPE( __VA_ARGS__ );                                                                  \
    };

#endif
