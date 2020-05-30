#include "gtest/gtest.h"
#include <memory>
#include <list>
#include <utility>
#include <iostream>
#include <string>
#include <exception>
#include <functional>
#include "metacpp/meta.h"
#include "metacpp/xml.h"

using namespace meta;

class XmlTest : public ::testing::Test {
protected:

	static void SetUpTestCase() {

	}

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
}; // end test setup


  
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class TestObj
{
public:
	int x;
	std::string txt;

	const std::string&  get() const
	{
		return txt;
	}

	void set(  std::string s)
	{
		txt = s;
	}
};


template<>
struct meta::Data<TestObj>
{
	static constexpr auto meta()
	{
		return meta::data<TestObj>(
			entity_root("TEST"),
			member("x", &TestObj::x),
//            meta::member("txt", &TestObj::txt, meta::attribute())
			"txt", &TestObj::txt
			//getter_setter("txt", &Test::get, &Test::set)
		);
	}
};

class TestObj2
{
public:
	int x;
	std::string txt;

	const std::string& get() 
	{
		return txt;
	}

	void set(std::string s)
	{
		txt = s;
	}

    static constexpr auto meta()
    {
        return meta::data<TestObj2>(
            meta::entity_root("TEST"),
            meta::member("x", &TestObj2::x, meta::attribute()),
			//meta::member("txt", &TestObj2::txt, meta::attribute())
			meta::getter_setter("txt",&TestObj2::get, &TestObj2::set, meta::attribute())
        );
    }
}; 
    
         
class TestObj3
{
public:
	int x;
	std::string txt;

    static constexpr auto meta()
    {
        return meta::data<TestObj3>(
            meta::entity_root("TEST", meta::xmlns(/*"myns",*/"urn:myawesome.ns")),
            meta::member("x", &TestObj3::x, meta::xmlns("o","urn:other.ns")),
            meta::member("txt", &TestObj3::txt, meta::xmlns("p","urn:myawesome.ns"))
        );
    }
};    

         
class TestObj4
{
public:
	int x;
	std::string txt;

    static constexpr auto meta()
    {
        return meta::data<TestObj4>(
            meta::entity_root("TEST", meta::xmlns("myns","urn:myawesome.ns")),
            meta::member("x", &TestObj4::x, meta::attribute("o","urn:other.ns")),
            meta::member("txt", &TestObj4::txt, meta::xmlns("p","urn:myawesome.ns"))
        );
    }
};  

class TestObj5
{
public:
	int x;
	std::string txt;

    static constexpr auto meta()
    {
        return meta::data<TestObj5>(
            meta::entity_root("TEST", meta::xmlns(/*"myns",*/"urn:myawesome.ns")),
            meta::member("x", &TestObj5::x, meta::xmlns("o","urn:other.ns")),
            meta::member("txt", &TestObj5::txt, meta::xmlns("p","urn:myprivate.ns"))
        );
    }
};   


class ArrayTest
{
public:

	std::vector<TestObj> test;

	constexpr static auto meta() 
	{
		return meta::data<ArrayTest>(
			meta::entity_root("huhu"),
			meta::member("test", &ArrayTest::test)
		);
	}
};


template<>
class meta::Data<std::vector<TestObj>>
{
public:

	constexpr static auto meta()
	{
		return meta::data<std::vector<TestObj>>(
			meta::entity_root("vector")
		);
	}
};

  
TEST_F(XmlTest, simpleXml)
{
	TestObj t{ 42, "hello\"<&> meta" };

	auto xml = toXml(t);

	std::string s = xml->toString();
    std::cout << s << std::endl;

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<TEST><x>42</x><txt>hello\"&lt;&amp;&gt; meta</txt></TEST>",s.c_str());
    
	TestObj t2;
	fromXml(s, t2);

	EXPECT_STREQ("hello\"<&> meta",t2.txt.c_str());
	EXPECT_EQ(42,t2.x);
     
}
     
            
            
TEST_F(XmlTest, simpleXml2)
{
	 TestObj2 t{ 42, "hello <&> meta" };

	auto xml = toXml(t);
 
	std::string s = xml->toString();
    std::cout << s << std::endl;

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<TEST x=\"42\" txt=\"hello &lt;&amp;> meta\" />",s.c_str());
    
	TestObj2 t2;
	fromXml(xml, t2);

	EXPECT_STREQ("hello <&> meta",t2.txt.c_str());
	EXPECT_EQ(42,t2.x);
      
}       

