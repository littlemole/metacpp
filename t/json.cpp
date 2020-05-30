#include "gtest/gtest.h"
#include <memory>
#include <list>
#include <utility>
#include <iostream>
#include <string>
#include <exception>
#include <functional>
#include "metacpp/meta.h"
#include "metacpp/json.h"
 
using namespace meta;

class JsonTest : public ::testing::Test {
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
	unsigned int x;
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
			//entity_root("TEST"),
			member("x", &TestObj::x),
			"txt", &TestObj::txt
			//getter_setter("txt", &Test::get, &Test::set)
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


TEST_F(JsonTest, simpleJson)
{
	TestObj t{ 42, "hello meta" };

	Json::Value json = toJson(t);

	std::string s = JSON::stringify(json);

	EXPECT_STREQ("{\n\t\"txt\" : \"hello meta\",\n\t\"x\" : 42\n}",s.c_str());

	TestObj t2;
	fromJson(json, t2);

	EXPECT_STREQ("hello meta",t2.txt.c_str());
	EXPECT_EQ(42,t2.x);
}


TEST_F(JsonTest, simpleJsonVector)
{
	TestObj t{ 42, "hello meta" };
    ArrayTest at;
    at.test.push_back(t);
    at.test.push_back(t);

	Json::Value json = toJson(at);

	std::string s = JSON::stringify(json);

	EXPECT_STREQ("{\n\t\"huhu\" : \n\t{\n\t\t\"test\" : \n\t\t[\n\t\t\t{\n\t\t\t\t\"txt\" : \"hello meta\",\n\t\t\t\t\"x\" : 42\n\t\t\t},\n\t\t\t{\n\t\t\t\t\"txt\" : \"hello meta\",\n\t\t\t\t\"x\" : 42\n\t\t\t}\n\t\t]\n\t}\n}",s.c_str());

	ArrayTest at2;
	fromJson(json, at2);

    json = toJson(at2);
	s = JSON::stringify(json);

	EXPECT_STREQ("{\n\t\"huhu\" : \n\t{\n\t\t\"test\" : \n\t\t[\n\t\t\t{\n\t\t\t\t\"txt\" : \"hello meta\",\n\t\t\t\t\"x\" : 42\n\t\t\t},\n\t\t\t{\n\t\t\t\t\"txt\" : \"hello meta\",\n\t\t\t\t\"x\" : 42\n\t\t\t}\n\t\t]\n\t}\n}",s.c_str());

}
 
TEST_F(JsonTest, jsonVector)
{
	TestObj t{ 42, "hello meta" };

    std::vector<TestObj> v;
    v.push_back(t);
    v.push_back(t);

	Json::Value json = toJson(v);

	std::string s = JSON::stringify(json);

	EXPECT_STREQ("{\n\t\"vector\" : \n\t[\n\t\t{\n\t\t\t\"txt\" : \"hello meta\",\n\t\t\t\"x\" : 42\n\t\t},\n\t\t{\n\t\t\t\"txt\" : \"hello meta\",\n\t\t\t\"x\" : 42\n\t\t}\n\t]\n}",s.c_str());

    std::vector<TestObj> v2;
	fromJson(json, v2);

    json = toJson(v2);
	s = JSON::stringify(json);

	EXPECT_STREQ("{\n\t\"vector\" : \n\t[\n\t\t{\n\t\t\t\"txt\" : \"hello meta\",\n\t\t\t\"x\" : 42\n\t\t},\n\t\t{\n\t\t\t\"txt\" : \"hello meta\",\n\t\t\t\"x\" : 42\n\t\t}\n\t]\n}",s.c_str());

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



TEST_F(JsonTest, toJson) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	Json::Value json = toJson(user);

	std::string s = JSON::flatten(json); 

	std::cout << s << std::endl;

	EXPECT_EQ("{\"user\":{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}}",s);

	User other;
	fromJson(json,other);

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

