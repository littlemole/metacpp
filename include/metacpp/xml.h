#ifndef _MOL_DEF_GUARD_DEFINE_META_SERIALIZER_XML_XXX_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_META_SERIALIZER_XML_XXX_DEF_GUARD_


#include <patex/document.h>
#include "metacpp/meta.h"


namespace meta {

///////////////////////////////////////////////////////////////////////////////////////////

// forwards

template<class T>
void toXml( const std::vector<T>& from, patex::xml::ElementPtr to);

template<class T>
void toXml( std::vector<T>& from, patex::xml::ElementPtr to);

template<class T>
void toXml( const T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void toXml( T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);


namespace impl {

// forwards

template<class T>
void toXml( const meta::EntityName& n, const std::vector<T>& from, patex::xml::ElementPtr to );

template<class T>
void toXml( const meta::EntityName& n, std::vector<T>& from, patex::xml::ElementPtr to );

template<class T>
void toXml( const meta::EntityName& n, const T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr );

template<class T>
void toXml( const meta::EntityName& n, T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr );

template<class T>
void fromXml( const meta::EntityName& name, patex::xml::ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void fromXml( const meta::EntityName& name, patex::xml::ElementPtr from, std::vector<T>&  to);

///////////////////////////////////////////////////////////////////////////////////////////

// helpers

inline patex::xml::ElementPtr createElement(const meta::EntityName& n, patex::xml::ElementPtr to)
{
	patex::xml::ElementPtr el;
	if(!n.ns_uri())
	{
		// create simple element
		el = to->ownerDocument()->createElement(n.name);
	}
	else
	{
		if ( to->defaultNamespace() == n.ns_uri() )
		{
			// create simple element
			el = to->ownerDocument()->createElement(n.name);
		}
		else if(!n.ns_prefix())
		{
			// create default namespaced element
			el = to->ownerDocument()->createElementNS(n.name,n.ns_uri());
		}
		else
		{
			std::ostringstream oss;
			oss << n.ns_prefix() << ":" << n.name;
			std::string en = oss.str();

			std::string prefix = to->getPrefixFromNS(n.ns_uri());
			if( prefix == n.ns_prefix())
			{
				// create prefixed element reusing prefix
				el = to->ownerDocument()->createElement(en);
			}
			else
			{
				// create prefixed element with xmlns
				el = to->ownerDocument()->createElementNS(en,n.ns_uri());
			}
		}
	}	
	return el;
}



inline void set_attribute(const meta::EntityName& n, const std::string& from, patex::xml::ElementPtr to)
{
	if(n.ns_uri() && n.ns_prefix())
	{
		std::ostringstream oss1;
		oss1 << n.ns_prefix() << ":" << n.name;
		std::string an = oss1.str();

		if(to->getNSfromPrefix(n.ns_prefix()) != n.ns_uri())
		{
			std::ostringstream oss2;
			oss2 << "xmlns:" << n.ns_prefix();
			to->setAttribute(oss2.str(),n.ns_uri());			
		}
		
		to->setAttribute(an,from);			
	}
	else
	{
		to->setAttribute(n.name,from);			
	}		
}

inline void string2Xml( const meta::EntityName& n,  const std::string& from, patex::xml::ElementPtr to)
{
	if(!n.name) return;

	if(n.is_attribute())
	{
		set_attribute(n,from,to);
		return;
	}

	patex::xml::ElementPtr el = createElement(n,to);
	if(el)
	{
		to->appendChild(el);
		if(!from.empty())
		{
			patex::xml::TextPtr txt = to->ownerDocument()->createTextNode(from);
			if(txt)
			{
				el->appendChild(txt);
			}
		}
	}
}

inline std::string fromXml(const meta::EntityName& name, patex::xml::ElementPtr from)
{
	if(!name.name)
	{
		return from->innerXml();
	}

	if(name.is_attribute())
	{
		if(name.ns_uri())
		{
			return from->attr(name.name,name.ns_uri());
		}
		else
		{
			return from->attr(name.name);
		}
	}

	patex::xml::ElementPtr el;
	if(name.ns_uri())
	{
		el = from->childNodes()->getChildByNameNS(name.name,name.ns_uri());
	}
	else
	{
		el = from->childNodes()->getChildByName(name.name);
	}
	
	if(el)
	{
		return el->innerXml();
	}	
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// toXml terminator

inline void toXml( const meta::EntityName& n,  const std::string& from, patex::xml::ElementPtr to)
{
	string2Xml(n,from,to);
}

inline void toXml( const meta::EntityName& n,  std::string& from, patex::xml::ElementPtr to)
{
	string2Xml(n,from,to);
}

template<class T>
void toXml( const meta::EntityName& n, const T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
	if(!n.name) return;

	std::ostringstream oss;
	oss << from;

	std::string tmp = oss.str();

	string2Xml(n,tmp,to);
}

template<class T>
void toXml( const meta::EntityName& n,  T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
	if(!n.name) return;

	std::ostringstream oss;
	oss << from;

	std::string tmp = oss.str();

	string2Xml(n,tmp,to);
}

///////////////////////////////////////////////////////////////////////////////////////////

// fromXml termiinators

inline void fromXml( const meta::EntityName& name, patex::xml::ElementPtr from, std::string& to)
{
	to = fromXml(name,from);
}

template<class T>
void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, T& to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
	std::istringstream iss( fromXml(name,from) );
	iss >> to;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// toXml recursive impl

template<class T>
void toXml( const meta::EntityName& n, const T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type*  )
{
	if(n.name)
	{
		patex::xml::ElementPtr el = createElement(n,to);
		to->appendChild(el);		

		auto visitor = [&el]( auto n, auto m)
		{	
			toXml(n, *m, el);
		};
		meta::visit(from,visitor);		   
	}
	else
	{
		auto visitor = [&to]( auto n, auto m)
		{	
			toXml(n, *m, to);
		};
		meta::visit(from,visitor);		
	}
}

template<class T>
void toXml( const meta::EntityName& n, T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type*  )
{
	if(n.name)
	{
		patex::xml::ElementPtr el = createElement(n,to);
		to->appendChild(el);		

		auto visitor = [&el]( auto n, auto m)
		{	
			toXml(n, *m, el);
		};
		meta::visit(from,visitor);		   
	}
	else
	{
		auto visitor = [&to]( auto n, auto m)
		{	
			toXml(n, *m, to);
		};
		meta::visit(from,visitor);		
	}
}


template<class T>
void toXml( const meta::EntityName& n, const std::vector<T>& from, patex::xml::ElementPtr to )
{
	for( auto& f : from)
	{
		toXml(n,f,to);				
	}
}

template<class T>
void toXml( const meta::EntityName& n,  std::vector<T>& from, patex::xml::ElementPtr to )
{
	for( auto& f : from)
	{
		toXml(n,f,to);				
	}
}

///////////////////////////////////////////////////////////////////////////////////////////

// fromXml recursive impl

template<class T>
void fromXml( const meta::EntityName& name, patex::xml::ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type* )
{
	auto el = from;

	if(name.name)
	{
		if(name.ns_uri())
		{
			el = from->childNodes()->getChildByNameNS(name.name,name.ns_uri());
		}
		else
		{
			el = from->childNodes()->getChildByName(name.name);	
		}		
	}

	auto visitor = [&el]( auto n, auto m)
	{	
		using value_t = std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::setter_value_t>;
		value_t value;

		fromXml(n,el,value);
		m = value;
	};
	meta::visit(to,visitor);	
}


template<class T>
void fromXml( const meta::EntityName& name, patex::xml::ElementPtr from, std::vector<T>&  to)
{
	to.clear();

	patex::xml::NodeListPtr items;
	if(name.ns_uri())
	{
		items = from->childNodes()->getChildrenByNameNS(name.name,name.ns_uri());
	}
	else
	{
		items = from->childNodes()->getChildrenByName(name.name);
	}
	

	int size = items->length();
	for( int i = 0; i < size; i++)
	{
		T t;
		fromXml( meta::EntityName(), std::dynamic_pointer_cast<patex::xml::Element>(items->item(i)), t);
		to.push_back(std::move(t));	
	}
}

} // end namespace impl

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// toXml API

template<class T>
void toXml( const T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* )
{
	const auto& r = meta::entity_root<T>();

	impl::toXml( r, from, to);
}


template<class T>
void toXml( T& from, patex::xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* )
{
	const auto& r = meta::entity_root<T>();

	impl::toXml( r, from, to);
}

template<class T>
std::shared_ptr<patex::xml::Document> toXml(const T& t)
{
	auto doc = patex::xml::Document::create();
	toXml(t,doc->documentElement());

	return doc;
}

template<class T>
std::shared_ptr<patex::xml::Document> toXml( T& t)
{
	auto doc = patex::xml::Document::create();
	toXml(t,doc->documentElement());

	return doc;
}

template<class T>
void toXml( const std::vector<T>& from, patex::xml::ElementPtr to )
{
	auto r = meta::entity_root<std::vector<T>>();
	if(!r.name) return;

	auto el = impl::createElement(r,to);
	to->appendChild(el);

	for( auto& i : from)
	{
		toXml(i,el);
	}
}

template<class T>
void toXml( std::vector<T>& from, patex::xml::ElementPtr to )
{
	auto r = meta::entity_root<std::vector<T>>();
	if(!r.name) return;

	auto el = impl::createElement(r,to);
	to->appendChild(el);

	for( auto& i : from)
	{
		toXml(i,el);
	}
}

//////////////////////////////////////////////////////////////

// fromXml Api

template<class T>
void fromXml( patex::xml::ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr )
{
	const auto& r = meta::entity_root<T>();

	impl::fromXml(r,from,to);
	return;
}


template<class T>
void fromXml( patex::xml::ElementPtr from, std::vector<T>& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr )
{
	const auto& r = meta::entity_root<std::vector<T>>();
	const auto& rt = meta::entity_root<T>();

	auto el = from;
	if(r.name)
	{
		if(r.ns_uri())
		{
			el = from->childNodes()->getChildByNameNS(r.name,r.ns_uri());
		}
		else
		{
			el = from->childNodes()->getChildByName(r.name);
		}
		impl::fromXml(rt,el,to);
		return;
	}
}


template<class T>
void fromXml(patex::xml::DocumentPtr doc,T& t )
{
	fromXml(doc->documentElement(),t);
}


template<class T>
void fromXml(const std::string& xml, T& t )
{
	auto doc = patex::xml::Document::parse_str(xml);
	fromXml(doc->documentElement(),t);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

} // end namespace meta

#endif

