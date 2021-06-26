#pragma once

template<int ...>
struct Sequence {};

/**
 * Used to generate a compile-time type 'Sequence<0, 1, 2, ..., N>'. This can e.g. be used to get back a parameter
 * back that was stored in a tuple. Let's go through the steps the compiler takes to evaluate
 *     typename IndexGenerator<sizeof...(T)>::type()
 * Firstly, let's assume sizeof...(T) evaluates to 4. Then IndexGenerator<3> expands to
 *     IndexGenerator<3, 3>           (N = 3, S = 3)
 * by means of inheritance. This expression again evaluates to
 *     IndexGenerator<2, 2, 3>        (N = 2, S... = 2, 3)
 * which evaluates to
 *     IndexGenerator<1, 1, 2, 3>     (N = 1, S... = 1, 2, 3)
 * which evaluates to
 *     IndexGenerator<0, 0, 1, 2, 3>  (N = 0, S... = 0, 1, 2, 3)
 * Now the specialized case of IndexGenerator<0, S...> is present and type gets defined as a type alias for
 *     Sequence<0, 1, 2, 3>
 *
 * This could for example be use in a templated function
 *     template<int ...sequence>
 *     void call_function(Sequence<sequence...>) {
 *         function(std::get<sequence>(savedArgs)...);
 *     }
 * where savedArgs is a std::tuple that was made with std::make_tuple(std::move(T)...)
 *
 * NOTE: The same can be achieved since c++14 with std::make_index_sequence<sizeof...(T)>{}
 *
 * @tparam N
 * @tparam S
 */
template<int N, int ...S>
struct IndexGenerator : IndexGenerator<N - 1, N - 1, S...> {};

template<int ...S>
struct IndexGenerator<0, S...> {
    using type = Sequence<S...>;
};
