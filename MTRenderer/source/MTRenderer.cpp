/// @file MTRenderer.cpp
/// @author Masayoshi Kamai

#include "stdafx.h"
#include "MTRenderer.h"

using namespace DirectX;

//----------------------------------------------------------------------------------------------------
// SceneActor
//----------------------------------------------------------------------------------------------------
/// @brief �R���X�g���N�^
SceneActor::SceneActor(SceneActorType inType)
: type(inType)
{
    translation = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    rotation    = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    scale       = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
}

/// @brief �f�X�g���N�^
SceneActor::~SceneActor() {
    ;
}

//----------------------------------------------------------------------------------------------------
// TriangleSceneActor
//----------------------------------------------------------------------------------------------------
/// @brief �R���X�g���N�^
TriangleSceneActor::TriangleSceneActor()
: SceneActor(SceneActorType::Triangle)
, rotSpeed(1.0f)
, scaleSpeed(1.0f)
, rotAngle(0.0f)
, scaleAngle(0.0f)
{
    ;
}

/// @brief �f�X�g���N�^
TriangleSceneActor::~TriangleSceneActor() {
    ;
}

/// @brief �X�V����
void TriangleSceneActor::Update(float delta) {
    // �w�葬�x��Z����]
    rotAngle += 360.0f * GetRotSpeed() * delta;
    while (360.0f < rotAngle) {
        rotAngle -= 360.0f;
    }
    rotation = XMVectorSetZ(rotation, rotAngle);

    // 0.5����2.5�{�Ŋg��k��
    scaleAngle += 2.0f * XM_PI * scaleSpeed * delta;
    while ((2.0f * XM_PI) < scaleAngle) {
        scaleAngle -= 2.0f * XM_PI;
    }
    float s = std::sinf(scaleAngle) + 1.5f;
    scale = XMVectorSet(s, s, s, 0.0f);
}

//----------------------------------------------------------------------------------------------------
// CameraSceneActor
//----------------------------------------------------------------------------------------------------
/// @brief �R���X�g���N�^
CameraSceneActor::CameraSceneActor()
: SceneActor(SceneActorType::Camera) 
, screenWidth(0)
, screenHeight(0)
, fovInDeg(0.0f)
{
    translation = XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);
    rotation    = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    scale       = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

    cameraEye = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    cameraAt  = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    cameraUp  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}

/// @brief �f�X�g���N�^
CameraSceneActor::~CameraSceneActor() {
    ;
}

/// @brief �X�V����
void CameraSceneActor::Update(float delta) {
    // �J���������p�����[�^
    // �ʒu:   (0.0f, 0.0f, 0.0f)
    // �O����: (0.0f, 0.0f, 1.0f)
    // �����: (0.0f, 1.0f, 0.0f)
    // �Ƃ��ĉ�]��K�p
    const float d2r = XM_PI / 180.0f;
    XMMATRIX rotMtx = XMMatrixRotationRollPitchYawFromVector(XMVectorMultiply(rotation, XMVectorSet(d2r, d2r, d2r, 0.0f)));
    XMVECTOR dirVec = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotMtx);
    XMVECTOR upVec  = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotMtx);

    cameraEye = translation;
    cameraAt  = XMVectorAdd(cameraEye, dirVec);
    cameraUp  = upVec;
}

/// @brief ������
bool CameraSceneActor::Init(const UINT width, const UINT height, const float degFov) {
    screenWidth  = width;
    screenHeight = height;
    fovInDeg     = degFov;

    return true;
}

//----------------------------------------------------------------------------------------------------
// SceneProxy
//----------------------------------------------------------------------------------------------------
/// @brief �R���X�g���N�^
SceneProxy::SceneProxy(SceneActorType inType)
: type(inType)
{
    ;
}

/// @brief �f�X�g���N�^
SceneProxy::~SceneProxy() {
    ;
}

//----------------------------------------------------------------------------------------------------
// TriangleSceneProxy
//----------------------------------------------------------------------------------------------------
/// @brief �R���X�g���N�^
TriangleSceneProxy::TriangleSceneProxy()
: SceneProxy(SceneActorType::Triangle)
{
    ;
}

/// @brief �f�X�g���N�^
TriangleSceneProxy::~TriangleSceneProxy() {
    ;
}

