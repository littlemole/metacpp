#include "gtest/gtest.h"
#include "metacpp/json.h"
#include "metacpp/xml.h"


using namespace patex;
using namespace meta;

class BasicSerializationTest : public ::testing::Test {
 protected:
 
  static void SetUpTestCase() {

	  //theLoop().signal(SIGPIPE).then([](int s){});

  }

  virtual void SetUp() {
	 // MOL_TEST_PRINT_CNTS();
  }
 
  virtual void TearDown() {
	 // MOL_TEST_PRINT_CNTS();
  }
}; // end test setup




struct XmlTest
{
	XmlTest()
	{
		id = "42";
	}

	std::string id;

	struct XmlTest1 {

	  int index = 1;

		struct XmlTest2 {

			XmlTest2()
			{
				v.push_back("one");
				v.push_back("two");
				v.push_back("three");
			}
			std::vector<std::string> v;  


		}	level2;

	} level1;

};  

template<>
struct meta::Data<XmlTest::XmlTest1::XmlTest2>
{
        static constexpr auto meta()
        {
                return meta::data(
					"v", &XmlTest::XmlTest1::XmlTest2::v
				);
		}
};

    

template<>
struct meta::Data<XmlTest::XmlTest1>
{
        static constexpr auto meta()
        {
                return meta::data(
					member("index", &XmlTest::XmlTest1::index, attribute()),
					"level2", &XmlTest::XmlTest1::level2
				);
		}
};
 

template<>
struct meta::Data<XmlTest>
{
        static constexpr auto meta()
        {
                return meta::data(
					entity_root("XmlTest"),
					member("id", &XmlTest::id, attribute()),
					"level1", &XmlTest::level1
				);
		}
};
 
 


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
                return meta::data(
					entity_root("user"),
					"username", &User::username,
					"login", &User::login,
					"pwd", &User::pwd,
					"tags", &User::tags
				);
		}
};

 

class Users 
{
public:
	std::string something;
	std::vector<User> users;

	static constexpr auto meta()
	{
		return meta::data(
			"something", &Users::something,
			"users", &Users::users
		);
	}
};

class ArrayTest 
{
public:

	std::vector<std::vector<int>> array;

	static constexpr auto meta()
	{
		return meta::data(
			"array", &ArrayTest::array
		);
	}
};

class Dummy
{
public:
	int x = 0;

	static constexpr auto meta()
	{
		return meta::data(
			"value", &Dummy::x
		);
	}	
};
 
   
 
TEST_F(BasicSerializationTest, toXml) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	std::shared_ptr<xml::Document> doc = toXml(user);

	std::string s = doc->toString(); 
 
	std::cout << s << std::endl;

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<user><username>mike</username><login>littlemole</login><pwd>secret</pwd><tags>one</tags><tags>two</tags><tags>three</tags></user>",s);

	User other;
	fromXml(doc,other);

	EXPECT_EQ("mike",other.username);
	EXPECT_EQ("littlemole",other.login);
	EXPECT_EQ("secret",other.pwd);

	EXPECT_EQ("one",other.tags[0]);
	EXPECT_EQ("two",other.tags[1]);
	EXPECT_EQ("three",other.tags[2]);
 
	XmlTest xt;
	doc = toXml(xt);
   
	s = doc->toString();

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<XmlTest id=\"42\"><level1 index=\"1\"><level2><v>one</v><v>two</v><v>three</v></level2></level1></XmlTest>",s);

	std::cout << s << std::endl;

	xml::DocumentPtr d2 = xml::Document::create();
	xml::Element* p = d2->parse(s);

	std::cout << "--- " << (int*)p << " ---" << std::endl;

	std::cout << "--- " << d2->toString() << " ---" << std::endl;

	XmlTest rxto;
	rxto.id = "";
	rxto.level1.index = 0;

	fromXml(d2,rxto);
 
	doc = toXml(rxto);
	s = doc->toString();
std::cout << s << std::endl;

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<XmlTest id=\"42\"><level1 index=\"1\"><level2><v>one</v><v>two</v><v>three</v></level2></level1></XmlTest>", s);
}


TEST_F(BasicSerializationTest, toJson) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	Json::Value json = toJson(user);

	std::string s = JSON::flatten(json); 

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
    
 
TEST_F(BasicSerializationTest, toJsonArray) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	std::vector<User> root;
	root.push_back(user);
	root.push_back(user);
	root.push_back(user);  
   
	Json::Value json = toJson(root);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("[{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"},{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"},{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}]",s);

	std::vector<User> other; 
	fromJson(json,other);   

	for(int i = 0; i < 3; i++)
	{
	 	EXPECT_EQ("mike",other[i].username);
		EXPECT_EQ("littlemole",other[i].login);
		EXPECT_EQ("secret",other[i].pwd);

		EXPECT_EQ("one",other[i].tags[0]);
		EXPECT_EQ("two",other[i].tags[1]);
		EXPECT_EQ("three",other[i].tags[2]);
	}
}

template<class T>
std::string joinTabs(const std::vector<T>& v)
{
	if(v.empty())
	return "";

	std::ostringstream oss;

	for( auto i : v)
	{
		oss << csv_quote(i);
		oss << '\t';
	}

	std::string result = oss.str();
	return result.substr(0,result.size()-1);
}

TEST_F(BasicSerializationTest, toJsonArray2) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	Users users { "the thing", { user } };

	Json::Value json = toJson(users);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("{\"something\":\"the thing\",\"users\":[{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}]}",s);

	Users other;
	fromJson(json,other);

	EXPECT_EQ("the thing",other.something);

	EXPECT_EQ("mike",other.users[0].username);
	EXPECT_EQ("littlemole",other.users[0].login);
	EXPECT_EQ("secret",other.users[0].pwd);

	EXPECT_EQ("one",other.users[0].tags[0]);
	EXPECT_EQ("two",other.users[0].tags[1]);
	EXPECT_EQ("three",other.users[0].tags[2]);
}

 
TEST_F(BasicSerializationTest, toJsonArray3) 
{
	ArrayTest at { { {1,2,3}, {4,5,6} } };

	Json::Value json = toJson(at);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("{\"array\":[[1,2,3],[4,5,6]]}",s);

	ArrayTest other;
	fromJson(json,other);

	EXPECT_EQ(1,other.array[0][0]);
	EXPECT_EQ(2,other.array[0][1]);
	EXPECT_EQ(3,other.array[0][2]);
	EXPECT_EQ(4,other.array[1][0]);
	EXPECT_EQ(5,other.array[1][1]);
	EXPECT_EQ(6,other.array[1][2]);

}

TEST_F(BasicSerializationTest, toJsonArray4) 
{
	std::vector<std::vector<std::string>> v { {"a","b"}, {"c","d"} };

	Json::Value json = toJson(v);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("[[\"a\",\"b\"],[\"c\",\"d\"]]",s);

	std::vector<std::vector<std::string>> other;
	fromJson(json,other);

	EXPECT_EQ("a",other[0][0]);
	EXPECT_EQ("b",other[0][1]);
	EXPECT_EQ("c",other[1][0]);
	EXPECT_EQ("d",other[1][1]);
} 


int main(int argc, char **argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

    return r;
}