TEST_F(XmlTest, simpleXml3)
{
	TestObj3 t{ 42, "hello meta" };

	auto xml = toXml(t);

	std::string s = xml->toString();
    std::cout << s << std::endl;

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<TEST xmlns=\"urn:myawesome.ns\"><o:x xmlns:o=\"urn:other.ns\">42</o:x><txt>hello meta</txt></TEST>",s.c_str());
    
	TestObj3 t2;
	fromXml(xml, t2);

	EXPECT_STREQ("hello meta",t2.txt.c_str());
	EXPECT_EQ(42,t2.x);
      
} 
   
   
TEST_F(XmlTest, simpleXml4)
{
	TestObj4 t{ 42, "hello meta" };

	auto xml = toXml(t);

	std::string s = xml->toString();
    std::cout << s << std::endl;

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<myns:TEST xmlns:myns=\"urn:myawesome.ns\" xmlns:o=\"urn:other.ns\" o:x=\"42\"><p:txt xmlns:p=\"urn:myawesome.ns\">hello meta</p:txt></myns:TEST>",s.c_str());
    
	TestObj4 t2;
	fromXml(xml, t2);

	EXPECT_STREQ("hello meta",t2.txt.c_str());
	EXPECT_EQ(42,t2.x);
      
}


TEST_F(XmlTest, simpleXml5)
{
	TestObj5 t{ 42, "hello meta" };

	auto xml = toXml(t);

	std::string s = xml->toString();
    std::cout << s << std::endl;

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<TEST xmlns=\"urn:myawesome.ns\"><o:x xmlns:o=\"urn:other.ns\">42</o:x><p:txt xmlns:p=\"urn:myprivate.ns\">hello meta</p:txt></TEST>",s.c_str());
    
	TestObj5 t2;
	fromXml(xml, t2);

	EXPECT_STREQ("hello meta",t2.txt.c_str());
	EXPECT_EQ(42,t2.x);
      
}

TEST_F(XmlTest, simpleXmlVector)
{
	TestObj t{ 42, "hello meta" };
    ArrayTest at;
    at.test.push_back(t);
    at.test.push_back(t);

	auto xml = toXml(at);

	std::string s = xml->toString();
    std::cout << s << std::endl;

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<huhu><test><x>42</x><txt>hello meta</txt></test><test><x>42</x><txt>hello meta</txt></test></huhu>",s.c_str());

	ArrayTest at2;
	fromXml(xml, at2);

    xml = toXml(at2);
	s = xml->toString();

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<huhu><test><x>42</x><txt>hello meta</txt></test><test><x>42</x><txt>hello meta</txt></test></huhu>",s.c_str());

}


TEST_F(XmlTest, xmlVector)
{
	TestObj t{ 42, "hello meta" };

    std::vector<TestObj> v;
    v.push_back(t);
    v.push_back(t);

	auto xml = toXml(v);
	std::string s = xml->toString();

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<vector><TEST><x>42</x><txt>hello meta</txt></TEST><TEST><x>42</x><txt>hello meta</txt></TEST></vector>",s.c_str());

    std::vector<TestObj> v2;
	fromXml(xml, v2);

	xml = toXml(v2);
	s = xml->toString();

	EXPECT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<vector><TEST><x>42</x><txt>hello meta</txt></TEST><TEST><x>42</x><txt>hello meta</txt></TEST></vector>",s.c_str());

} 
 


struct User
{
public:

	std::string username;
	std::string login;
	std::string pwd;
	std::vector<std::string> tags;
};

template<>
struct meta::Data<User>
{
        static constexpr auto meta()
        {
                return meta::data<User>(
					entity_root("user"),
					"username", &User::username,
					"login", &User::login,
					"pwd", &User::pwd,
					"tags", &User::tags
				);
		}
};



TEST_F(XmlTest, toXml) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };

	auto xml = toXml(user);
	std::string s = xml->toString();

	std::cout << s << std::endl;

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<user><username>mike</username><login>littlemole</login><pwd>secret</pwd><tags>one</tags><tags>two</tags><tags>three</tags></user>",s);

	User other;
	fromXml(xml,other);

	EXPECT_EQ("mike",other.username);
	EXPECT_EQ("littlemole",other.login);
	EXPECT_EQ("secret",other.pwd);

	EXPECT_EQ("one",other.tags[0]);
	EXPECT_EQ("two",other.tags[1]);
	EXPECT_EQ("three",other.tags[2]);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int r = RUN_ALL_TESTS();

	return r;
}

