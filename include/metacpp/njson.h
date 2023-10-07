#ifndef _GEF_GUARD_DEFINE_METACPP_JSON_H_DEF_GUARD_
#define _GEF_GUARD_DEFINE_METACPP_JSON_H_DEF_GUARD_


#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

#include "metacpp/meta.h"

//using namespace nlohmann;

///////////////////////////////////////////////////////////////////////////////////////////

namespace JSON {

	class ParseEx : public std::exception
	{
	public:
		ParseEx(const std::string& s)
			: msg(s)
		{}

		const char* what() const noexcept
		{
			return msg.c_str();
		}

		std::string msg;
	};

	//////////////////////////////////////////////////////////////

	//! parse string containing JSON 
	//! \ingroup json
	inline nlohmann::json parse(const std::string& txt)
	{
        try {
            nlohmann::json j = nlohmann::json::parse(txt);
    		return j;
        } 
        catch(...)
        {
			throw ParseEx("failed to parse json");
        }
	}

	//////////////////////////////////////////////////////////////

	//! serialize JSON structure into plaintext
	//! \ingroup json

	inline const std::string stringify( nlohmann::json value)
	{
		return value.dump(4);
	}

	//! flatten a JSON structure removing whitespace and newlines
	//! \ingroup json
	inline const std::string flatten( nlohmann::json value)
	{
        return value.dump();
	}

} // end namespace JSON

namespace meta {

inline nlohmann::json exToJson(const std::exception& ex)
{
    nlohmann::json result = nlohmann::json::object();
    nlohmann::json err    = nlohmann::json::object();

	err["type"] = typeid(ex).name();
	err["msg"] = ex.what();

	result["error"] = err;
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////

namespace impl {

///////////////////////////////////////////////////////////////////////////////////////////
// forwards

template<class T>
void toJson(const char* name, T& from, nlohmann::json& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void toJson(const char* name, const T& from, nlohmann::json& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void fromJson(const char* name, const nlohmann::json&, T& t);

///////////////////////////////////////////////////////////////////////////////////////////

inline void toJson(const char* name, nlohmann::json& from, nlohmann::json& to)
{
	to[name] = from;
}

inline void toJson(const char* name, const nlohmann::json& from, nlohmann::json& to)
{
	to[name] = from;
}

inline void toJson(const char* name, const std::string& from, nlohmann::json& to)
{
	to[name] = from;
}

inline void toJson(const char* name, std::string& from, nlohmann::json& to)
{
	to[name] = from;
}







inline void toJson(const char* name, const long long int& from, nlohmann::json& to)
{
	to[name] = from;
}

template<class T >
void toJson(const char* name, const T& from, nlohmann::json& to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
	to[name] = from;
}

template<class T>
void toJson(const char* name, const std::vector<T>& from, nlohmann::json& to)
{
    nlohmann::json result = nlohmann::json::array();

	for (auto& i : from)
	{
        nlohmann::json item = nlohmann::json::object();
		toJson(name, i, item);
		result.push_back(item[name]);
	}

	to[name] = result;
}

template<class T>
void toJson(const char* name, std::vector<T>& from, nlohmann::json& to)
{
	nlohmann::json result = nlohmann::json::array();

	for (auto& i : from)
	{
		nlohmann::json item = nlohmann::json::object();
		toJson(name, i, item);
		result.push_back(item[name]);
	}

	to[name] = result;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

inline void fromJson(const char* name, const nlohmann::json& from, std::string& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}

	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, int& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}
	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, unsigned int& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}

	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, double& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}
	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, float& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}
	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, bool& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}

	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, long long& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}

	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, unsigned long long& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}

	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, char& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}

	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, unsigned char& t)
{
	if(!name)
	{
		from.get_to(t);
		return;
	}
	if (from.contains(name))
	{
		from[name].get_to(t);
	}
}

inline void fromJson(const char* name, const nlohmann::json& from, nlohmann::json& to)
{
	if(!name)
	{
		to = from;
		return;
	}
	if (from.contains(name))
	{
		to = from[name];
	}
}

