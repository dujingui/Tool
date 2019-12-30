#include "pch.h"
#include <iostream>
#include "Unpacker.h"

int main()
{
	int alpha = 0;

	std::cout << "在当前目录创建origin文件夹，存放整图,再创建unpacket文件夹用来存放输出的碎图\n";

	std::cout << "请输入Alpha值:";

	std::cin >> alpha;

	unpacker(alpha);

	system("pause");
}