/// @brief Actor����Proxy�֕`�����`�B
void TriangleSceneProxy::Commit(SceneActor *actor) {
    auto triSceneActor = static_cast<TriangleSceneActor*>(actor);

    // World�ϊ��s��Z�o
    const float d2r = XM_PI / 180.0f;
    XMMATRIX scaleMtx = XMMatrixScalingFromVector(triSceneActor->scale);
    XMMATRIX rotMtx   = XMMatrixRotationRollPitchYawFromVector(XMVectorMultiply(triSceneActor->rotation, XMVectorSet(d2r, d2r, d2r, 0.0f)));
    XMMATRIX transMtx = XMMatrixTranslationFromVector(triSceneActor->translation);
    worldMtx = XMMatrixMultiply(XMMatrixMultiply(scaleMtx, rotMtx), transMtx);
}

//----------------------------------------------------------------------------------------------------
// CameraSceneProxy
//----------------------------------------------------------------------------------------------------
/// @brief �R���X�g���N�^
CameraSceneProxy::CameraSceneProxy()
: SceneProxy(SceneActorType::Camera)
{
    ;
}

/// @brief �f�X�g���N�^
CameraSceneProxy::~CameraSceneProxy() {
    ;
}

/// @brief Actor����Proxy�֕`�����`�B
void CameraSceneProxy::Commit(SceneActor *actor) {
    auto cameraSceneActor = static_cast<CameraSceneActor*>(actor);

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width    = static_cast<float>(cameraSceneActor->screenWidth);
    viewport.Height   = static_cast<float>(cameraSceneActor->screenHeight);
    viewport.MinDepth = D3D12_MIN_DEPTH;
    viewport.MaxDepth = D3D12_MAX_DEPTH;

    scissorRect.left   = 0;
    scissorRect.top    = 0;
    scissorRect.right  = cameraSceneActor->screenWidth;
    scissorRect.bottom = cameraSceneActor->screenHeight;

    const float aspectRatio = static_cast<float>(cameraSceneActor->screenWidth) / static_cast<float>(cameraSceneActor->screenHeight);
    const float fovAngleY = cameraSceneActor->fovInDeg * XM_PI / 180.0f;

    viewMtx = XMMatrixTranspose(XMMatrixLookAtLH(cameraSceneActor->cameraEye, cameraSceneActor->cameraAt, cameraSceneActor->cameraUp));
    projMtx = XMMatrixTranspose(XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 1000.0f));
}


//----------------------------------------------------------------------------------------------------
// MTRenderer
//----------------------------------------------------------------------------------------------------
/// @brief ���C���E�B���h�E�������b�Z�[�W�n���h��
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        break;

    case WM_KEYDOWN:
        break;

    case WM_KEYUP:
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        ;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

/// @brief �R���X�g���N�^
MTRenderer::MTRenderer(HINSTANCE hInstance, int nCmdShow)
: renderTheadReady(false)
, mainTheadReady(false)
, canvasWidth(DEFAULT_CANVAS_WIDTH)
, canvasHeight(DEFAULT_CANVAS_HEIGHT)
, instanceHandle(hInstance)
, commandShowFlags(nCmdShow)
, windowHandle(nullptr)
, flags(0)
, backBufferIndex(0)
, backBufferCount(0)
, fenceEvent(nullptr)
{
    ;
}

/// @brief �f�X�g���N�^
MTRenderer::~MTRenderer() {
    ;
}

/// @brief ����������
bool MTRenderer::Init() {
    return Init(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT, DEFAULT_FRAME_COUNT, L"Multi-Threaded Renderer");
}

