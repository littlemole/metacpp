#ifndef _GEF_GUARD_DEFINE_METACPP_META_H_DEF_GUARD_
#define _GEF_GUARD_DEFINE_METACPP_META_H_DEF_GUARD_


// MetaTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include "metacpp/traits.h"

///////////////////////////////////////////////////////////////////////////////////////////

namespace meta {

	class xmlns
	{
	public:

		const char* prefix = 0;
		const char* ns = 0;
		bool is_attribute = false;

		constexpr xmlns(bool attr = false)
			: is_attribute(attr)
		{}

		constexpr xmlns(const char* n, bool attr = false)
			: ns(n), is_attribute(attr)
		{}

		constexpr xmlns(const char* p, const char* n, bool attr = false)
			: prefix(p),ns(n), is_attribute(attr)
		{}
	};

	inline constexpr auto attribute()
	{
		return xmlns(true);
	}

	inline constexpr auto attribute(const char* n)
	{
		return xmlns(n,true);
	}

	inline constexpr auto attribute(const char* p, const char* n)
	{
		return xmlns(p,n,true);
	}

	class EntityName
	{
	public:

		const char* name = 0;
		xmlns ns;

		EntityName()
		{}

		EntityName(const char* n)
			:name(n)
		{}

		EntityName( const char* n, const xmlns& s)
			: name(n), ns(s)
		{}

		bool equals(const std::string& n) const
		{
			return n == name;
		}

		bool operator==(const std::string& n) const
		{
			return equals(n);
		}

		const char* ns_prefix() const
		{
			return ns.prefix;
		}

		const char* ns_uri() const
		{
			return ns.ns;
		}

		bool is_attribute() const
		{
			return ns.is_attribute;
		}
	};

namespace impl {

	///////////////////////////////////////////////////////////////////////////////////////////


	template<class T, class M>
	class Member
	{
	public:
		static constexpr bool is_member = true;

		const char* name = 0;
		xmlns ns;
		M T::* member = nullptr;

		using getter_value_t = M;
		using setter_value_t = M;

		getter_value_t get(const T& t) const
		{
			return t.*member;
		}

		void set(T& t, setter_value_t v) const
		{
			t.*member = v;
		}

	};

	template<class T, class M1, class M2>
	class GetterSetter
	{
	public:
		static constexpr bool is_member = true;

		typedef M1(T::* getter_t)();
		typedef void (T::* setter_t)(M2);

		using getter_value_t = std::remove_const_t<std::remove_reference_t<M1>>;
		using setter_value_t = std::remove_const_t<std::remove_reference_t <M2>>;

		const char* name = 0;
		xmlns ns;

		getter_t getter;
		setter_t setter;

		getter_value_t get( T& t) const
		{
			if (!getter)
				throw std::exception();

			return (t.*getter)();
		}

		void set(T& t, const setter_value_t& m) const
		{
			if (!setter)
				return;//throw std::exception();

			(t.*setter)(m);
		}
	};

	template<class T, class M1, class M2>
	class GetterSetterConst
	{
	public:
		static constexpr bool is_member = true;

		typedef M1(T::* getter_t)() const;
		typedef void (T::* setter_t)(M2);

		using getter_value_t = std::remove_const_t < std::remove_reference_t<M1>>;
		using setter_value_t = std::remove_const_t < std::remove_reference_t<M2>>;

		const char* name = 0;
		xmlns ns;

		getter_t getter;
		setter_t setter;

		getter_value_t get(const T& t) const
		{
			if (!getter)
				throw std::exception();

			return (t.*getter)();
		}

		void set(T& t, const setter_value_t& m) const
		{
			if (!setter)
				return; //throw std::exception();

			(t.*setter)(m);
		}
	};

	template< class F>
	class MemFun
	{
	public:
	
		static constexpr bool is_member = false;

		using getter_value_t = int;
		using setter_value_t = int;

		const char* name = 0;
		xmlns ns;

		F member = nullptr;

		template<class T, class ... Args>
		auto invoke(T& t, Args ... args)
		{
			return (t.*member)(args...);
		}

		template<class T>
		getter_value_t get(const T& t) const
		{
			return 0;
		}

		template<class T>
		void set(T& t, const setter_value_t& m) const
		{
		}
	};

	template< class R, class T, class ... Args>
	class MemFun<R(T,Args...)>
	{
	public:
		const char* name = 0;
		xmlns ns;

		R(T::*member)(Args...) = nullptr;

