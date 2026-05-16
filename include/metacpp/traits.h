#ifndef _GEF_GUARD_DEFINE_METACPP_TRAITS_H_DEF_GUARD_
#define _GEF_GUARD_DEFINE_METACPP_TRAITS_H_DEF_GUARD_

#include <functional>
#include <set>
#include <type_traits>


#ifndef _WIN32
#include <experimental/type_traits>
#endif

#ifdef _WIN32

namespace std {
namespace experimental {
namespace detail {

	template <class Default, class AlwaysVoid,
		template<class...> class Op, class... Args>
	struct detector {
		using value_t = std::false_type;
		using type = Default;
	};

	template <class Default, template<class...> class Op, class... Args>
	struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
		// Note that std::void_t is a C++17 feature
		using value_t = std::true_type;
		using type = Op<Args...>;
	};

} // end namespace detail

	struct nonesuch
	{
		nonesuch() = delete;
		~nonesuch() = delete;
		nonesuch(nonesuch const&) = delete;
		void operator=(nonesuch const&) = delete;
	};

	template <template<class...> class Op, class... Args>
	using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

	template <template<class...> class Op, class... Args>
	using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

	template <class Default, template<class...> class Op, class... Args>
	using detected_or = detail::detector<Default, void, Op, Args...>;

}} // end namespaces

#endif



namespace meta {
namespace impl {

	template<class T>
	using has_meta_t = decltype(T::meta());

	template<class T>
	using has_meta = std::experimental::is_detected<has_meta_t, T>;

	template<class T>
	using has_entity_root_t = decltype(T::entity_root);

	template<class T>
	using has_entity_root = std::experimental::is_detected<has_entity_root_t, T>;


	template<class T, class R = void>  
	struct enable_if_ns { typedef R type; };

	template<class T, class Enable = void>
	struct HasNs
	{
		constexpr static bool value = false;
	};

	template<class T>
	struct HasNs<T,typename enable_if_ns<decltype(T::xml_namespace)>::type>
	{
		constexpr static bool value = true;
	};
	
} // end namespace impl

template<class T>
constexpr bool has_ns()
{
    return impl::HasNs<T>::value;
};


}

#endif