/// @brief ����������
bool MTRenderer::Init(const UINT width, const UINT height, const UINT inFrameCount, LPCWSTR titleStr) {
    canvasWidth  = width;
    canvasHeight = height;
    backBufferCount   = inFrameCount;

    // WindowClass������
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.hInstance      = instanceHandle;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName  = L"MTRenderer";
    if (!RegisterClassEx(&wcex)) {
        return false;
    }

    RECT windowRect = { 0, 0, static_cast<LONG>(canvasWidth), static_cast<LONG>(canvasHeight) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Window����
    windowHandle = CreateWindow(
        wcex.lpszClassName,
        titleStr,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        instanceHandle,
        nullptr);

    if (windowHandle == nullptr) {
        Deinit();
        return false;
    }

    if (!InitDevices()) {
        Deinit();
        return false;
    }

    if (!InitResources()) {
        Deinit();
        return false;
    }

    sceneActors.reserve(DEFAULT_SCENE_ACTOR_CAPACITY);
    sceneProxies.reserve(DEFAULT_SCENE_PROXY_CAPACITY);

    // �f�t�H���g�J������������
    defaultCameraActor.Init(width, height, 90.0f);
    AddSceneActor(&defaultCameraActor);

    // �f�t�H���g�̃A�N�^��������
    defaultTriangleActor.SetRotSpeed(1.0f);
    defaultTriangleActor.SetScaleSpeed(0.5f);
    AddSceneActor(&defaultTriangleActor);

    return true;
}

/// @brief �I������
void MTRenderer::Deinit() {
    SetFlag(GlobalFlag::TerminateRenderer);

    if (renderThread.joinable()) {
        renderThread.join();
    }
    if (mainThread.joinable()) {
        mainThread.join();
    }

    if (fenceEvent != nullptr) {
        CloseHandle(fenceEvent);
        fenceEvent = nullptr;
    }

    for (size_t i = 0; i < sceneProxies.size(); ++i) {
        auto proxy = sceneProxies[i];
        delete proxy;
        sceneProxies[i] = nullptr;
    }
}

/// @brief ���s
int MTRenderer::Run() {
    mainThread   = std::thread(MainThreadFunc, this);
    renderThread = std::thread(RenderThreadFunc, this);
    
    ShowWindow(windowHandle, commandShowFlags);
    UpdateWindow(windowHandle);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

/// @brief �w�肵���t���O���Z�b�g
void MTRenderer::SetFlag(const GlobalFlag flag) {
    flags |= static_cast<UINT>(flag);
}

/// @brief �w�肵���t���O���N���A
void MTRenderer::ClearFlag(const GlobalFlag flag) {
    flags &= ~static_cast<UINT>(flag);
}

/// @brief �w�肵���t���O���Z�b�g����Ă��邩���ׂ�
bool MTRenderer::TestFlag(const GlobalFlag flag) {
    return (flags & static_cast<UINT>(flag)) != 0;
}

/// @brief �A�N�^��ǉ�
void MTRenderer::AddSceneActor(SceneActor *actor) {
    if (actor == nullptr) {
        return;
    }

    sceneActors.push_back(actor);
}

namespace {
void SetThreadName(LPCSTR name, DWORD threadId) {
    const DWORD MS_VC_EXCEPTION = 0x406d1388;

#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO {
        DWORD dwType;        // Must be 0x1000.
        LPCSTR szName;       // Pointer to name (in user addr space).
        DWORD dwThreadID;    // Thread ID (-1 = caller thread).
        DWORD dwFlags;       // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    THREADNAME_INFO info;
    info.dwType     = 0x1000;
    info.szName     = name;
    info.dwThreadID = threadId;
    info.dwFlags    = 0;

    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        ;
    }
}
} // namespace ""

/// @brief ���C���X���b�h�����֐�
int MTRenderer::MainThreadFunc(MTRenderer *renderer) {
    SetThreadName("MainThread", GetThreadId(renderer->mainThread.native_handle()));

    float delta = 0.0f;

    while (!renderer->TestFlag(GlobalFlag::TerminateRenderer)) {
        auto beginTime = std::chrono::system_clock::now();

        // N�t���[���̎��O�X�V����
        renderer->PreUpdate();

        // N�t���[���̍X�V����
        renderer->Update(delta);

        // RenderThread��SceneProxy�󂯎��\�ɂȂ�܂ő҂�
        renderer->SyncRenderThread();

        // Actor����Proxy��N�t���[���̕`�����`�B
        renderer->CommitSceneProxy();

        // RehderThread�ĊJ
        renderer->RestartRenderThread();

        auto endTime = std::chrono::system_clock::now();
        delta = std::chrono::duration<float>(endTime - beginTime).count();
    }

    // �f�b�h���b�N���
    {
        {
            std::unique_lock<std::mutex> lock(renderer->syncMainThreadMtx);
            renderer->mainTheadReady = true;
        }
        renderer->syncMainThreadCond.notify_one();
    }

    return 0;
}

/// @brief �`��X���b�h�����֐�
int MTRenderer::RenderThreadFunc(MTRenderer *renderer) {
    SetThreadName("RenderThread", GetThreadId(renderer->renderThread.native_handle()));

    while (!renderer->TestFlag(GlobalFlag::TerminateRenderer)) {
        // MainThread��SceneProxy���󂯎��\�ɂȂ�������ʒm
        // N�t���[���̕`������󂯎��
        renderer->SyncMainThread();

        // N�t���[���̎��O�`�揈��
        renderer->PreRender();

        // N-2�t���[����GPU����������҂�
        renderer->SyncGPU();

        // ����������҂��A�摜���X�V
        renderer->Present();

        // N-1�t���[����CommandList���L�b�N
        renderer->PopulateCommandList();

        // N�t���[����`��
        renderer->Render();
    }

    // �f�b�h���b�N���
    {
        {
            std::unique_lock<std::mutex> lock(renderer->syncRenderThreadMtx);
            renderer->renderTheadReady = true;
        }
        renderer->syncRenderThreadCond.notify_one();
    }

    // �Ō��GPU�����̊�����҂�
    renderer->SyncGPU();

    return 0;
}

/// @brief �`��f�o�C�X�̏�����
bool MTRenderer::InitDevices() {

    UINT dxgiFactoryFlags = 0;

#if ENABLE_D3D12_DEBUG_INTERFACE
    {
        ID3D12Debug *debugInterface = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {
            d3dDebugInterface.Attach(debugInterface);
            d3dDebugInterface->EnableDebugLayer();

            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    // IDXGIFactory6�𐶐�
    {
        ComPtr<IDXGIFactory4> dxgiFactory4 = nullptr;
        if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)))) {
            return false;
        }

        IDXGIFactory6 *dxgiFactory6 = nullptr;
        if (FAILED(dxgiFactory4->QueryInterface(IID_PPV_ARGS(&dxgiFactory6)))) {
            return false;
        }
        dxgiFactory.Attach(dxgiFactory6);
    }

    // IDXGIAdapter1���擾
    {
        ComPtr<IDXGIAdapter1> dxgiAdapter1 = nullptr;

        DXGI_GPU_PREFERENCE gpuPreferences[] = {
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            DXGI_GPU_PREFERENCE_UNSPECIFIED
        };

        for (auto pref : gpuPreferences) {
            UINT adapterIdx = 0;
            while (dxgiFactory->EnumAdapterByGpuPreference(adapterIdx, pref, IID_PPV_ARGS(&dxgiAdapter1)) != DXGI_ERROR_NOT_FOUND) {
                DXGI_ADAPTER_DESC1 desc;
                dxgiAdapter1->GetDesc1(&desc);

                if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr))) {
                    break;
                }
                ++adapterIdx;
            }
            if (dxgiAdapter1.Get() != nullptr) {
                break;
            }
        }
        if (dxgiAdapter1.Get() == nullptr) {
            return false;
        }
        dxgiAdapter.Swap(dxgiAdapter1);
    }

    // ID3D12Device6�𐶐�
    {
        ComPtr<ID3D12Device> device;
        if (FAILED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)))) {
            return false;
        }

        if (FAILED(device.As(&d3dDevice))) {
            return false;
        }
    }

    // CommandQueue����
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Priority = 0;
        queueDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 0;

        if (FAILED(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3dCommandQueue)))) {
            return false;
        }

        d3dCommandQueue->SetName(L"DefaultCommandQueue");
    }

    // SwapChain����
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width              = canvasWidth;
        swapChainDesc.Height             = canvasHeight;
        swapChainDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo             = FALSE;
        swapChainDesc.SampleDesc.Count   = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount        = backBufferCount;
        swapChainDesc.Scaling            = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        ComPtr<IDXGISwapChain1> dxgiSwapChain1;
        if (FAILED(dxgiFactory->CreateSwapChainForHwnd(d3dCommandQueue.Get(), windowHandle, &swapChainDesc, nullptr, nullptr, &dxgiSwapChain1))) {
            return false;
        }

        if (FAILED(dxgiSwapChain1.As(&dxgiSwapChain))) {
            return false;
        }

        // Alt+Enter�̐؂�ւ����֎~
        dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);

        // �����̃t���[���ԍ����擾
        backBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
    }

    // DescriptorHeap����
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.NumDescriptors = backBufferCount;
        rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask       = 0;

        if (FAILED(d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&d3dRTVHeap)))) {
            return false;
        }

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.NumDescriptors = backBufferCount;
        dsvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvHeapDesc.NodeMask       = 0;

        if (FAILED(d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&d3dDSVHeap)))) {
            return false;
        }

        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
        cbvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        cbvHeapDesc.NumDescriptors = (DEFAULT_CBV_COUNT + DEFAULT_SRV_COUNT + DEFAULT_UAV_COUNT) * backBufferCount;
        cbvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask       = 0;

        if (FAILED(d3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&d3dCBVHeap)))) {
            return false;
        }

        D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
        cbvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        cbvHeapDesc.NumDescriptors = DEFAULT_SAMPLER_COUNT * backBufferCount;
        cbvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask       = 0;

        if (FAILED(d3dDevice->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&d3dSamplerHeap)))) {
            return false;
        }
    }

    // RenderTarget�擾
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3dRTVHeap->GetCPUDescriptorHandleForHeapStart();
        rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        for (UINT i = 0; i < backBufferCount; ++i) {
            if (FAILED(dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&d3dRenderTarget[i])))) {
                return false;
            }

            d3dDevice->CreateRenderTargetView(d3dRenderTarget[i].Get(), nullptr, rtvHandle);
            rtvHandle.ptr += static_cast<SIZE_T>(rtvDescriptorSize);
        }
    }
    return true;
}

