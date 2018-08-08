#ifndef SOA_H
#define SOA_H

#include <array>
#include <vector>

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

    template <template <class...> class Container, typename T>
    class container_traits
    {

    public:
        using reference = typename Container<T>::reference;
        using const_reference = typename Container<T>::const_reference;
        using pointer = typename Container<T>::pointer;
        using const_pointer = typename Container<T>::const_pointer;
        using difference_type = typename Container<T>::difference_type;
        using value_type = typename Container<T>::value_type;
    };

    template <template <class...> class Container, typename... Attrs>
    class soa_container_traits
    {
        template <typename T>
        using traits = container_traits<Container, T>;

    public:
        using reference = std::tuple<typename traits<Attrs>::reference...>;
        using const_reference = std::tuple<typename traits<Attrs>::const_reference...>;
        using pointer = std::tuple<typename traits<Attrs>::pointer...>;
        using const_pointer = std::tuple<typename traits<Attrs>::const_pointer...>;
        using difference_type = std::tuple<typename traits<Attrs>::difference_type...>;
        using value_type = std::tuple<typename traits<Attrs>::value_type...>;

        using arrays = std::tuple<Container<Attrs>...>;
        static constexpr std::size_t attribute_count = std::tuple_size<arrays>{};
    };

    // :TODO: const_iterator, reverse_iterator, const_reverse_iterator

    template <template <class...> class Container, typename... Attrs>
    class soa_iterator : public std::tuple<typename Container<Attrs>::iterator...>
    {
        using traits = soa_container_traits<Container, Attrs...>;
        using base = std::tuple<typename Container<Attrs>::iterator...>;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename traits::value_type;
        using difference_type = typename traits::difference_type;
        using pointer = typename traits::pointer;
        using reference = typename traits::reference;

        soa_iterator() = delete;

        template< class... UTypes >
        explicit soa_iterator( UTypes&&... args ) : base (args...)
        {
        }

        soa_iterator & operator++()
        {
            detail::index_apply<traits::attribute_count>(
                    [this]( auto... Is )
                        { detail::call(( std::get<Is>( *this )++, true )... ); } );

            return *this;
        }

        const soa_iterator operator++(int)
        {
            soa_iterator retval = *this;
            ++(*this);

            return retval;
        }

        bool operator==(soa_iterator other) const
        {
            // :TODO: assert all other iterators in tuples do match

            return std::get<0>( *this ) == std::get<0>( other );
        }

        bool operator!=(soa_iterator other) const
        {
            return !(*this == other);
        }

        reference operator*() const {
            return detail::index_apply<traits::attribute_count>(
                    [this]( auto... Is ) -> reference
                        { return { ( *std::get<Is>( *this ) )...}; } );
        }
    };

    template <template <class...> class Container, typename... Attrs>
    class soa
    {
        using traits = soa_container_traits<Container, Attrs...>;

    public:
        using size_type = std::size_t;
        using reference = typename traits::reference;
        using const_reference = typename traits::const_reference;
        using iterator = soa_iterator<Container, Attrs...>;
        using arrays = typename traits::arrays;

        soa() noexcept = default;

        iterator begin() noexcept
        {
            return detail::index_apply<attribute_count>(
                    [this]( auto... Is ) -> iterator
                        { return iterator {std::begin( std::get<Is>( data ) )...}; } );
        }

        iterator end() noexcept
        {
            return detail::index_apply<attribute_count>(
                    [this]( auto... Is ) -> iterator
                        { return iterator {std::end( std::get<Is>( data ) )...}; } );
        }

        reference operator[]( size_type idx )
        {
            return detail::index_apply<attribute_count>(
                    [this, idx]( auto... Is ) -> reference
                        { return reference {std::get<Is>( data )[idx]...}; } );
        }

        const_reference operator[]( size_type idx ) const
        {
            return detail::index_apply<attribute_count>(
                    [this, idx]( auto... Is ) -> const_reference
                        { return const_reference {std::get<Is>( data )[idx]...}; } );
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

        static constexpr std::size_t attribute_count = traits::attribute_count;
    };

    template <typename... Attrs>
    class soa_vector : private soa<std::vector, Attrs...>
    {
        using base = soa<std::vector, Attrs...>;
        using base::attribute_count;

    public:
        using typename base::size_type;
        using typename base::reference;
        using typename base::const_reference;
        using typename base::iterator;
        using typename base::arrays;

        soa_vector() noexcept = default;

        explicit soa_vector( size_type count )
        {
            resize( count );
        }

        using base::operator[];
        using base::begin;
        using base::end;
        using base::array;
        using base::empty;
        using base::size;

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
    using soa_array = soa<detail::std_array_helper<count>::template type, Attrs...>;
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
    class soacpp::soa_container_traits<Container, CLASS_>                                                              \
    {                                                                                                                  \
        using adapted_type = CLASS_;                                                                                   \
                                                                                                                       \
    public:                                                                                                            \
        struct reference { DECLARE_REFERENCE_ATTRIBUTES( __VA_ARGS__ ); };                                             \
        struct const_reference { DECLARE_CONST_REFERENCE_ATTRIBUTES( __VA_ARGS__ ); };                                 \
                                                                                                                       \
        using arrays = GET_TUPLE_TYPE( __VA_ARGS__ );                                                                  \
        static constexpr std::size_t attribute_count = std::tuple_size<arrays>{};                                      \
    };

#endif