		R invoke(T& t, Args ... args)
		{
			return (t.*member)(args...);
		}
	};

	template< class T, class ... Args>
	class MemFun<void(T,Args...)>
	{
	public:
		const char* name = 0;
		xmlns ns;

		void(T::*member)(Args...) = nullptr;

		void invoke(T& t, Args ... args)
		{
			(t.*member)(args...);
		}
	};


	class EntityRoot
	{
	public:
		const char* root = 0;
		xmlns ns;

		using getter_value_t = void;
		using setter_value_t = void;

		template<class T>
		getter_value_t get(const T& t) const
		{
		}

		template<class T>
		void set(T& t, setter_value_t* m) const
		{
		}
	};

} // end namespace impl

	//////////////////////////////////////////////////////////////

	template<class M, class T>
	constexpr auto member(const char* name, M T::* m)
	{
		return impl::Member<T, M>{name, xmlns(), m};
	}

	template<class M, class T>
	constexpr auto member(const char* name, M T::* m, const xmlns& ns)
	{
		return impl::Member<T, M>{name, ns, m};
	}

	inline constexpr auto entity_root(const char* name)
	{
		return impl::EntityRoot{ name };
	}

	inline constexpr auto entity_root(const char* name, const xmlns& ns)
	{
		return impl::EntityRoot{ name, ns };
	}

	template<class M>
	constexpr auto mem_fun(const char* name, M m)
	{
		return impl::MemFun<M>{name, xmlns(),m };
	}

	template<class M>
	constexpr auto mem_fun(const char* name, M m, const xmlns& ns)
	{
		return impl::MemFun<M>{name, ns,m };
	}

	template<class T, class M1, class M2>
	constexpr auto getter_setter(const char* name, M1(T::* g)(), void (T::* s)(M2 m))
	{
		return impl::GetterSetter<T, M1, M2>{name, xmlns(), g, s};
	}

	template<class T, class M1, class M2>
	constexpr auto getter_setter(const char* name, M1(T::* g)(), void (T::* s)(M2 m), const xmlns& ns)
	{
		return impl::GetterSetter<T, M1, M2>{name, ns, g, s};
	}

	template<class T, class M1, class M2>
	constexpr auto getter_setter(const char* name, M1(T::* g)() const, void (T::* s)(M2 m))
	{
		return impl::GetterSetterConst<T, M1, M2>{name, xmlns(), g, s};
	}

	template<class T, class M1, class M2>
	constexpr auto getter_setter(const char* name, M1(T::* g)() const, void (T::* s)(M2 m), const xmlns& ns)
	{
		return impl::GetterSetterConst<T, M1, M2>{name, ns, g, s};
	}

	template<class T, class M>
	constexpr auto getter_setter(const char* name, M(T::* g)() )
	{
		return impl::GetterSetter<T, M, M>{ name, xmlns(),  g, nullptr };
	}

	template<class T, class M>
	constexpr auto getter_setter(const char* name, M(T::* g)(), const xmlns& ns )
	{
		return impl::GetterSetter<T, M, M>{ name, ns,  g, nullptr };
	}

	template<class T, class M>
	constexpr auto getter_setter(const char* name, M(T::* g)() const)
	{
		return impl::GetterSetterConst<T, M, M>{ name, xmlns(), g, nullptr };
	}

	template<class T, class M>
	constexpr auto getter_setter(const char* name, M(T::* g)() const, const xmlns& ns )
	{
		return impl::GetterSetterConst<T, M, M>{ name, ns, g, nullptr };
	}

	template<class T, class M>
	constexpr auto getter_setter(const char* name, void (T::* s)(M m))
	{
		return impl::GetterSetter<T, M, M>{name, xmlns(), nullptr, s};
	}