/// @brief �e�탊�\�[�X��������
bool MTRenderer::InitResources() {
    // VertexBuffer, VertexBufferView����
    {
        const Vertex vertices[] =
        {
            { { +0.0f, +0.5773503f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { +0.5f, -0.2886751f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.5f, -0.2886751f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };

        D3D12_HEAP_PROPERTIES d3dHeapProp;
        d3dHeapProp.Type                 = D3D12_HEAP_TYPE_UPLOAD;
        d3dHeapProp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        d3dHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        d3dHeapProp.CreationNodeMask     = 1;
        d3dHeapProp.VisibleNodeMask      = 1;

        D3D12_RESOURCE_DESC d3dResDesc;
        d3dResDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
        d3dResDesc.Alignment          = 0;
        d3dResDesc.Width              = sizeof(vertices);
        d3dResDesc.Height             = 1;
        d3dResDesc.DepthOrArraySize   = 1;
        d3dResDesc.MipLevels          = 1;
        d3dResDesc.Format             = DXGI_FORMAT_UNKNOWN;
        d3dResDesc.SampleDesc.Count   = 1;
        d3dResDesc.SampleDesc.Quality = 0;
        d3dResDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        d3dResDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

        if (FAILED(d3dDevice->CreateCommittedResource(&d3dHeapProp, D3D12_HEAP_FLAG_NONE, &d3dResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vtxBuffer)))) {
            return false;
        }

        // ���_�f�[�^�]��
        void *dst = nullptr;
        D3D12_RANGE range = { 0, 0 };
        vtxBuffer->Map(0, &range, &dst);
        if (dst != nullptr) {
            memcpy(dst, vertices, sizeof(vertices));
        }
        vtxBuffer->Unmap(0, nullptr);

        vtxBufferView.BufferLocation = vtxBuffer->GetGPUVirtualAddress();
        vtxBufferView.StrideInBytes  = sizeof(Vertex);
        vtxBufferView.SizeInBytes    = sizeof(vertices);
    }

    // RootSignature����
    {
        D3D12_ROOT_PARAMETER rootParameters[1];
        rootParameters[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].Descriptor.ShaderRegister           = 0;
        rootParameters[0].Descriptor.RegisterSpace            = 0;
        rootParameters[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_VERTEX;

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.NumParameters     = 1;
        rootSignatureDesc.pParameters       = rootParameters;
        rootSignatureDesc.NumStaticSamplers = 0;
        rootSignatureDesc.pStaticSamplers   = nullptr;
        rootSignatureDesc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        
        ComPtr<ID3DBlob> sigBlob;
        ComPtr<ID3DBlob> errBlob;
        if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob))) {
            return false;
        }

        if (FAILED(d3dDevice->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&d3dRootSignature)))) {
            return false;
        }
    }

    // PipelineStateObject����
    {
        ComPtr<ID3DBlob> vsBlob;
        ComPtr<ID3DBlob> psBlob;

        // Shader������
        {
            UINT compileFlags = 0;

        #if defined(_DEBUG)
            compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        #endif

            WCHAR assetsPath[512];
            DWORD size = GetModuleFileName(nullptr, assetsPath, _countof(assetsPath));
            if (size == 0 || size == _countof(assetsPath)) {
                return false;
            }

            WCHAR *lastSlash = wcsrchr(assetsPath, L'\\');
            if (lastSlash != nullptr) {
                lastSlash[1] = L'\0';
            }
            
            std::wstring shaderFile = assetsPath;
            shaderFile += L"simple_shaders.hlsl";

            if (FAILED(D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vsBlob, nullptr))) {
                return false;
            }
            if (FAILED(D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &psBlob, nullptr))) {
                return false;
            }
        }

        // InputLayout��`
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature                  = d3dRootSignature.Get();
        psoDesc.VS.pShaderBytecode              = vsBlob->GetBufferPointer();
        psoDesc.VS.BytecodeLength               = vsBlob->GetBufferSize();
        psoDesc.PS.pShaderBytecode              = psBlob->GetBufferPointer();
        psoDesc.PS.BytecodeLength               = psBlob->GetBufferSize();
        psoDesc.SampleMask                      = UINT_MAX;
        psoDesc.InputLayout.pInputElementDescs  = inputElementDescs;
        psoDesc.InputLayout.NumElements         = sizeof(inputElementDescs) / sizeof(inputElementDescs[0]);
        psoDesc.IBStripCutValue                 = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
        psoDesc.PrimitiveTopologyType           = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets                = 1;
        psoDesc.RTVFormats[0]                   = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count                = 1;
        psoDesc.SampleDesc.Quality              = 0;
        psoDesc.NodeMask                        = 0;
        psoDesc.CachedPSO.pCachedBlob           = nullptr;
        psoDesc.CachedPSO.CachedBlobSizeInBytes = 0;
        psoDesc.Flags                           = D3D12_PIPELINE_STATE_FLAG_NONE;

        // Blend state
        psoDesc.BlendState.AlphaToCoverageEnable  = FALSE;
        psoDesc.BlendState.IndependentBlendEnable = FALSE;
        for (size_t i = 0; i < 8; ++i) {
            psoDesc.BlendState.RenderTarget[i].BlendEnable           = FALSE;
            psoDesc.BlendState.RenderTarget[i].LogicOpEnable         = FALSE;
            psoDesc.BlendState.RenderTarget[i].SrcBlend              = D3D12_BLEND_ONE;
            psoDesc.BlendState.RenderTarget[i].DestBlend             = D3D12_BLEND_ZERO;
            psoDesc.BlendState.RenderTarget[i].BlendOp               = D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[i].SrcBlendAlpha         = D3D12_BLEND_ONE;
            psoDesc.BlendState.RenderTarget[i].DestBlendAlpha        = D3D12_BLEND_ZERO;
            psoDesc.BlendState.RenderTarget[i].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[i].LogicOp               = D3D12_LOGIC_OP_NOOP;
            psoDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }

        // Depth stencil state
        psoDesc.DepthStencilState.DepthEnable                  = FALSE;
        psoDesc.DepthStencilState.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ZERO;
        psoDesc.DepthStencilState.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        psoDesc.DepthStencilState.StencilEnable                = FALSE;
        psoDesc.DepthStencilState.StencilReadMask              = 0xff;
        psoDesc.DepthStencilState.StencilWriteMask             = 0xff;
        psoDesc.DepthStencilState.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
        psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        psoDesc.DepthStencilState.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
        psoDesc.DepthStencilState.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_ALWAYS;
        psoDesc.DepthStencilState.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
        psoDesc.DepthStencilState.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
        psoDesc.DepthStencilState.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
        psoDesc.DepthStencilState.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_ALWAYS;

        // Rasterizer state
        psoDesc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode              = D3D12_CULL_MODE_BACK;
        psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
        psoDesc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
        psoDesc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        psoDesc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        psoDesc.RasterizerState.DepthClipEnable       = TRUE;
        psoDesc.RasterizerState.MultisampleEnable     = FALSE;
        psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
        psoDesc.RasterizerState.ForcedSampleCount     = 0;
        psoDesc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        if (FAILED(d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&d3dPipelineState)))) {
            return false;
        }
    }

    // FrameData�����A������
    {
        D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = d3dCBVHeap->GetCPUDescriptorHandleForHeapStart();

        for (UINT i = 0; i < backBufferCount; ++i) {
            auto &frameData = frameDataArray[i];

            // CommandAllocator����
            if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameData.d3dCommandAllocator)))) {
                return false;
            }

            // CommandList����
            {
                if (FAILED(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameData.d3dCommandAllocator.Get(), d3dPipelineState.Get(), IID_PPV_ARGS(&frameData.d3dCommandList)))) {
                    return false;
                }

                // �������u�L�^���v��ԂȂ̂Œ���ɕ��Ă���
                frameData.d3dCommandList->Close();
            }

            // Fence����
            {
                if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameData.d3dFence)))) {
                    return false;
                }

                // ��r�p�t�F���X�l��������
                frameData.fenceValue = 1;
            }

            // ConstantBuffer����
            {
                D3D12_HEAP_PROPERTIES d3dHeapProp;
                d3dHeapProp.Type                 = D3D12_HEAP_TYPE_UPLOAD;
                d3dHeapProp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                d3dHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
                d3dHeapProp.CreationNodeMask     = 1;
                d3dHeapProp.VisibleNodeMask      = 1;

                D3D12_RESOURCE_DESC d3dResDesc;
                d3dResDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
                d3dResDesc.Alignment          = 0;
                d3dResDesc.Width              = sizeof(SceneConstantBuffer);
                d3dResDesc.Height             = 1;
                d3dResDesc.DepthOrArraySize   = 1;
                d3dResDesc.MipLevels          = 1;
                d3dResDesc.Format             = DXGI_FORMAT_UNKNOWN;
                d3dResDesc.SampleDesc.Count   = 1;
                d3dResDesc.SampleDesc.Quality = 0;
                d3dResDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
                d3dResDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

                if (FAILED(d3dDevice->CreateCommittedResource(&d3dHeapProp, D3D12_HEAP_FLAG_NONE, &d3dResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&frameData.d3dConstantBuffer)))) {
                    return false;
                }

                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
                cbvDesc.BufferLocation = frameData.d3dConstantBuffer->GetGPUVirtualAddress();
                cbvDesc.SizeInBytes    = sizeof(SceneConstantBuffer);

                d3dDevice->CreateConstantBufferView(&cbvDesc, cbvHandle);
                cbvHandle.ptr += d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
        }
    }

    // GPU����������҂��߂̃C�x���g�𐶐�
    fenceEvent = CreateEvent(nullptr, FALSE /*ManualReset*/, FALSE /*InitialState*/, L"GPUSyncFence");
    if (fenceEvent == nullptr) {
        return false;
    }

    return true;
}

