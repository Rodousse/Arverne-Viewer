

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <exception>
#include "PbrViewer.hpp"

int main(int argc, char* argv[])
{
	PbrViewer App;
	App.setName("Triangle");
	try {
		App.run();
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}