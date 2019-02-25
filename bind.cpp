#include <tuple>
#include <iostream>
#include <functional>


template <typename T>
struct G {
	G() = delete;
    G(T value) : value(std::move(value)) {}

    template <typename ...Args>
    T operator()(Args const &...as) {
        return value;
    }

    T value;
};



template <int N>
struct G<std::_Placeholder<N>> {
	G() {}
	G(std::_Placeholder<N>) {}

    template <typename A, typename ...As>
    auto const& operator()(A const & a1, As const &...as) {
        return G<std::_Placeholder<N - 1>>()(as...);
    }
};

template <>
struct G<std::_Placeholder<1>> {

	G() {}
	G(std::_Placeholder<1> value) : value(std::move(value)) {}

    template <typename A, typename ...As>
    A const& operator()(A const &a1, As const &...as) {
        return a1;
    }

    std::_Placeholder<1> value;
};


template <size_t ...Indices>
struct index_tuple {};

template <size_t N, size_t ...Indices>
struct make_index_tuple {
    using type = typename make_index_tuple<N-1, N-1, Indices...>::type;
};

template <size_t ...Indices>
struct make_index_tuple<0, Indices...> {
    using type = index_tuple<Indices...>;
};



template <typename F, typename ...Bs>
struct bind_t {
    bind_t(F f, Bs ...bs)
        : f(std::move(f)),  gs(std::move(bs)...) {}

    F f;
    std::tuple<G<Bs>...> gs;

    template <typename ...As>
    auto operator()(As const &...as) {
        return call(typename make_index_tuple<sizeof...(Bs)>::type(), as...);
    }

    template <typename ...As, size_t ...Indices>
    auto call(index_tuple<Indices...>, As const &...as) {
        return f(std::get<Indices>(gs)(as...)...);
    }
};

namespace kek {
template <typename F, typename ...Bs>
bind_t<F, Bs...> bind(F f, Bs ...bs) {
    return bind_t<F, Bs...>(std::move(f), std::move(bs)...);
}
}

void f(int a, std::string const& s) {
	std::cout << a;
}
using namespace std::placeholders;

int main() {
    //kek::bind(f, 4, 5, 6)();
    std::string s;
    kek::bind(f, _1, _2)(6, s);	
}