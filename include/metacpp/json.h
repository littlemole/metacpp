#ifndef _GEF_GUARD_DEFINE_METACPP_JSON_H_DEF_GUARD_
#define _GEF_GUARD_DEFINE_METACPP_JSON_H_DEF_GUARD_


#include <iostream>
#include <sstream>
#include <json/json.h>

#include "metacpp/meta.h"

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
	inline Json::Value parse(const std::string& txt)
	{
		Json::Value json;

		Json::CharReaderBuilder rbuilder;
		std::string errs;
		std::istringstream iss(txt);
		bool ok = Json::parseFromStream(rbuilder, iss, &json, &errs);
		if (!ok)
		{
			throw ParseEx(errs);
		}
		return json;
	}

	//////////////////////////////////////////////////////////////

	//! serialize JSON structure into plaintext
	//! \ingroup json

	inline const std::string stringify(Json::Value value)
	{
		Json::StreamWriterBuilder wbuilder;
		return Json::writeString(wbuilder, value);

	}

	//! flatten a JSON structure removing whitespace and newlines
	//! \ingroup json
	inline const std::string flatten(Json::Value value)
	{
		Json::StreamWriterBuilder wbuilder;
		wbuilder["commentStyle"] = "None";
		wbuilder["indentation"] = "";
		return Json::writeString(wbuilder, value);
	}

} // end namespace JSON

namespace meta {

inline Json::Value exToJson(const std::exception& ex)
{
	Json::Value result(Json::objectValue);
	Json::Value err(Json::objectValue);

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
void toJson(const char* name, T& from, Json::Value& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void toJson(const char* name, const T& from, Json::Value& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void fromJson(const char* name, const Json::Value&, T& t);

///////////////////////////////////////////////////////////////////////////////////////////

inline void toJson(const char* name, Json::Value& from, Json::Value& to)
{
	to[name] = from;
}

inline void toJson(const char* name, const Json::Value& from, Json::Value& to)
{
	to[name] = from;
}

inline void toJson(const char* name, const std::string& from, Json::Value& to)
{
	to[name] = from;
}

inline void toJson(const char* name, std::string& from, Json::Value& to)
{
	to[name] = from;
}

template<class T >
void toJson(const char* name, const T& from, Json::Value& to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
	to[name] = from;
}

template<class T>
void toJson(const char* name, const std::vector<T>& from, Json::Value& to)
{
	Json::Value result(Json::arrayValue);

	for (auto& i : from)
	{
		Json::Value item(Json::objectValue);
		toJson(name, i, item);
		result.append(item[name]);
	}

	to[name] = result;
}

template<class T>
void toJson(const char* name, std::vector<T>& from, Json::Value& to)
{
	Json::Value result(Json::arrayValue);

	for (auto& i : from)
	{
		Json::Value item(Json::objectValue);
		toJson(name, i, item);
		result.append(item[name]);
	}

	to[name] = result;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

inline void fromJson(const char* name, const Json::Value& from, std::string& t)
{
	if (from.isMember(name))
	{
		t = from[name].asString();
	}
}

inline void fromJson(const char* name, const Json::Value& from, int& t)
{
	if (from.isMember(name))
	{
		t = from[name].asInt();
	}
}

inline void fromJson(const char* name, const Json::Value& from, unsigned int& t)
{
	if (from.isMember(name))
	{
		t = from[name].asUInt();
	}
}

inline void fromJson(const char* name, const Json::Value& from, double& t)
{
	if (from.isMember(name))
	{
		t = from[name].asDouble();
	}
}

inline void fromJson(const char* name, const Json::Value& from, float& t)
{
	if (from.isMember(name))
	{
		t = from[name].asFloat();
	}
}

inline void fromJson(const char* name, const Json::Value& from, bool& t)
{
	if (from.isMember(name))
	{
		t = from[name].asBool();
	}
}

inline void fromJson(const char* name, const Json::Value& from, long long& t)
{
	if (from.isMember(name))
	{
		t = from[name].asInt64();
	}
}

inline void fromJson(const char* name, const Json::Value& from, unsigned long long& t)
{
	if (from.isMember(name))
	{
		t = from[name].asUInt64();
	}
}

inline void fromJson(const char* name, const Json::Value& from, char& t)
{
	if (from.isMember(name))
	{
		t = from[name].asInt();
	}
}

inline void fromJson(const char* name, const Json::Value& from, unsigned char& t)
{
	if (from.isMember(name))
	{
		t = from[name].asInt();
	}
}

inline void fromJson(const char* name, const Json::Value& from, Json::Value& to)
{
	if (from.isMember(name))
	{
		to = from[name];
	}
}

template<class T>
void fromJson(const char* name, const Json::Value& from, std::vector<T>& v)
{
	v.clear();

	if (name && !from.isMember(name))
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
void toJson(const char* n, const T& from, Json::Value& to, typename std::enable_if<std::is_class<T>::value>::type*)
{
	auto result = Json::Value(Json::objectValue);

	auto visitor = [&from, &to, n](auto name, auto m)
	{
		auto value = *m;
		toJson(name.name, value, to[n]);
	};
	meta::visit(from, visitor);
}

template<class T>
void toJson(const char* n, T& from, Json::Value& to, typename std::enable_if<std::is_class<T>::value>::type*)
{
	auto result = Json::Value(Json::objectValue);

	auto visitor = [&to, n](auto name, auto m)
	{
		auto value = *m;
		toJson(name.name, value, to[n]);
	};
	meta::visit(from, visitor);
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromJson(const char* name, const Json::Value& from, T& t)
{
	if (name && !from.isMember(name))
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
Json::Value toJson( const T& t)
{
	Json::Value result(Json::objectValue);

	const char* root = meta::entity_root<T>();

	if (root)
	{
		impl::toJson(root, t, result);
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
Json::Value toJson( T& t)
{
	Json::Value result(Json::objectValue);

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

inline Json::Value toJson(const Json::Value& t)
{
	return t;
}

template<class T>
Json::Value toJson(const std::vector<T>& t)
{
	Json::Value v(Json::arrayValue);
	Json::Value& result = v;
	Json::Value root(Json::objectValue);

	for (auto& i : t)
	{
		Json::Value item(Json::objectValue);
		impl::toJson("dummy", i, item);
		v.append(item["dummy"]);
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
Json::Value toJson( std::vector<T>& t)
{
	Json::Value v(Json::arrayValue);
	Json::Value& result = v;
	Json::Value root(Json::objectValue);

	for (auto& i : t)
	{
		Json::Value item(Json::objectValue);
		impl::toJson("dummy", i, item);
		v.append(item["dummy"]);
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
void fromJson(const Json::Value& from, T& t)
{
	auto root = meta::entity_root<T>();

	impl::fromJson(root.name, from, t);
}

template<class T>
void fromJson(const std::string& from, T& t)
{
	Json::Value json = JSON::parse(from);
	impl::fromJson(json, t);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

} // end namespace meta


#endif