	template<class T, class M>
	constexpr auto getter_setter(const char* name, void (T::* s)(M m), const xmlns& ns )
	{
		return impl::GetterSetter<T, M, M>{name, ns, nullptr, s};
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	inline constexpr std::tuple<> data()
	{
		return std::tuple<>();
	}

	template<class T, class M, class ... Args>
	constexpr auto data(const char* n, M T::* m, Args ... args);

	template<class T, class M, class ... Args>
	constexpr auto data(impl::Member<T, M> m, Args ... args);

	template<class T, class M1, class M2, class ... Args>
	constexpr auto data(impl::GetterSetter<T, M1, M2> m, Args ... args);

	template<class T, class M1, class M2, class ... Args>
	constexpr auto data(impl::GetterSetterConst<T, M1, M2> m, Args ... args);

	template<class M, class ... Args>
	constexpr auto data(impl::MemFun<M> m, Args ... args);

	template<class ... Args>
	constexpr auto data(impl::EntityRoot m, Args ... args);

	template<class T, class M, class ... Args>
	constexpr auto data(const char* n, M T::* m, Args ... args)
	{
		return std::tuple_cat(std::make_tuple(meta::member(n, m)), data(args...));
	}

	template<class T, class M, class ... Args>
	constexpr auto data(impl::Member<T,M> m, Args ... args)
	{
		return std::tuple_cat(std::make_tuple(m),  data(args...));
	}

	template<class T, class M1, class M2, class ... Args>
	constexpr auto data(impl::GetterSetter<T, M1,M2> m, Args ... args)
	{
		return std::tuple_cat(std::make_tuple(m),  data(args...));
	}

	template<class T, class M1, class M2, class ... Args>
	constexpr auto data(impl::GetterSetterConst<T, M1, M2> m, Args ... args)
	{
		return std::tuple_cat(std::make_tuple(m), data(args...));
	}

	template<class M, class ... Args>
	constexpr auto data(impl::MemFun<M> m, Args ... args)
	{
		return std::tuple_cat(std::make_tuple(m),  data(args...));
	}

	template<class ... Args>
	constexpr auto data(impl::EntityRoot m, Args ... args)
	{
		return std::tuple_cat(std::make_tuple(m),  data(args...));
	}


	///////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	class Data
	{
	private:

		template<class P>
		static constexpr auto meta_of(typename std::enable_if<std::is_class<P>::value && impl::has_meta<P>::value>::type* = nullptr)
		{
			return P::meta();
		}

		template<class P>
		static constexpr auto meta_of(typename std::enable_if<std::is_class<P>::value && !impl::has_meta<P>::value>::type* = nullptr)
		{
			return std::tuple<>();
		}

	public:

		constexpr static auto meta( )
		{
			return meta_of<T>();
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	constexpr auto of(const T& )
	{
		return Data<T>::meta();
	}

	template<class T>
	constexpr auto of()
	{
		return Data<T>::meta();
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	EntityName entity_root()
	{
		auto m = of<T>();

		if constexpr (std::tuple_size<decltype(m)>::value == 0)
		{
			return EntityName();
		}
		else
		{
			auto r = std::get<0>(m);

			if constexpr (std::is_same<decltype(r), impl::EntityRoot>::value)
			{
				return EntityName(r.root,r.ns);
			}
			else
			{
				return EntityName();
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	template<class T,class M>
	class MetaProxy
	{
	public:
		T& t;
		M& m;

		using getter_value_t = typename M::getter_value_t;
		using setter_value_t = typename M::setter_value_t;
		
		template<class P>
		MetaProxy<T, M>& operator=(const P& p)
		{
			return assign(p, std::is_same<setter_value_t, P>() );
		}
		
		auto operator*() const
		{
			return m.get(t);
		}
		
		template<class P>
		operator P() const
		{
			return get<P>(std::is_same<P, getter_value_t>());
		}
		
	private:
		template<class P>
		MetaProxy<T, M>& assign(const P& p, std::true_type )
		{
			m.set(t, p);
			return *this;
		}

		template<class P>
		MetaProxy<T, M>& assign(const P& p, std::false_type)
		{
			return *this;
		}

		template<class P>
		P get( std::true_type) const
		{
			return m.get(t);
		}

		template<class P>
		P get( std::false_type) const
		{
			return P();
		}
	};



	///////////////////////////////////////////////////////////////////////////////////////////

	template<class M, class T, class V, size_t I>
	void visit(M& m, T& from, V visitor)
	{
		auto member = std::get<I>(m);
		if constexpr (!std::is_same<decltype(member), impl::EntityRoot>::value)
		{
			visitor(
				EntityName {member.name, member.ns},
				MetaProxy<T,decltype(member)> {from,member}
			);
		}
		if constexpr (I + 1 < std::tuple_size<M>::value)
		{
			meta::visit<M, T, V, I + 1>(m, from, visitor);
		}
	}



	template<class T, class V>
	void visit(T& from, V visitor)
	{
		auto m = meta::of(from);
		visit<decltype(m), T, V, 0>(m, from, visitor);
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	template<class M, class T, size_t I, class Tuple>
	auto tuple(const M& m, T& from, Tuple tup)
	{
		auto member = std::get<I>(m);
		if constexpr (std::is_same<decltype(member), impl::EntityRoot>::value)
		{
			return tuple<M, T, I + 1, decltype(tup)>(m, from, tup);
		}
		else
		{
			auto new_tup = std::tuple_cat(tup, std::make_tuple(member.get(from)));
			if constexpr (I + 1 < std::tuple_size<M>::value)
			{
				return tuple<M, T, I + 1, decltype(new_tup)>(m, from, new_tup);
			}
			else
			{
				return new_tup;
			}
		}
	}

	template<class T>
	auto tuple(T& from)
	{
		auto m = of(from);
		auto tup = std::tuple<>();
		return tuple<decltype(m), T, 0, decltype(tup)>(m, from, tup);
	}

	///////////////////////////////////////////////////////////////////////////////////////////


	template<class M, class T, size_t I, class V>
	void find(const M& m, const char* n, V visitor)
	{
		auto member = std::get<I>(m);

		if constexpr (std::is_same<decltype(member), impl::EntityRoot>::value)
		{
			return find<M, T, I + 1, V>(m, n, visitor);
		}
		else
		{

			if (member.name && strcmp(n, member.name) == 0)
			{
				visitor(member);
				return;
			}
			if constexpr (I + 1 < std::tuple_size<M>::value)
			{
				find<M, T, I + 1, V>(m, n, visitor);
			}
		}
	}

	template<class T, class V>
	void find(T& unused, const char* n, V visitor)
	{
		auto meta = of<T>();
		find<decltype(meta), T, 0, V>(meta, n, visitor);
	}

	template<class T, class V>
	void find(const char* n, V visitor)
	{
		auto meta = of<T>();
		find<decltype(meta), T, 0, V>(meta, n, visitor);
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	template<class R, class T>
	R get(T& from, char const* const n)
	{
		R result;
		find<T>(n, [&from, &result](auto m)
		{
			if constexpr (std::is_constructible<R, typename decltype(m)::getter_value_t>::value)
			{
				result = m.get(from);
			}
			else
			{
				throw std::exception();
			}
		});
		return result;
	}

	template<class T, class P>
	void set(T& to, char const* const n, P param)
	{
		find<T>(n, [&to, &param](auto m)
		{
			if constexpr (std::is_constructible<typename decltype(m)::setter_value_t, P>::value)
			{
				m.set(to, param);
			}
			else
			{
				throw std::exception();
			}
		});
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	template<class P, class T>
	bool has_getter(const char* n)
	{
		bool result = false;
		find<T>(n, [&result](auto m)
		{
			if constexpr (std::is_constructible<P, typename decltype(m)::getter_value_t>::value)
			{
				result = true;
			}
		});
		return result;
	}


	template<class P, class T>
	bool has_getter(const T& t, const char* n)
	{
		return has_getter<P, T>(n);
	}


	template<class P, class T>
	bool has_setter(const char* n)
	{
		bool result = false;
		find<T>(n, [&result](auto m)
		{
			if constexpr (std::is_constructible<typename decltype(m)::setter_value_t, P>::value)
			{
				result = true;
			}
		});
		return result;
	}

	template<class P, class T>
	bool has_setter(const T& t, const char* n)
	{
		return has_setter<P, T>(n);
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	template<class R, class M, class T, size_t I, class ... Args>
	R invoke(const M& m, T& from, const char* name, Args ... args)
	{
		auto member = std::get<I>(m);
		if constexpr (std::is_same<decltype(member), impl::EntityRoot>::value)
		{
			return invoke<R, M, T, I + 1, Args...>(m, from, name, args ...);
		}
		else
		{
			if constexpr (std::is_invocable_r<R, decltype(member.member), T, Args ...>::value)
			{
				if (strcmp(name, member.name) == 0)
				{
					return std::invoke(member.member, from, args...);
				}
			}
			if constexpr (I + 1 < std::tuple_size<M>::value)
			{
				return invoke<R, M, T, I + 1, Args...>(m, from, name, args ...);
			}
			throw std::exception();
			return R();
		}
	}

	template<class R, class T, class ... Args>
	R invoke(T& from, const char* n, Args ... args)
	{
		auto meta = of(from);
		return invoke<R, decltype(meta), T, 0, Args... >(meta, from, n, args...);
	}
} // end namespace meta



#endif
