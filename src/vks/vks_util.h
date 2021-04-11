//
// Created by magnias on 05/04/2021.
//

#ifndef _VKS_UTIL_H_
#define _VKS_UTIL_H_

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class VksUtil
{
 public:

	static std::vector<char> readFile(const std::string &filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
};

#endif //_VKS_UTIL_H_
