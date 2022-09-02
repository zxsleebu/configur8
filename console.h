#define WIN32_LEAN_AND_MEAN
#pragma once
#include <windows.h>
#include <stdio.h>
#include <iostream>
namespace Console {
	using namespace std;
	template <typename ... Arguments>
	void print(const char* format, const char* sign, WORD color, Arguments ... args) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
		cout << "[" << sign << "] ";
		printf(format, args...);
		cout << endl;
		SetConsoleTextAttribute(hConsole, 7);
	}
	template <typename ... Arguments>
	void info(const char* format, Arguments ... args) {
		print(format, "i", 0xF, args...);
	}
	template <typename ... Arguments>
	void success(const char* format, Arguments ... args) {
		print(format, "+", 0xA, args...);
	}
	template <typename ... Arguments>
	void error(const char* format, Arguments ... args) {
		print(format, "-", 0xC, args...);
	}
	template <typename ... Arguments>
	void misc(const char* format, Arguments ... args) {
		print(format, ".", 0x7, args...);
	}
}