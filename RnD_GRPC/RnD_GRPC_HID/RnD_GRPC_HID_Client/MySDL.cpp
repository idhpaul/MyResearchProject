#include "MySDL.h"

#include <assert.h>

#include <iostream>
#include <functional>
#include <vector>

#include "MSLM_HID_API.h"

MySDL::MySDL()
    :windowWidth_(1280), windowHeight_(720), textureWidth_(1920), textureHeight_(1080)
{
    SetFunctionPointList();

    RegisterHook();

}

MySDL::MySDL(uint32_t wWidth, uint32_t wHeight, uint32_t tWidth, uint32_t tHeight)
    :windowWidth_(wWidth), windowHeight_(wHeight), textureWidth_(tWidth), textureHeight_(tHeight)
{
    SetFunctionPointList();

    RegisterHook();
}

MySDL::~MySDL()
{
    UnRegisterHook();

    Destory();

    SDL_Quit();
}

bool MySDL::Create()
{
    std::call_once(sdlinit_, [=]() {
        SDL_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
        });

    // @note : SDL_WINDOW_INPUT_GRABBED 커서 창 가두기
    int window_flag = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS /*| SDL_WINDOW_INPUT_GRABBED*/;

    window_ = SDL_CreateWindow("RX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth_, windowHeight_, window_flag);
    assert(window_ != NULL);


    int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer_ = SDL_CreateRenderer(window_, -1, renderer_flags);
    assert(renderer_ != NULL);

    SDL_SetRenderDrawColor(renderer_, 114, 144, 154, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);

    SDL_SetWindowMinimumSize(window_, 640, 480);

    isInit_=true;

    return true;
}

bool MySDL::Destory()
{
    isInit_ = false;

    if (renderer_) {

        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_) {

        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    return true;
}

bool MySDL::Process()
{
    int reslut = 0;
    while (reslut >= 0)
    {
        if (SDL_WaitEvent(&sdlEvent_))
        {
            reslut = MyFunctionCaller(sdlEvent_.type, sdlEvent_);
        }
        else
        {
            std::cout << "SDL Wait Event failed" << std::endl;
            return false;
        }
    }

    return true;
}

bool MySDL::IsIntialized()
{
    return isInit_;
}

void MySDL::Create_Injector(const std::string& ip, const std::string& port)
{
    std::string connectinfo = ip + ":" + port;
    injectorClient = std::make_unique<InjectClient>(grpc::CreateChannel(connectinfo, grpc::InsecureChannelCredentials()));

    // Need Mouse cursor callback register
    cb_create_colorcursor = std::bind(&MySDL::CreateColorCursor, this, std::placeholders::_1);
    cb_destroy_colorcursor = std::bind(&MySDL::DestroyColorCursor, this);

    injectorClient->RegisterCreateCursorCB(cb_create_colorcursor);
    injectorClient->RegisterDestroyCursorCB(cb_destroy_colorcursor);

}

void MySDL::Destory_Injector()
{
    
}

void MySDL::SetFunctionPointList()
{
    myFunctionList_[0][0] = &MySDL::MyQuitFunction;

    myFunctionList_[1][0] = &MySDL::MyWindowFunction;
    myFunctionList_[1][1] = &MySDL::MySystemFuctnion;

    myFunctionList_[2][0] = &MySDL::MyKeyDownFunction;
    myFunctionList_[2][1] = &MySDL::MyKeyUPFunction;                

    myFunctionList_[3][0] = &MySDL::MyMouseMotionFunction;
    myFunctionList_[3][1] = &MySDL::MyMouseButtonDownFunction;
    myFunctionList_[3][2] = &MySDL::MyMouseButtonUpFunction;
    myFunctionList_[3][3] = &MySDL::MyMouseWheelFunction;      


    myFunctionList_[4][0] = &MySDL::MyUserFunction;
}

int MySDL::MyFunctionCaller(const uint32_t type, const SDL_Event& event)
{
    unsigned int groupIndex = type >> 8;
    if (groupIndex & 0b1000'0000)
        groupIndex = 4;                             // for SDL User Event

    if (groupIndex > 4)                             // Boundary out
        return 0;

    unsigned int subIndex = type & 0b0000'1111;     // C++14 binary literal type

    if (!myFunctionList_[groupIndex - 1][subIndex])
        return 0;

    return (this->*myFunctionList_[groupIndex - 1][subIndex])(event);
}

int MySDL::MyQuitFunction(const SDL_Event& sdlEvent)
{
    std::cout << "Quit func called" << std::endl;
    return -1;
}

int MySDL::MyWindowFunction(const SDL_Event& sdlEvent)
{
    switch (sdlEvent.window.event) {
    case SDL_WINDOWEVENT_SHOWN:
        SDL_Log("Window %d shown", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_HIDDEN:
        SDL_Log("Window %d hidden", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_EXPOSED:
        SDL_Log("Window %d exposed", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_MOVED:
        SDL_Log("Window %d moved to %d,%d", sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2);
        break;
    case SDL_WINDOWEVENT_RESIZED:
        SDL_Log("Window %d resized to %dx%d", sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2);
        break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        SDL_Log("Window %d size changed to %dx%d", sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2);
        break;
    case SDL_WINDOWEVENT_MINIMIZED:
        SDL_Log("Window %d minimized", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_MAXIMIZED:
        SDL_Log("Window %d maximized", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_RESTORED:
        SDL_Log("Window %d restored", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_ENTER:
        SDL_Log("Mouse entered window %d", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_LEAVE:
        SDL_Log("Mouse left window %d", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        SDL_Log("Window %d gained keyboard focus", sdlEvent.window.windowID);
        isKeyboardFocus = true;
        break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
        SDL_Log("Window %d lost keyboard focus", sdlEvent.window.windowID);
        isKeyboardFocus = false;;
        break;
    case SDL_WINDOWEVENT_CLOSE:
        SDL_Log("Window %d closed", sdlEvent.window.windowID);
        break;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
        SDL_Log("Window %d is offered a focus", sdlEvent.window.windowID);
        //_IsSDLKeyboardFocus = true;
        break;
    case SDL_WINDOWEVENT_HIT_TEST:
        SDL_Log("Window %d has a special hit test", sdlEvent.window.windowID);
        break;
    default:
        SDL_Log("Window %d got unknown event %d",
            sdlEvent.window.windowID, sdlEvent.window.event);
        break;
    }
    return 0;
}

int MySDL::MySystemFuctnion(const SDL_Event& sdlEvent)
{
    return 0;
}

int MySDL::MyKeyDownFunction(const SDL_Event& sdlEvent)
{
    std::cout << "SDL Get Key Down" << std::endl;
    printf("dll key code : %x\n", sdlEvent.key.keysym.unused);
    printf("dll key extend : %x\n", sdlEvent.key.keysym.mod);

    uint32_t kbmode = 0;
    kbmode |= 0b0000'0001'0000'0000;                // Key down
    if (sdlEvent.key.keysym.mod)
        kbmode |= 0b0000'0000'0000'0001;            // extend key

    injectorClient->PushKeyboard(sdlEvent.key.keysym.unused, kbmode);


    if ((sdlEvent.key.keysym.mod & KMOD_CTRL) &&
        (sdlEvent.key.keysym.mod & KMOD_ALT) &&
        (sdlEvent.key.keysym.mod & KMOD_SHIFT))
    {

        // Check for quit combo (Ctrl+Alt+Shift+Q)
        if (sdlEvent.key.keysym.sym == SDLK_RETURN) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Detected Change Screen Mode");

            if (isFullScreen)
            {
                SDL_SetWindowFullscreen(window_, 0);
                isFullScreen = false;
            }
            else
            {
                SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
                isFullScreen = true;
            }
        }
    }

    if ((sdlEvent.key.keysym.mod & KMOD_CTRL) &&
        (sdlEvent.key.keysym.mod & KMOD_ALT) &&
        (sdlEvent.key.keysym.mod & KMOD_SHIFT))
    {

        // Check for quit combo (Ctrl+Alt+Shift+Q)
        if (sdlEvent.key.keysym.sym == SDLK_SPACE) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Detected Change Grab");

            if (isGrab)
            {
                SDL_SetWindowGrab(window_, SDL_FALSE);
                isGrab = false;
            }
            else
            {
                SDL_SetWindowGrab(window_, SDL_TRUE);
                isGrab = true;
            }
        }
    }
    return 0;
}

int MySDL::MyKeyUPFunction(const SDL_Event& sdlEvent)
{
    std::cout << "Key Up" << std::endl;

    printf("dll key code : %x\n", sdlEvent.key.keysym.unused);
    printf("dll key extend : %x\n", sdlEvent.key.keysym.mod);

    uint32_t kbmode = 0;
    if (sdlEvent.key.keysym.mod)
        kbmode |= 0b0000'0000'0000'0001;          // extend key

    injectorClient->PushKeyboard(sdlEvent.key.keysym.unused, kbmode);

    return 0;
}

int MySDL::MyMouseMotionFunction(const SDL_Event& sdlEvent)
{

    int relativew = 0, relativeh = 0;
    GetWindowSize(&relativew, &relativeh);

    std::cout << "Mouse move : " << (sdlEvent.motion.x * textureWidth_) / relativew << "x" << (sdlEvent.motion.y * textureHeight_) / relativeh << std::endl;

    //Push Grpc protobuff message(async)
    injectorClient->PushMouse(
        MouseProceedType::MOUSE_MOTION,
        NONE,
        (sdlEvent.motion.x * textureWidth_) / relativew,
        (sdlEvent.motion.y * textureHeight_) / relativeh
    );

    return 0;
}

int MySDL::MyMouseButtonDownFunction(const SDL_Event& sdlEvent)
{
    int relativew = 0, relativeh = 0;
    GetWindowSize(&relativew, &relativeh);
    
    std::cout << "Mouse bt Down" << std::endl;

    //Push Grpc protobuff message(async)
    injectorClient->PushMouse(
        MouseProceedType::MOUSE_DOWN,
        MouseButtonType(sdlEvent.button.button),
        0,
        0
    );

    return 0;
}

int MySDL::MyMouseButtonUpFunction(const SDL_Event& sdlEvent)
{
    int relativew = 0, relativeh = 0;
    GetWindowSize(&relativew, &relativeh);
    
    std::cout << "Mouse bt Up" << std::endl;

    //Push Grpc protobuff message(async)
    injectorClient->PushMouse(
        MouseProceedType::MOUSE_UP,
        MouseButtonType(sdlEvent.button.button),
        0,
        0
    );

    return 0;
}

int MySDL::MyMouseWheelFunction(const SDL_Event& sdlEvent)
{
    int relativew = 0, relativeh = 0;
    GetWindowSize(&relativew, &relativeh);
    
    if (sdlEvent.wheel.y > 0)
        std::cout << "Mouse wheel Up" << std::endl;
    else
        std::cout << "Mouse wheel Down" << std::endl;

    //Push Grpc protobuff message(async)
    injectorClient->PushMouse(
        MouseProceedType::MOUSE_WHEEL,
        MouseButtonType::NONE,
        sdlEvent_.button.x,
        sdlEvent_.button.y
    );

    return 0;
}

int MySDL::MyUserFunction(const SDL_Event& sdlEvent)
{
    std::cout << "User called" << std::endl;

    return 0;
}

void MySDL::RegisterHook()
{
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);

    struct tm tm;
    localtime_s(&tm, &tt);

    MSLM_Start_Keyboard((void*)&sdlEvent_, &isKeyboardFocus);
}

void MySDL::UnRegisterHook()
{
    // End MSLM Keyboard Hook
    MSLM_Stop_Keyboard();
}

inline void MySDL::GetWindowSize(int* width, int* hegith)
{
    SDL_GetWindowSize(window_, width, hegith);
}

void MySDL::CreateColorCursor(CursorData data)
{
    cursor_surface_ = SDL_CreateRGBSurfaceWithFormatFrom((void*)data.cursor_image.c_str(), data.width, data.height, data.width, data.width * 4, SDL_PIXELFORMAT_BGRA32);
    if (cursor_surface_ == NULL)
    {
        std::cerr << "Create RGB surface failed GLE: " << SDL_GetError() << std::endl;
    }

    // Create Color Cursor
    cursor_ = SDL_CreateColorCursor(cursor_surface_, data.xHotspot, data.yHotspot);
    if (cursor_ == NULL)
    {
        std::cerr << "Create Color Cursor failed GLE: " << SDL_GetError() << std::endl;
        std::cerr << "Get last error : " << GetLastError() << std::endl;
    }

    // Set Color Cursor
    SDL_SetCursor(cursor_);
}

void MySDL::DestroyColorCursor()
{
    if (cursor_surface_)
    {
        SDL_FreeSurface(cursor_surface_);
        cursor_surface_ = nullptr;
    }
    
    if (cursor_)
    {
        SDL_FreeCursor(cursor_);
        cursor_ = nullptr;
    }
    
}