// ���O�X�V����
void MTRenderer::PreUpdate() {
    ;
}

// �X�V����
void MTRenderer::Update(float delta) {
    for (auto actor : sceneActors) {
        actor->Update(delta);
    }
}

// RenderThread��SceneProxy�󂯎��\�ɂȂ�܂ő҂�
void MTRenderer::SyncRenderThread() {
    std::unique_lock<std::mutex> lock(syncRenderThreadMtx);

    // RenderThread����ʒm������܂ő҂�
    syncRenderThreadCond.wait(lock, [this] {
        return renderTheadReady;
    });

    // ���Z�b�g
    renderTheadReady = false;
}

// Actor����Proxy�֕`�����`�B
void MTRenderer::CommitSceneProxy() {
    if (sceneProxies.size() < sceneActors.size()) {
        for (size_t i = sceneProxies.size(); i < sceneActors.size(); ++i) {
            auto actor = sceneActors[i];

            switch (actor->type) {
            case SceneActorType::Camera:
                sceneProxies.push_back(new CameraSceneProxy);
                break;

            case SceneActorType::Triangle:
                sceneProxies.push_back(new TriangleSceneProxy);
                break;

            default:
                ;
            }
        }
    }

    assert(sceneActors.size() == sceneProxies.size());

    for (size_t i = 0; i < sceneProxies.size(); ++i) {
        auto proxy = sceneProxies[i];
        proxy->Commit(sceneActors[i]);
    }
}

