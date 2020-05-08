#include "gtest/gtest.h"
#include <memory>
#include <list>
#include <utility>
#include <iostream>
#include <string>
#include <exception>
#include <functional>
#include "metacpp/meta.h"

class GetterSetterTest : public ::testing::Test {
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

    int get_x()
    {
        return x;
    }

    void set_x(int i)
    {
        x = i;
    }

	const std::string&  get_txt() const
	{
		return txt;
	}

	void set_txt(  std::string s)
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
			getter_setter("x", &TestObj::get_x, &TestObj::set_x),
			getter_setter("txt", &TestObj::get_txt, &TestObj::set_txt)
		);
	}
};






TEST_F(GetterSetterTest, simpleVisit)
{
	TestObj t{ 42, "hello meta" };

	int x = 0;
	meta::visit(t, [&x](auto n, auto m) 
	{
		if (n == "txt")
		{
			m = std::string("arrrg");
		}
		if (n.equals("x"))
		{
			x = m;
		}
	});

	EXPECT_STREQ("arrrg",t.txt.c_str());
	EXPECT_EQ(42,x);
}



TEST_F(GetterSetterTest, simpleSet)
{
	TestObj t{ 42, "hello meta" };

	meta::set(t, "x", 43);

	EXPECT_STREQ("hello meta",t.txt.c_str());
	EXPECT_EQ(43,t.x);
}



TEST_F(GetterSetterTest, simpleGet)
{
	TestObj t{ 42, "hello meta" };

	int x = meta::get<int>(t, "x");

	EXPECT_EQ(42,x);
}


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int r = RUN_ALL_TESTS();

	return r;
}

