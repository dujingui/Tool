#include "pch.h"
#include <iostream>
#include "Unpacker.h"

int main()
{
	int alpha = 0;

	std::cout << "�ڵ�ǰĿ¼����origin�ļ��У������ͼ,�ٴ���unpacket�ļ�����������������ͼ\n";

	std::cout << "������Alphaֵ:";

	std::cin >> alpha;

	unpacker(alpha);

	system("pause");
}
