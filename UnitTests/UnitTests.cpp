#include "pch.h"
#include "CppUnitTest.h"
#include "../ObjForGL/objLoader.hpp"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::string yoda_path = "yoda/yoda.obj";

namespace UnitTests
{
	TEST_CLASS(TestObjLoader)
	{
	public:
		
		TEST_METHOD(TESTLoadObjFile)
		{
			ObjLoader obj_loader;
			Assert::IsTrue(obj_loader.loadObjFile(yoda_path));
		}
	};
}