template<class T>
void fromJson(const char* name, const nlohmann::json& from, std::vector<T>& v)
{
	v.clear();

	if (name && !from.contains(name))
	{
		return;
	}

	auto f = name ? from[name] : from;

	unsigned int size = f.size();
	for (unsigned int i = 0; i < size; i++)
	{
		T t;
		fromJson(0,f[i], t);
		v.push_back(std::move(t));
	}
}



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void toJson(const char* n, const T& from, nlohmann::json& to, typename std::enable_if<std::is_class<T>::value>::type*)
{
    nlohmann::json result = nlohmann::json::object();

	auto visitor = [&from, &to, n](auto name, auto m)
	{
		auto value = *m;
		toJson(name.name, value, to[n]);
	};
	meta::visit(from, visitor);
}

template<class T>
void toJson(const char* n, T& from, nlohmann::json& to, typename std::enable_if<std::is_class<T>::value>::type*)
{
    nlohmann::json result = nlohmann::json::object();

	auto visitor = [&to, n](auto name, auto m)
	{
		auto value = *m;
		toJson(name.name, value, to[n]);
	};
	meta::visit(from, visitor);
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromJson(const char* name, const nlohmann::json& from, T& t)
{
	if (name && !from.contains(name))
		return;

	auto f = name ? from[name] : from;

	auto visitor = [&f](auto name, auto m)
	{
		std::remove_reference_t<typename decltype(m)::setter_value_t> value;
		fromJson(name.name, f, value);
		m = value;
	};

	meta::visit(t, visitor);
}

} // end namespace impl

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
nlohmann::json toJson( const T& t)
{
	nlohmann::json result = nlohmann::json::object();

	auto root = meta::entity_root<T>();

	if (root.name)
	{
		impl::toJson(root.name, t, result);
	}
	else
	{
		auto visitor = [&result](auto name, auto m)
		{
			auto value = *m;
			impl::toJson(name.name, value, result);
		};
		meta::visit(t, visitor);
	}

	return result;
}

template<class T>
nlohmann::json toJson( T& t)
{
    nlohmann::json result = nlohmann::json::object();

	auto root = meta::entity_root<T>();

	if (root.name)
	{
		impl::toJson(root.name, t, result);
	}
	else
	{
		auto visitor = [&result](auto name, auto m) 
		{
			auto value = *m;
			impl::toJson(name.name, value, result);
		};
		meta::visit(t, visitor);
	}

	return result;
}

inline nlohmann::json toJson(const nlohmann::json& t)
{
	return t;
}

inline nlohmann::json toJson(nlohmann::json& t)
{
	return t;
}

template<class T>
nlohmann::json toJson(const std::vector<T>& t)
{
    nlohmann::json v = nlohmann::json::array();
	nlohmann::json& result = v;
    nlohmann::json root = nlohmann::json::object();

	for (auto& i : t)
	{
        nlohmann::json item = nlohmann::json::object();
		impl::toJson("dummy", i, item);
		v.push_back(item["dummy"]);
	}

	auto r = meta::entity_root<std::vector<T>>();
	if (r.name)
	{
		root[r.name] = v;
		result = root;
	}

	return result;
}

template<class T>
nlohmann::json toJson( std::vector<T>& t)
{
    nlohmann::json v = nlohmann::json::array();
	nlohmann::json& result = v;
    nlohmann::json root = nlohmann::json::object();

	for (auto& i : t)
	{
        nlohmann::json item = nlohmann::json::object();
		impl::toJson("dummy", i, item);
		v.push_back(item["dummy"]);
	}

	auto r = meta::entity_root<std::vector<T>>();
	if (r.name)
	{
		root[r.name] = v;
		result = root;
	}

	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void fromJson(const nlohmann::json& from, T& t)
{
	auto root = meta::entity_root<T>();

	impl::fromJson(root.name, from, t);
}

template<class T>
void fromJson(const std::string& from, T& t)
{
	nlohmann::json j = JSON::parse(from);
	fromJson(j, t);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

} // end namespace meta


#endif
