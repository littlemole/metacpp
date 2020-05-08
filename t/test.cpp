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

class BasicTest : public ::testing::Test {
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
	/*
	static constexpr const char* entity_root()
	{
		return "test";
	}
	*/

	/*
	static constexpr auto meta() 
	{
		return meta::data<Test>(
			meta::entity_root("TEST"),
			//"x", &Test::x,
			meta::member("x", &Test::x),
			"txt", &Test::txt
			//member("x", &Test::x),
			//meta::member("txt", &Test::txt)
			//getter_setter("txt", &Test::get, &Test::set)
		);
	}
	*/
};


template<>
struct meta::Data<TestObj>
{
	static constexpr auto meta()
	{
		return meta::data<TestObj>(
			entity_root("TEST", xmlns("myns","http://somewhere.com/")),
			member("x", &TestObj::x, attribute("myns","http://somewhere.com/")),
			"txt", &TestObj::txt
			//getter_setter("txt", &Test::get, &Test::set)
		);
	}
};




class Operation
{
public:

	std::string test(int x, std::string txt)
	{
		std::ostringstream oss;
		oss << txt << " " << x;
		return oss.str();
	}

	int test2(int x, std::string txt)
	{
		return x + txt.size();
	}

	std::string test3(int x)
	{
		std::ostringstream oss;
		oss << "test3: "  << x;
		return oss.str();
	}

	constexpr static auto meta() 
	{
		using namespace meta;
		return data<Operation>(
			entity_root("operation"),
			mem_fun("test", &Operation::test),
			mem_fun("test2", &Operation::test2),
			mem_fun("test3", &Operation::test3)
		);
	}
};

class SingleTest
{
public:

	std::vector<TestObj> test;

	constexpr static auto meta() 
	{
		return meta::data<SingleTest>(
			meta::entity_root("huhu"),
			meta::member("test", &SingleTest::test)
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


TEST_F(BasicTest, simpleVisit)
{
	TestObj t{ 42, "hello meta" };

	unsigned int x = 0;
	meta::visit(t, [&x](auto n, auto m) 
	{
		if (n ==  "txt")
		{
			m = std::string("arrrg");
		}
		if (n == "x") 
		{
			x = m;
		}
	});

	EXPECT_STREQ("arrrg",t.txt.c_str());
	EXPECT_EQ(42,x);
}



TEST_F(BasicTest, simpleSet)
{
	TestObj t{ 42, "hello meta" };

	meta::set(t, "x", 43);

	EXPECT_STREQ("hello meta",t.txt.c_str());
	EXPECT_EQ(43,t.x);
}


TEST_F(BasicTest, xmlNs)
{
	TestObj t{ 42, "hello meta" };

	auto r = meta::entity_root<TestObj>();

	EXPECT_STREQ("http://somewhere.com/",r.ns_uri());
	EXPECT_STREQ("myns",r.ns_prefix());

	std::string p;
	std::string ns;
	bool is_attr = false;
	meta::visit(t, [&p,&ns,&is_attr](auto n, auto m) 
	{
		if (n == "x") 
		{
			if(n.ns_prefix())
				p = n.ns_prefix();
			ns = n.ns_uri();
			is_attr = n.is_attribute();
		}
	});
	EXPECT_STREQ("http://somewhere.com/",ns.c_str());
	EXPECT_STREQ("myns",p.c_str());
	EXPECT_EQ(true,is_attr);
}


TEST_F(BasicTest, simpleConst)
{
	const TestObj t{ 42, "hello meta" };

	int found = 0;

	meta::visit(t, [&found](auto n, auto m)
	{
		if (n == "txt")
		{
			found++;
			std::string s = m;
			EXPECT_STREQ("hello meta",s.c_str());
		}
		if ( n == "x") 
		{
			found++;
			unsigned int i = m;
			EXPECT_EQ(42,i);
		}
	});
	EXPECT_EQ(2,found);
}

int tester()
{

}

TEST_F(BasicTest, simpleConstExpr)
{
	//	constexpr int x = tester(); // that would fail to compile

	constexpr auto m = meta::of<TestObj>();
}

/*
int main()
{

	TestObj t{ 42, "hello meta" };

	auto meta = meta::of(t);

	unsigned int x = 0;

	meta::visit(t, [&t,&x](auto n, auto m) 
	{
//		using M = decltype(m);
	//	typename M::getter_value_t value = m.get(t);
		std::cout << n.name << ":" << *m << std::endl;
	//	if constexpr (std::is_assignable<typename M::getter_value_t,const char *>::value)
		//if constexpr (std::is_assignable<decltype(m)::setter_value_t, const char*>())
		{
			if (strcmp(n.name, "txt") == 0)
			{
				//m.set(t,"arrg");
				m = std::string("arrrg");
			}
		}
		//if constexpr (std::is_assignable<decltype(m)::setter_value_t, int>())
		{
			if (strcmp(n.name, "x") == 0)
			{
				//m.set(t,"arrg");
				//m = "arrrg";
				x = m;
			}
		}
	});

	std::cout << "X :" << x << std::endl;

	meta::set(t, "x", 43);

	meta::visit(t, [&t](auto n, auto m)
	{
		//using M = decltype(m);
		//typename M::getter_value_t value = m.get(t);
		std::cout << n.name << ":" << *m << std::endl;
	});

	const TestObj t2 = t;

	meta::visit(t2, [&t](auto n, auto m)
	{
		//using M = decltype(m);
		//typename M::getter_value_t value = m.get(t);

		std::cout << n.name << ":" << *m << std::endl;
	});

	
	if (meta::has_setter<int>(t,"x"))
	{
		meta::set(t, "x", 44);
	}

	int tmp = meta::get<int>(t, "x");
	std::cout << tmp << std::endl;

	meta::find(t,"x", [&t](auto m) 
	{
		if constexpr (std::is_constructible<typename decltype(m)::setter_value_t,int>::value)
		{
			m.set(t, 45);
		}
	});

	std::cout << meta::get<std::string>(t,"txt") << std::endl;

	auto tu = meta::tuple(t);
	std::cout << std::get<0>(tu) << std::endl;
	std::cout << std::get<1>(tu) << std::endl;

	auto er = meta::entity_root<TestObj>();

	if(!er.name)
	{
		er = "NO ROOT";
	}

	std::cout << er.name << std::endl;
	
	Operation op;

	std::cout << meta::invoke<std::string>(op,"test",42,"hu") << std::endl;

	Json::Value json = toJson(t);

	std::cout << JSON::stringify(json) << std::endl;

	TestObj t3;
	fromJson(json, t3);

	std::cout << t3.x << ":" << t3.txt << std::endl;

	std::vector<TestObj> v{ t,t2,t3 };

	json = toJson(v);
	std::cout << JSON::stringify(json) << std::endl;

	std::vector<TestObj> v2;
	fromJson(json, v2);

	std::cout << v2.size() << std::endl;
	json = toJson(v2);
	std::cout << JSON::stringify(json) << std::endl;

	SingleTest st;
	st.test.push_back(t);
	st.test.push_back(t2);

	json = toJson(st);
	std::cout << JSON::stringify(json) << std::endl;

	SingleTest st2;
	fromJson(json, st2);

	json = toJson(st2);
	std::cout << JSON::stringify(json) << std::endl;

}
*/

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int r = RUN_ALL_TESTS();

	return r;
}

