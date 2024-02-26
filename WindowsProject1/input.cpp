#include "input.h"

HRESULT Input::Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
    HRESULT hr = S_OK;

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);

    if (SUCCEEDED(hr)) {
        hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
    }

    if (SUCCEEDED(hr)) {
        hr = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
    }

    if (SUCCEEDED(hr)) {
        hr = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    }

    if (SUCCEEDED(hr)) {
        hr = m_keyboard->Acquire();
    }

    if (SUCCEEDED(hr)) {
        hr = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
    }

    if (SUCCEEDED(hr)) {
        hr = m_mouse->SetDataFormat(&c_dfDIMouse);
    }

    if (SUCCEEDED(hr)) {
        hr = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    }

    if (SUCCEEDED(hr)) {
        hr = m_mouse->Acquire();
    }

    return hr;

}

void Input::Resize(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

void Input::Realese() {
    if (m_mouse) {
        m_mouse->Unacquire();
        m_mouse->Release();
        m_mouse = nullptr;
    }

    if (m_keyboard) {
        m_keyboard->Unacquire();
        m_keyboard->Release();
        m_keyboard = 0;
    }

    if (m_directInput) {
        m_directInput->Release();
        m_directInput = 0;
    }
}

bool Input::Frame() {
    bool result;

    result = ReadKeyboard();
    if (!result)
        return false;

    result = ReadMouse();
    if (!result)
        return false;

    return true;
}

bool Input::ReadKeyboard() {
    HRESULT result;

    result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
    if (FAILED(result)) {
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            m_keyboard->Acquire();
        else
            return false;
    }

    return true;
}


bool Input::ReadMouse() {
    HRESULT result;

    result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
    if (FAILED(result)) {
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            m_mouse->Acquire();
        else
            return false;
    }

    return true;
}

XMFLOAT3 Input::IsMouseUsed() {
    if (m_mouseState.rgbButtons[0] || m_mouseState.rgbButtons[1] || m_mouseState.rgbButtons[2] & 0x80)
        return XMFLOAT3((float)m_mouseState.lX, (float)m_mouseState.lY, (float)m_mouseState.lZ);

    return XMFLOAT3(0.0f, 0.0f, 0.0f);
};