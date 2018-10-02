#ifndef SOA_H
#define SOA_H

#include <array>
#include <tuple>
#include <vector>

namespace soacpp
{
    template <template <class...> class Container, typename... Attrs>
    class soa_container_traits;

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

        template <template <class...> class Container, typename... Attrs>
        class soa_impl
        {
            using traits = soa_container_traits<Container, Attrs...>;

        public:
            using size_type = std::size_t;
            using reference = typename traits::reference;
            using const_reference = typename traits::const_reference;
            using iterator = typename traits::iterator;
            using arrays = typename traits::arrays;

            soa_impl() noexcept = default;

            iterator begin() noexcept
            {
                return detail::index_apply<attribute_count>(
                    [this]( auto... Is ) { return iterator{std::begin( std::get<Is>( data ) )...}; } );
            }

            iterator end() noexcept
            {
                return detail::index_apply<attribute_count>(
                    [this]( auto... Is ) { return iterator{std::end( std::get<Is>( data ) )...}; } );
            }

            reference operator[]( size_type idx )
            {
                return detail::index_apply<attribute_count>(
                    [this, idx]( auto... Is ) { return reference{std::get<Is>( data )[idx]...}; } );
            }

            const_reference operator[]( size_type idx ) const
            {
                return detail::index_apply<attribute_count>(
                    [this, idx]( auto... Is ) { return const_reference{std::get<Is>( data )[idx]...}; } );
            }

            template <std::size_t I>
            typename std::tuple_element<I, arrays>::type & array()
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

            static constexpr std::size_t attribute_count = traits::attribute_count;
        };
    } // namespace detail

    template <typename T>
    class container_traits
    {

    public:
        using size_type = typename T::size_type;
        using reference = typename T::reference;
        using const_reference = typename T::const_reference;

        using iterator = typename T::iterator;
        // :TODO: const, reverse, const_reverse

        using reserve_t = void ( T::* )( size_type capacity );
        static constexpr reserve_t reserve = &T::reserve;
    };

    template <typename T, std::size_t count>
    class container_traits<std::array<T, count>>
    {

        using array = std::array<T, count>;

    public:
        using size_type = typename array::size_type;
        using reference = typename array::reference;
        using const_reference = typename array::const_reference;

        using iterator = typename array::iterator;
        // :TODO: const, reverse, const_reverse
    };

    template <template <class...> class Container, typename... Attrs>
    class soa_iterator;

    template <template <class...> class Container, typename... Attrs>
    class soa_container_traits
    {
        template <typename T>
        using traits = container_traits<Container<T>>;

    public:
        using reference = std::tuple<typename traits<Attrs>::reference...>;
        using const_reference = std::tuple<typename traits<Attrs>::const_reference...>;

        using iterator_base = std::tuple<typename traits<Attrs>::iterator...>;
        using iterator = soa_iterator<Container, Attrs...>;

        using arrays = std::tuple<Container<std::decay_t<Attrs>>...>;
        static constexpr std::size_t attribute_count = std::tuple_size<arrays>{};
    };

    // :TODO: const_iterator, reverse_iterator, const_reverse_iterator

    template <template <class...> class Container, typename... Attrs>
    class soa_iterator : public soa_container_traits<Container, Attrs...>::iterator_base
    {
        using traits = soa_container_traits<Container, Attrs...>;
        using base = typename traits::iterator_base;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using reference = typename traits::reference;

        soa_iterator() = delete;

        template <class... UTypes>
        explicit soa_iterator( UTypes &&... args )
            : base( args... )
        {
        }

        soa_iterator & operator++()
        {
            detail::index_apply<traits::attribute_count>(
                [this]( auto... Is ) { detail::call( ( std::get<Is>( *this )++, true )... ); } );

            return *this;
        }

        const soa_iterator operator++( int )
        {
            soa_iterator retval = *this;
            ++( *this );

            return retval;
        }

        bool operator==( soa_iterator other ) const
        {
            // :TODO: assert all other iterators in tuples do match

            return std::get<0>( *this ) == std::get<0>( other );
        }

        bool operator!=( soa_iterator other ) const
        {
            return !( *this == other );
        }

        reference operator*() const
        {
            return detail::index_apply<traits::attribute_count>(
                [this]( auto... Is ) { return reference{( *std::get<Is>( *this ) )...}; } );
        }
    };

    template <typename... Attrs>
    class soa_vector : public detail::soa_impl<std::vector, Attrs...>
    {
        using base = detail::soa_impl<std::vector, Attrs...>;

    public:
        using typename base::size_type;

        soa_vector() = default;

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
            detail::index_apply<base::attribute_count>( [this, count]( auto... Is ) {
                auto container_reserve = []( auto && container, size_type cnt ) {
                    ( container.*container_traits<std::decay_t<decltype( container )>>::reserve )( cnt );
                };

                detail::call( ( container_reserve( std::get<Is>( this->data ), count ), true )... );
            } );
        }

        void resize( size_type count )
        {
            detail::index_apply<base::attribute_count>( [this, count]( auto... Is ) {
                detail::call( ( std::get<Is>( this->data ).resize( count ), true )... );
            } );
        }
    };

    namespace detail
    {
        template <std::size_t count>
        struct std_array_helper
        {
            template <typename T>
            using type = std::array<T, count>;
        };
    } // namespace detail

    template <std::size_t count, typename... Attrs>
    using soa_array = detail::soa_impl<detail::std_array_helper<count>::template type, Attrs...>;
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