void MTRenderer::RestartRenderThread() {
    {
        std::unique_lock<std::mutex> lock(syncMainThreadMtx);
        mainTheadReady = true;
    }

    // SceneProxy�̎󂯓n����������������RenderThread�֒ʒm
    syncMainThreadCond.notify_one();
}

void MTRenderer::SyncMainThread() {
    {
        std::unique_lock<std::mutex> lock(syncRenderThreadMtx);
        renderTheadReady = true;
    }

    // SceneProxy���󂯓���\�Ƃ�������MainThread�ʒm
    syncRenderThreadCond.notify_one();

    // SceneProxy
    {
        std::unique_lock<std::mutex> lock(syncMainThreadMtx);

        // SceneProxy�̎󂯓n������������܂ő҂�
        syncMainThreadCond.wait(lock, [this] {
            return mainTheadReady;
        });

        // ���Z�b�g
        mainTheadReady = false;
    }
}

// ���O�`�揈��
void MTRenderer::PreRender() {
    ;
}

// N-2�t���[����GPU����������҂�
void MTRenderer::SyncGPU() {
    auto &syncFrameData = frameDataArray[backBufferIndex];

    if (!syncFrameData.syncGPU) {
        return;
    }

    // N-2�t���[����GPU����������҂�
    if (syncFrameData.d3dFence->GetCompletedValue() < syncFrameData.fenceValue) {
        if (SUCCEEDED(syncFrameData.d3dFence->SetEventOnCompletion(syncFrameData.fenceValue, fenceEvent))) {
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    }
    syncFrameData.fenceValue++;
    syncFrameData.syncGPU = false;
}

// ����������҂��A�`����X�V
void MTRenderer::Present() {
    UINT prevBackBufferIndex = backBufferIndex;

    // ����������҂��ĕ`��C���[�W�X�V
    dxgiSwapChain->Present(1, 0);

    // �t���[���ԍ��X�V
    backBufferIndex = dxgiSwapChain->GetCurrentBackBufferIndex();
}

// N-1�t���[����CommandList���L�b�N
void MTRenderer::PopulateCommandList() {
    auto &frameData = frameDataArray[backBufferIndex];

    if (!frameData.syncGPU) {
        return;
    }

    // N-1�t���[���̕`��R�}���h���L�b�N
    ID3D12CommandList *d3dCommandLists[] = { frameData.d3dCommandList.Get() };
    d3dCommandQueue->ExecuteCommandLists(1, d3dCommandLists);
}

// N�t���[����`��
void MTRenderer::Render() {
    const UINT nextBackBufferIndex = (backBufferIndex + 1) % backBufferCount;
    auto &frameData = frameDataArray[nextBackBufferIndex];

    // �ȑO�L�^�Ɏg�p��������������������CommandAllocator�����Z�b�g
    frameData.d3dCommandAllocator->Reset();

    // �`��J�n�O��ComandList�����Z�b�g
    frameData.d3dCommandList->Reset(frameData.d3dCommandAllocator.Get(), d3dPipelineState.Get());

    frameData.d3dCommandList->SetGraphicsRootSignature(d3dRootSignature.Get());

    ID3D12DescriptorHeap *d3dDescHeaps[] = { d3dCBVHeap.Get() };
    frameData.d3dCommandList->SetDescriptorHeaps(1, d3dDescHeaps);

    // ConstantBuffer���o�C���h
    D3D12_GPU_VIRTUAL_ADDRESS cbLocation = frameData.d3dConstantBuffer->GetGPUVirtualAddress();
    frameData.d3dCommandList->SetGraphicsRootConstantBufferView(0, cbLocation);

    // ConstantBuffer�փf�[�^�]���J�n
    D3D12_RANGE range = { 0, 0 };
    SceneConstantBuffer *cbvData = nullptr;
    frameData.d3dConstantBuffer->Map(0, &range, reinterpret_cast<void **>(&cbvData));

    // �J��������
    for (auto sceneProxy : sceneProxies) {
        if (sceneProxy->GetActorType() == SceneActorType::Camera) {
            auto cameraSceneProxy = static_cast<CameraSceneProxy *>(sceneProxy);

            frameData.d3dCommandList->RSSetViewports(1, &cameraSceneProxy->GetViewport());
            frameData.d3dCommandList->RSSetScissorRects(1, &cameraSceneProxy->GetScissorRect());

            cameraSceneProxy->GetViewMatrix(&cbvData->ViewMatrix);
            cameraSceneProxy->GetProjectionMatrix(&cbvData->ProjMatrix);
            break;
        }
    }

    // Present -> RenderTarget
    {
        D3D12_RESOURCE_BARRIER d3dResBarrier;
        d3dResBarrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3dResBarrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        d3dResBarrier.Transition.pResource   = d3dRenderTarget[nextBackBufferIndex].Get();
        d3dResBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        d3dResBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
        d3dResBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        frameData.d3dCommandList->ResourceBarrier(1, &d3dResBarrier);
    }

    // RenderTarget�Z�b�g
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3dRTVHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += static_cast<SIZE_T>(nextBackBufferIndex) * static_cast<SIZE_T>(rtvDescriptorSize);
    frameData.d3dCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // RenderTarget�N���A
    const float clearColor[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    frameData.d3dCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    
    // Triangle���C���X�^���X�`��
    UINT instanceCount = 0;
    for (auto sceneProxy : sceneProxies) {
        if (sceneProxy->GetActorType() == SceneActorType::Triangle) {
            auto triSceneProxy = static_cast<TriangleSceneProxy *>(sceneProxy);
            triSceneProxy->GetWorldMatrix(&cbvData->worldMatrix[instanceCount]);

            instanceCount++;
        }
    }

    // ConstantBuffer�ւ̃f�[�^�]������
    frameData.d3dConstantBuffer->Unmap(0, nullptr);

    if (0 < instanceCount) {
        // VertexBuffer�Z�b�g
        frameData.d3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        frameData.d3dCommandList->IASetVertexBuffers(0, 1, &vtxBufferView);

        // �C���X�^���X�`��
        frameData.d3dCommandList->DrawInstanced(3, instanceCount, 0, 0);
    }

    // RenderTarget -> Present
    {
        D3D12_RESOURCE_BARRIER d3dResBarrier;
        d3dResBarrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3dResBarrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        d3dResBarrier.Transition.pResource   = d3dRenderTarget[nextBackBufferIndex].Get();
        d3dResBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        d3dResBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
        d3dResBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        frameData.d3dCommandList->ResourceBarrier(1, &d3dResBarrier);
    }

    // CommandList�����
    frameData.d3dCommandList->Close();

    // GPU�����p�t�F���X���Z�b�g
    d3dCommandQueue->Signal(frameData.d3dFence.Get(), frameData.fenceValue);
    frameData.syncGPU = true;
}
