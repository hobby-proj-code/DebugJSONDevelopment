#include "DebugJSON\DebugJSON.hpp"
#include <iostream>
#include <cstring>
#include <assert.h>

int main()
{
    FILE *fp = fopen("sample.json", "rb");
	char* contents=0;
	long contSize = 0;
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		contSize = ftell(fp);
		rewind(fp);
        char buff[contSize+1024]{};
		fread(buff, 1, contSize, fp);
		fclose(fp);
        JSON json(buff,contSize+1024);
        //json.get<bool>("topics", "topics", 0, "slug", 10);
		assert(json.path<bool>("topics", "topics", 0, "bumped").get() == true);
		std::cout<<"topics/topics[0]/bumped(true) --> "<<(json.path<bool>("topics", "topics", 0, "bumped").get() ? "true" : "false")<<std::endl;
		
		assert(json.path<float>("topics", "topics", 0, "id").get() == 8.8f);
		std::cout<<"topics/topics[0]/id(8.8) --> "<<json.path<float>("topics", "topics", 0, "id").get()<<std::endl;
		
		assert(strcmp(json.path<char[100]>("topics", "topics", 2, "slug").get(),"setting-for-google-api-key") == 0);
		std::cout<<"topics/topics[2]/slug(setting-for-google-api-key) --> "<<json.path<char[100]>("topics", "topics", 2, "slug").get()<<std::endl;

		assert(strcmp(json.path<char[100]>("topics", "topics", 2, "slugee").get("DUMMY"),"DUMMY") == 0);
		std::cout<<"topics/topics[2]/slugee(DUMMY) --> "<<json.path<char[100]>("topics", "topics", 2, "slugee").get("DUMMY")<<std::endl;
		
		assert(strcmp(json.path<char[100]>("topics", "topics", 2, "slugee").get(),"") == 0);
		std::cout<<"topics/topics[2]/slugee("") --> "<<json.path<char[100]>("topics", "topics", 2, "slugee").get()<<std::endl;

		assert(json.path<IsNull>("nulltest").get() == true);
		std::cout<<"nulltest(1) --> "<<json.path<IsNull>("nulltest").get()<<std::endl;
		
		assert(json.path<IsNull>("nulltest1").get() == false);
		std::cout<<"nulltest(0) --> "<<json.path<IsNull>("nulltest1").status<<std::endl;
		std::cout<<"///////////////////////////////////////////////////////////////////////"<<std::endl;

		assert(json.getArraySize("topics", "topics") == 30);
		std::cout<<"topics/topics Length(30) --> "<<json.getArraySize("topics", "topics")<<std::endl;

		assert(json.getArraySize("topics", "topics", 5, "posters") == 3);
		std::cout<<"topics/topics[5]/posters Length(3) --> "<<json.getArraySize("topics", "topics", 5, "posters")<<std::endl;
		
		assert(json.getArraySize("topics", "topics", 5, "poster") == -1);
		std::cout<<"Invalid topics/topics[5]/poster Length(-1) --> "<<json.getArraySize("topics", "topics", 5, "poster")<<std::endl;
		
		assert(json.getArraySize("ArrayTest1") == 0);
		std::cout<<"ArrayTest1(0) --> "<<json.getArraySize("ArrayTest1")<<std::endl;

		assert(json.getArraySize("ArrayTest2") == 1);
		std::cout<<"ArrayTest2(1) --> "<<json.getArraySize("ArrayTest2")<<std::endl;
		std::cout<<"///////////////////////////////////////////////////////////////////////"<<std::endl;
		
		assert(JSONInstance<16>("{}").validate() == true);
		std::cout<<"Validate {} (true) --> "<< (JSONInstance<16>("{}").validate() ? "true" : "false")<<std::endl;
		
		assert(JSONInstance<16>("{").validate() == false);
		std::cout<<"Validate { (false) --> "<< (JSONInstance<16>("{").validate() ? "true" : "false")<<std::endl;
		
		assert(JSONInstance<16>("{dd}").validate() == false);
		std::cout<<"Validate {dd} (false) --> "<< (JSONInstance<16>("{dd}").validate() ? "true" : "false")<<std::endl;
		
		assert(JSONInstance<16>("{\"dd\":1}").validate() == true);
		std::cout<<"Validate {\"dd\":1} (true) --> "<< (JSONInstance<16>("{\"dd\":1}").validate() ? "true" : "false")<<std::endl;
		std::cout<<"///////////////////////////////////////////////////////////////////////"<<std::endl;
		
		assert(json.path<bool>("topics", "topics", 0, "id").get() == false);
		std::cout<<"topics/topics[0]/id(0) --> "<<json.path<bool>("topics", "topics", 0, "id").get()<<std::endl;
		
		assert(strcmp(json.path<char[100]>("topics", "topics", 0, "bumped").get(),"") == 0);
		std::cout<<"topics/topics[0]/bumped() --> "<<json.path<char[100]>("topics", "topics", 0, "bumped").get()<<std::endl;
		std::cout<<"///////////////////////////////////////////////////////////////////////"<<std::endl;

		auto res = json.getRaw("topics", "topics", 0, "id");
		assert(strncmp(res.start,"8.8", 3) == 0);
		std::cout<<"(String)topics/topics[0]/id(8.8) --> ";
		std::cout.write(res.start, res.end - res.start + 1);
		std::cout<<std::endl;

		std::cout<<"(String)topics/topics[0]/bumped(true) --> ";
		res = json.getRaw("topics", "topics", 0, "bumped");
		assert(strncmp(res.start,"true", 4) == 0);
		std::cout.write(res.start, res.end - res.start + 1);
		std::cout<<std::endl;
		std::cout<<"///////////////////////////////////////////////////////////////////////"<<std::endl;
		std::cout<<"Level 1 missing --> ";
		assert(json.getRaw("nothing").start == json.getRaw("nothing").end && json.getRaw("nothing").status == false);
		res = json.getRaw("nothing");
		std::cout.write(res.start, 1);
		std::cout<<std::endl;

		std::cout<<"Array missing Index --> ";
		res = json.getRaw("users", 1000);
		assert(res.start == res.end && res.status == false && *res.start == ']');
		std::cout.write(res.start, res.end - res.start + 1);
		std::cout<<std::endl;

		std::cout<<"Empty JSON --> ";
		char emptyJSON[100]{};
		res = JSON(emptyJSON,100).getRaw("nothing");
		assert(res.start == res.end && res.status == false);
		std::cout.write(res.start, res.end - res.start);
		std::cout<<std::endl;

		std::cout<<"JSON filled with spaces --> ";
		char emptyJSON1[] = "                                                                 ";
		res = JSON(emptyJSON1,sizeof(emptyJSON1)).getRaw("nothing");
		assert(res.start == res.end && res.status == false);
		std::cout.write(res.start, res.end - res.start);
		std::cout<<std::endl;

		std::cout<<"Totally Corrupt JSON --> ";
		char emptyJSON2[] = "lksaclalck;sakccska;csajcjsajcsajcjsalcjlsajcsacj";
		res = JSON(emptyJSON2,sizeof(emptyJSON2)).getRaw("nothing");
		assert(res.start == res.end && res.status == false);
		std::cout.write(res.start, res.end - res.start);
		std::cout<<std::endl;

		
		std::cout<<"Pointing to array --> ";
		res = json.getRaw("ArrayTest2");
		assert(strncmp(res.start,"[1]",3) == 0);
		std::cout.write(res.start, res.end - res.start);
		std::cout<<std::endl;

		std::cout<<"Pointing to object --> ";
		res = json.getRaw("users", 4);
		assert(strncmp(res.start, 
			"{\"id\":73,\"username\":\"fimp\",\"avatar_template\":\"https://avatars.discourse.org/v2/letter/f/ee59a6/{size}.png\"}",
			sizeof("{\"id\":73,\"username\":\"fimp\",\"avatar_template\":\"https://avatars.discourse.org/v2/letter/f/ee59a6/{size}.png\"}") - 1) == 0);
		std::cout.write(res.start, res.end - res.start);
		std::cout<<std::endl;
		std::cout<<"///////////////////////////////////////////////////////////////////////"<<std::endl;
		JSONInstance<1024*32> ji1(buff);
		ji1.set(true,"nulltest1");
		std::cout.write(ji1.json, 100);
		std::cout<<"nulltest1 (\"nulltest\":null,\"nulltest1\":true,\"users\") --> ";
		assert(strncmp(ji1.json, 
			"{\"ArrayTest1\":[],\"ArrayTest2\":[1],\"nulltest\":null,\"nulltest1\":true,\"users\":[{\"id\":-1,\"username\":\"sys"
			, 100) == 0);
		std::cout<<std::endl;
		std::cout<<"///////////////////////////////////////////////////////////////////////"<<std::endl;
		std::cout<<23.568<<std::endl;
		JSONInstance<1024> ji;
		ji.set(23.56,"strange", "gortequsce", "enable", 10, 50, "xyz");
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}}}") == 0);
		ji.set(true,"start");
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":true}") == 0);
		ji.set(true,"start", "abc", "dev");
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":{\"abc\":{\"dev\":true}}}") == 0);
		ji.set(5, "array", 0);
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":{\"abc\":{\"dev\":true}},\"array\":[5]}") == 0);
		ji.set(136896, "xyz");
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":{\"abc\":{\"dev\":true}},\"array\":[5],\"xyz\":136896}") == 0);
		ji.set(6, "array", 1);
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":{\"abc\":{\"dev\":true}},\"array\":[5,6],\"xyz\":136896}") == 0);
		ji.set(7, "array", 2);
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":{\"abc\":{\"dev\":true}},\"array\":[5,6,7],\"xyz\":136896}") == 0);
		ji.set("str", "array", 1);
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":{\"abc\":{\"dev\":true}},\"array\":[5,\"str\",7],\"xyz\":136896}") == 0);
		ji.set("S","xyz");
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":{\"abc\":{\"dev\":true}},\"array\":[5,\"str\",7],\"xyz\":\"S\"}") == 0);
		ji.set("String","start");
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":\"String\",\"array\":[5,\"str\",7],\"xyz\":\"S\"}") == 0);
		ji.set(8, "array", 2);
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":\"String\",\"array\":[5,\"str\",8],\"xyz\":\"S\"}") == 0);
		ji.set(8, "array");
		std::cout<<ji.json<<std::endl;
		assert(strcmp(ji.json, "{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":\"String\",\"array\":8,\"xyz\":\"S\"}") == 0);
		std::cout<<"///////////////////////////////////////////////////////////////////////"<<std::endl;
		JSONInstance<1024> jr("{\"strange\":{\"gortequsce\":{\"enable\":[[{\"xyz\":23.56}]]}},\"start\":[  24 , 1  , 3,   8],   \"array\"     :     8       ,      \"xyz\":\"S\"}");
		jr.remove("array");
		std::cout<<jr.json<<std::endl;
		jr.remove("start", 0);
		std::cout<<jr.json<<std::endl;
		jr.remove("start", 1);
		std::cout<<jr.json<<std::endl;
		jr.remove("start", 1);
		std::cout<<jr.json<<std::endl;
		ji.set(23.56,"strange", "gortequsce", "enable", 0, 0, "xyz");
		std::cout<<jr.json<<std::endl;

		//when missing key at level 1;
		/*std::cout<<"--> Level 1 missing"<<std::endl;
		json.get(value, "nothing");
		std::cout<<"--> Array missing Index"<<std::endl;
		json.get(value, "users", 1000);
		std::cout<<"--> Empty JSON"<<std::endl;
		char emptyJSON[100]{};
		JSON json2(emptyJSON,100);
		json2.get(value, "nothing");
		std::cout<<"--> JSON filled with spaces"<<std::endl;
		char emptyJSON2[] = "                                                                                              ";
		JSON json3(emptyJSON2,100);
		json3.get(value, "nothing");
		std::cout<<"--> Totally Corrupt JSON"<<std::endl;
		char corruptJSON[] = "hsakichaocaoichihscoijsaocjposajcosajcosjc;oajocsjocjsaocjsao";
		JSON json4(corruptJSON,100);
		json4.get(value, "nothing");
		std::cout<<"--> Pointing to array"<<std::endl;
		json.get(value, "users");
		std::cout<<"--> Pointing to object"<<std::endl;
		json.get(value, "topics");*/
	}
	
    //JSON json((char*)"{\"check\":null,\"browsers\":{\"firefox\":{\"arr\":[0,1,2],\"name\":\"Firefox\",\"pref_url\":\"about:config\",\"releases\":{\"1\":{\"release_date\":\"2004-11-09\",\"status\":\"retired\",\"engine\":\"Gecko\",\"engine_version\":\"1.7\"}}}}}",0);
    //json.get<bool>("browsers", "firefox", "arr", 10);
    return 0;
}