#define TRANSFORM( NAME_, ARGS_, EXTRA_ ) GLUE( TRANSFORM_, VARCOUNT ARGS_ )( NAME_, ARGS_, EXTRA_ )
#define TRANSFORM_1( NAME_, ARGS_, EXTRA_ ) GLUE( NAME_, _LAST )( FIRST ARGS_, EXTRA_ )
#define TRANSFORM_2( NAME_, ARGS_, EXTRA_ ) NAME_( FIRST ARGS_, EXTRA_ ) TRANSFORM_1( NAME_, TUPLE_TAIL ARGS_, EXTRA_ )
#define TRANSFORM_3( NAME_, ARGS_, EXTRA_ ) NAME_( FIRST ARGS_, EXTRA_ ) TRANSFORM_2( NAME_, TUPLE_TAIL ARGS_, EXTRA_ )
#define TRANSFORM_4( NAME_, ARGS_, EXTRA_ ) NAME_( FIRST ARGS_, EXTRA_ ) TRANSFORM_3( NAME_, TUPLE_TAIL ARGS_, EXTRA_ )
#define TRANSFORM_5( NAME_, ARGS_, EXTRA_ ) NAME_( FIRST ARGS_, EXTRA_ ) TRANSFORM_4( NAME_, TUPLE_TAIL ARGS_, EXTRA_ )
#define TRANSFORM_6( NAME_, ARGS_, EXTRA_ ) NAME_( FIRST ARGS_, EXTRA_ ) TRANSFORM_5( NAME_, TUPLE_TAIL ARGS_, EXTRA_ )
#define TRANSFORM_7( NAME_, ARGS_, EXTRA_ ) NAME_( FIRST ARGS_, EXTRA_ ) TRANSFORM_6( NAME_, TUPLE_TAIL ARGS_, EXTRA_ )
#define TRANSFORM_8( NAME_, ARGS_, EXTRA_ ) NAME_( FIRST ARGS_, EXTRA_ ) TRANSFORM_7( NAME_, TUPLE_TAIL ARGS_, EXTRA_ )
#define TRANSFORM_9( NAME_, ARGS_, EXTRA_ ) NAME_( FIRST ARGS_, EXTRA_ ) TRANSFORM_8( NAME_, TUPLE_TAIL ARGS_, EXTRA_ )

#define GET_ELEMENT_TYPE_LAST( ATTRIBUTE_, CLASS_ ) decltype( CLASS_::ATTRIBUTE_ )
#define GET_ELEMENT_TYPE( ATTRIBUTE_, CLASS_ ) EVAL( GET_ELEMENT_TYPE_LAST( ATTRIBUTE_, CLASS_ ) ),
#define GET_COMMA_SEPARATED_ELEMENT_TYPES( CLASS_, ... ) TRANSFORM( GET_ELEMENT_TYPE, ( __VA_ARGS__ ), CLASS_ )

#define GET_TUPLE_ELEMENT_LAST( ATTRIBUTE_, UNUSED_ ) Container<decltype( adapted_type::ATTRIBUTE_ )>
#define GET_TUPLE_ELEMENT( ATTRIBUTE_, UNUSED_ ) EVAL( GET_TUPLE_ELEMENT_LAST( ATTRIBUTE_, UNUSED_ ) ),
#define MAKE_TUPLE_TYPE_LIST( ... ) < __VA_ARGS__ >
#define GET_TUPLE_TYPE( ... )                                                                                          \
    std::tuple EVAL( MAKE_TUPLE_TYPE_LIST( TRANSFORM( GET_TUPLE_ELEMENT, ( __VA_ARGS__ ), 0 ) ) )

#define DECLARE_ATTRIBUTE_LAST( ATTRIBUTE_, TYPE_ )                                                                    \
    typename container_traits<Container<decltype( adapted_type::ATTRIBUTE_ )>>::TYPE_ ATTRIBUTE_
#define DECLARE_ATTRIBUTE( ATTRIBUTE_, TYPE_ ) EVAL( DECLARE_ATTRIBUTE_LAST( ATTRIBUTE_, TYPE_ ) );
#define DECLARE_ATTRIBUTES( TYPE_, ... ) TRANSFORM( DECLARE_ATTRIBUTE, ( __VA_ARGS__ ), TYPE_ )

#define DECLARE_SOA_TYPE( CLASS_, ... )                                                                                \
    template <template <class...> class Container>                                                                     \
    class soacpp::soa_container_traits<Container, CLASS_>                                                              \
        : public soacpp::soa_container_traits<Container, GET_COMMA_SEPARATED_ELEMENT_TYPES( CLASS_, __VA_ARGS__ )>     \
    {                                                                                                                  \
        using adapted_type = CLASS_;                                                                                   \
                                                                                                                       \
    public:                                                                                                            \
        struct reference                                                                                               \
        {                                                                                                              \
            DECLARE_ATTRIBUTES( reference, __VA_ARGS__ );                                                              \
        };                                                                                                             \
        struct const_reference                                                                                         \
        {                                                                                                              \
            DECLARE_ATTRIBUTES( const_reference, __VA_ARGS__ );                                                        \
        };                                                                                                             \
                                                                                                                       \
        using iterator = soa_iterator<Container, CLASS_>;                                                              \
                                                                                                                       \
        using arrays = GET_TUPLE_TYPE( __VA_ARGS__ );                                                                  \
        static constexpr std::size_t attribute_count = std::tuple_size<arrays>{};                                      \
    };

#endif
