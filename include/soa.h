#ifndef SOA_H
#define SOA_H

#include <tuple>
#include <vector>

namespace soacpp
{
    template <typename... Attrs>
    class soa_traits;

    namespace detail
    {
        template <typename... Attrs>
        class soa_impl
        {
            using traits = soa_traits<Attrs...>;

        public:
            using size_type = std::size_t;
            using reference = typename traits::reference;
            using data_t = typename traits::data_t;

            soa_impl() noexcept = default;

            reference operator[]( size_type idx )
            {
                return std::apply( [idx]( auto &&... x ) { return reference{x[idx]...}; }, data );
            }

            template <std::size_t I>
            typename std::tuple_element<I, data_t>::type & array()
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
            data_t data;

        };
    } // namespace detail

    template <typename... Attrs>
    class soa_traits
    {
    public:
        using reference = std::tuple<typename std::vector<Attrs>::reference...>;

        using data_t = std::tuple<std::vector<std::decay_t<Attrs>>...>;
    };

    template <typename... Attrs>
    class vector : public detail::soa_impl<Attrs...>
    {
        using base = detail::soa_impl<Attrs...>;

    public:
        using typename base::reference;
        using typename base::size_type;

        vector() noexcept = default;

        explicit vector( size_type count )
        {
            resize( count );
        }

        size_type capacity() const noexcept
        {
            return std::get<0>( this->data ).capacity();
        }

        void reserve( size_type count )
        {
            std::apply( [count]( auto &&... x ) { std::make_tuple( ( x.reserve( count ), 0 )... ); }, this->data );
        }

        void resize( size_type count )
        {
            std::apply( [count]( auto &&... x ) { std::make_tuple( ( x.resize( count ), 0 )... ); }, this->data );
        }

        template <class... Args>
        reference emplace_back( Args &&... args )
        {
            auto zip = []( typename base::data_t & tuple_t, Args &&... us ) {
                return std::apply(
                    [&us...]( auto &... ts ) { return std::make_tuple( std::make_pair( &ts, us )... ); }, tuple_t );
            };

            return std::apply(
                []( auto &&... x ) {
                    std::make_tuple( ( std::get<0>( x )->emplace_back( std::get<1>( x ) ), 0 )... );
                    return reference{std::get<0>( x )->back()...};
                },
                zip( this->data, std::forward<Args>( args )... ) );
        }
    };
} // namespace soacpp

#endif
