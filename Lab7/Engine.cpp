#include "Engine.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <chrono>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>


Engine& Engine::getInstance()
{
    static Engine engine;
    return engine;
}

bool Engine::Init(const HINSTANCE& hInstance, const int& width, const int& height, const std::wstring& windowClass, const std::wstring& title, int nCmdShow)
{
    window = std::make_unique<RenderWindow>(hInstance, width, height, windowClass, title);
    graphics = std::make_unique<Graphics>();

    if (!window->Init(nCmdShow))
    {
        return false;
    }

    if (!graphics->InitDirectX(window->getHwnd(), window->getWidth(), window->getHeight()))
    {
        return false;
    }

    if (!graphics->InitShaders())
    {
        return false;
    }

    if (!graphics->InitScene())
    {
        return false;
    }

    graphics->InitImGUI(window->getHwnd());

    return true;
}

bool Engine::ProcessMessages()
{
    return window->ProcessMessages();
}

void Engine::Update()
{
    while (!Keyboard::getInstance().KeyBufferIsEmpty())
    {
        KeyboardEvent kbe = Keyboard::getInstance().ReadKey();
        unsigned char keycode = kbe.GetKeyCode();
    }

    while (!Mouse::getInstance().EventBufferIsEmpty())
	{
		MouseEvent me = Mouse::getInstance().ReadEvent();

        if (Mouse::getInstance().IsRightDown())
        {
            if (me.GetType() == MouseEvent::EventType::Move)
            {
                graphics->GetCamera().AdjustRotation(DirectX::XMVectorSet((float)Mouse::getInstance().GetPosChange().y * 0.01f, (float)Mouse::getInstance().GetPosChange().x * 0.01f, 0, 1.0f));
            }
        }
	}

	const float cameraSpeed = 0.01f;

	if (Keyboard::getInstance().KeyIsPressed('W'))
	{
		graphics->GetCamera().AdjustPosition(DirectX::XMVectorScale(graphics->GetCamera().GetForwardVector(), cameraSpeed));
	}
	if (Keyboard::getInstance().KeyIsPressed('S'))
	{
		graphics->GetCamera().AdjustPosition(DirectX::XMVectorScale(graphics->GetCamera().GetBackwardVector(), cameraSpeed));
	}
	if (Keyboard::getInstance().KeyIsPressed('A'))
	{
		graphics->GetCamera().AdjustPosition(DirectX::XMVectorScale(graphics->GetCamera().GetLeftVector(), cameraSpeed));
	}
	if (Keyboard::getInstance().KeyIsPressed('D'))
	{
		graphics->GetCamera().AdjustPosition(DirectX::XMVectorScale(graphics->GetCamera().GetRightVector(), cameraSpeed));
	}

    if (Keyboard::getInstance().KeyIsPressed(VK_SPACE))
    {
        graphics->GetCamera().AdjustPosition(DirectX::XMVectorSet(0.0f, cameraSpeed, 0.0f, 1.0f));
    }
    if (Keyboard::getInstance().KeyIsPressed('Z'))
    {
        graphics->GetCamera().AdjustPosition(DirectX::XMVectorSet(0.0f, -cameraSpeed, 0.0f, 1.0f));
    }

    if (Keyboard::getInstance().KeyIsPressed('C'))
    {
        if (!pressedKey)
        {
            graphics->setDebugFrustum(); 
            pressedKey = true;
        }
    }
    else
    {
        pressedKey = false;
    }
    if (graphics->cubeInstances.startAnalyzing)
    {
        for (int mode = 0; mode < 3; mode++)
        {
            std::vector<double> avgs;
            std::vector<double> mins;
            std::vector<double> maxs;

            for (int i = 100; i < 1100; i += 10)
            {
                std::vector <std::chrono::duration<double>> times;
                graphics->cubeInstances.numInstances = i;
                graphics->cubeInstances.drawMode = mode + 1;
                auto start = std::chrono::system_clock::now();
                while (std::chrono::system_clock::now() - start < std::chrono::seconds(3))
                {
                    auto timeRender = std::chrono::system_clock::now();
                    graphics->RenderFrame();
                    auto deltaTime = std::chrono::system_clock::now() - timeRender;
                    times.push_back(std::chrono::duration<double>(deltaTime));
                }
                double average = std::accumulate(times.begin(), times.end(), std::chrono::duration<double>(0)).count() / times.size();


                auto minMax = std::minmax_element(times.begin(), times.end());
                auto minDuration = *minMax.first;
                auto maxDuration = *minMax.second;
                double minSeconds = minDuration.count();
                double maxSeconds = maxDuration.count();

                avgs.emplace_back(average);
                mins.emplace_back(minSeconds);
                maxs.emplace_back(maxSeconds);
            }

            WriteFile(mode, avgs, mins, maxs);
        }

    
    }
    else graphics->RenderFrame();
}

void Engine::WriteFile(int numFile, const std::vector<double> avgs, const std::vector<double> mins, const std::vector<double> maxs)
{
    std::string filename;
    if (numFile == 0)
        filename = "Indexed.csv";
    else if (numFile == 1)
        filename = "Lab7.csv";
    else if (numFile == 2)
        filename = "Lab9.csv";
    else
        return;
    
    std::ofstream file(filename, std::ios::out);

    if (file.is_open())
    {
        for (const auto& item : avgs)
        {
            file << item << ";";
        }
        file << '\n';
        for (const auto& item : mins)
        {
            file << item << ";";
        }
        file << '\n';
        for (const auto& item : maxs)
        {
            file << item << ";";
        }
        file << '\n';

        file.close();
    }

}

void Engine::WindowResize(const int& width, const int& height)
{
    window->Resize(width, height);
    graphics->Resize(width, height);
}
