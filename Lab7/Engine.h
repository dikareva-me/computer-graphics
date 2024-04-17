#pragma once

#include "RenderWindow.h"
#include "Graphics.h"

#include <memory>

class Engine
{
public:
	static Engine& getInstance();
	
	bool Init(const HINSTANCE& hInstance, const int& width, const int& height, const std::wstring& windowClass, const std::wstring& title, int nCmdShow);
	bool ProcessMessages();
	void Update();
	void WindowResize(const int& width, const int& height);
	void WriteFile(int numFile, const std::vector<double> avgs, const std::vector<double> mins, const std::vector<double> maxs);
private:
	Engine() = default;
	Engine(const Engine&) = delete;
	Engine& operator=(Engine&) = delete;
private:
	bool pressedKey = false;
	std::unique_ptr<RenderWindow> window;
	std::unique_ptr<Graphics> graphics;
};

