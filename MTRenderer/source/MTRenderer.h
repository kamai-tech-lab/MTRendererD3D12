/// @file MTRenderer.h
/// @author Masayoshi Kamai

#pragma once

using Microsoft::WRL::ComPtr;

// Default value
const UINT DEFAULT_CANVAS_WIDTH           = 1280;
const UINT DEFAULT_CANVAS_HEIGHT          = 720;
const UINT DEFAULT_FRAME_COUNT            = 3;
const UINT MAX_FRAME_COUNT                = 3;
const UINT DEFAULT_CBV_COUNT              = 64;
const UINT DEFAULT_SRV_COUNT              = 64;
const UINT DEFAULT_UAV_COUNT              = 32;
const UINT DEFAULT_SAMPLER_COUNT          = 64;
const size_t DEFAULT_SCENE_ACTOR_CAPACITY = 32;
const size_t DEFAULT_SCENE_PROXY_CAPACITY = 32;


/// @enum SceneActorType
enum class SceneActorType : UINT {
    Camera,     ///< CameraActor
    Triangle,   ///< TriangleActor
};

/// @class SceneActor
class SceneActor {
public:
    SceneActorType      type;           // Actor�̎��
    DirectX::XMVECTOR   translation;    // ���s�ړ�����
    DirectX::XMVECTOR   rotation;       // ��]����
    DirectX::XMVECTOR   scale;          // �X�P�[������


    /// @brief �X�V����
    virtual void Update(float delta) = 0;

    /// @brief �R���X�g���N�^
    SceneActor(SceneActorType inType);

    /// @brief �f�X�g���N�^
    virtual ~SceneActor();
};

/// @class TriangleSceneActor
class TriangleSceneActor : public SceneActor {
friend class TriangleSceneProxy;

public:
    /// @brief �X�V����
    virtual void Update(float delta) override;

    /// @brief ��]���x���Z�b�g
    void SetRotSpeed(const float newSpeed) {
        rotSpeed = newSpeed;
    }

    /// @brief ��]���x���擾
    float GetRotSpeed() const {
        return rotSpeed;
    }

    /// @brief �X�P�[�����x���Z�b�g
    void SetScaleSpeed(const float newSpeed) {
        scaleSpeed = newSpeed;
    }

    /// @brief �X�P�[�����x���擾
    float GetScaleSpeed() const {
        return scaleSpeed;
    }
    
    /// @brief �R���X�g���N�^
    TriangleSceneActor();

    /// @brief �f�X�g���N�^
    virtual ~TriangleSceneActor();

protected:
    float   rotSpeed;
    float   scaleSpeed;

    float rotAngle;
    float scaleAngle;
};

/// @class CameraSceneActor
class CameraSceneActor : public SceneActor {
friend class CameraSceneProxy;

public:
    /// @brief �X�V����
    virtual void Update(float delta) override;

    /// @brief ������
    bool Init(const UINT width, const UINT height, const float degFov);

    /// @brief �R���X�g���N�^
    CameraSceneActor();

    /// @brief �f�X�g���N�^
    ~CameraSceneActor();

protected:
    UINT    screenWidth;
    UINT    screenHeight;
    float   fovInDeg;

    DirectX::XMVECTOR   cameraEye;
    DirectX::XMVECTOR   cameraAt;
    DirectX::XMVECTOR   cameraUp;
};


/// @class SceneProxy
class SceneProxy {
public:
    /// @brief Actor����Proxy�֕`�����`�B
    virtual void Commit(SceneActor *actor) = 0;

    /// @brief SceneActorType���擾
    SceneActorType GetActorType() const {
        return type;
    }

    /// @brief �R���X�g���N�^
    SceneProxy(SceneActorType inType);

    /// @brief �f�X�g���N�^
    virtual ~SceneProxy();

private:
    SceneActorType type;
};

/// @class TriangleSceneProxy
class TriangleSceneProxy : public SceneProxy {
public:
    /// @brief Actor����Proxy�֕`�����`�B
    virtual void Commit(SceneActor *actor) override;

    /// @brief WorldMatrix�擾
    void GetWorldMatrix(DirectX::XMFLOAT4X4 *dstWorldMtx) {
        XMStoreFloat4x4(dstWorldMtx, worldMtx);
    }

    /// @brief �R���X�g���N�^
    TriangleSceneProxy();

    /// @brief �f�X�g���N�^
    virtual ~TriangleSceneProxy();

protected:
    DirectX::XMMATRIX   worldMtx;
};

/// @class CameraSceneProxy
class CameraSceneProxy : public SceneProxy {
public:
    /// @brief Actor����Proxy�֕`�����`�B
    virtual void Commit(SceneActor *actor) override;

    /// @brief Viewport�擾
    D3D12_VIEWPORT& GetViewport() {
        return viewport;
    }

    /// @brief ScissorRect�擾
    D3D12_RECT& GetScissorRect() {
        return scissorRect;
    }

    /// @brief ViewMatrix�擾
    void GetViewMatrix(DirectX::XMFLOAT4X4 *dstViewMtx) {
        XMStoreFloat4x4(dstViewMtx, viewMtx);
    }

    /// @brief ProjectionMatrix�擾
    void GetProjectionMatrix(DirectX::XMFLOAT4X4 *dstProjMtx) {
        XMStoreFloat4x4(dstProjMtx, projMtx);
    }

    /// @brief �R���X�g���N�^
    CameraSceneProxy();

    /// @brief �f�X�g���N�^
    virtual ~CameraSceneProxy();

protected:
    D3D12_VIEWPORT      viewport;
    D3D12_RECT          scissorRect;
    DirectX::XMMATRIX   viewMtx;
    DirectX::XMMATRIX   projMtx;
};


/// @enum GlobalFlag
enum class GlobalFlag : UINT {
    TerminateRenderer   = (1u << 0u),
};


/// @class MTRenderer
class MTRenderer {
public:
    /// @brief ����������
    bool Init();

    /// @brief ����������
    bool Init(const UINT width, const UINT height, const UINT inFrameCount, LPCWSTR titleStr);

    /// @brief ���s
    /// @return �I���X�e�[�^�X
    int Run();

    /// @brief �I������
    void Deinit();

    /// @brief �w�肵���t���O���Z�b�g
    void SetFlag(const GlobalFlag flag);

    /// @brief �w�肵���t���O���N���A
    void ClearFlag(const GlobalFlag flag);

    /// @brief �w�肵���t���O���Z�b�g����Ă��邩���ׂ�
    bool TestFlag(const GlobalFlag flag);

    /// @brief �A�N�^��ǉ�
    void AddSceneActor(SceneActor *actor);

    /// @brief �R���X�g���N�^
    MTRenderer(HINSTANCE hInstance, int nCmdShow);

    /// @brief �f�X�g���N�^
    ~MTRenderer();

private:
    /// @brief ���C���X���b�h�����֐�
    /// @param renderer MTRenderer�ւ̃|�C���^
    /// @return �I���X�e�[�^�X
    static int MainThreadFunc(MTRenderer *renderer);

    /// @brief �`��X���b�h�����֐�
    /// @param renderer MTRenerer�ւ̃|�C���^
    /// @return �I���X�e�[�^�X
    static int RenderThreadFunc(MTRenderer *renderer);

    /// @brief �`��f�o�C�X�̏�����
    bool InitDevices();

    /// @brief �e�탊�\�[�X��������
    bool InitResources();

    /// @name MainThread����
    /// @{
    void PreUpdate();
    void Update(float delta);
    void SyncRenderThread();
    void CommitSceneProxy();
    void RestartRenderThread();
    /// @}

    /// @name RenderThead����
    /// @{
    void SyncMainThread();
    void PreRender();
    void SyncGPU();
    void Present();
    void PopulateCommandList();
    void Render();
    /// @}

private:
    std::thread mainThread;
    std::thread renderThread;

    bool                    mainTheadReady;
    bool                    renderTheadReady;
    std::mutex              syncMainThreadMtx;
    std::mutex              syncRenderThreadMtx;
    std::condition_variable syncMainThreadCond;
    std::condition_variable syncRenderThreadCond;

    UINT        canvasWidth;
    UINT        canvasHeight;
    HINSTANCE   instanceHandle;
    int         commandShowFlags;
    HWND        windowHandle;

    ComPtr<ID3D12Device6>           d3dDevice;
    ComPtr<IDXGISwapChain3>         dxgiSwapChain;
    ComPtr<ID3D12CommandQueue>      d3dCommandQueue;
    
    ComPtr<ID3D12DescriptorHeap>    d3dRTVHeap;
    ComPtr<ID3D12DescriptorHeap>    d3dDSVHeap;
    ComPtr<ID3D12DescriptorHeap>    d3dCBVHeap;
    ComPtr<ID3D12DescriptorHeap>    d3dSamplerHeap;
    ComPtr<ID3D12RootSignature>     d3dRootSignature;
    ComPtr<ID3D12PipelineState>     d3dPipelineState;
    
    ComPtr<ID3D12Resource>  d3dRenderTarget[MAX_FRAME_COUNT];
    UINT                    rtvDescriptorSize;

    ComPtr<IDXGIFactory6>   dxgiFactory;
    ComPtr<IDXGIAdapter1>   dxgiAdapter;

#if ENABLE_D3D12_DEBUG_INTERFACE
    ComPtr<ID3D12Debug>     d3dDebugInterface;
#endif


    /// @struct SceneConstantBuffer
    struct SceneConstantBuffer {
        DirectX::XMFLOAT4X4 ViewMatrix;
        DirectX::XMFLOAT4X4 ProjMatrix;
        DirectX::XMFLOAT4X4 worldMatrix[510];
    };

    // ConstantBuffer�̃T�C�Y��256-byte�A���C�����g���K�v
    static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant buffer size must be 256-byte aligned.");

    /// @brief ���_�f�[�^��`
    /// @struct Vertex
    struct Vertex {
        float pos[3];
        float color[4];
    };

    // Resources
    ComPtr<ID3D12Resource>      vtxBuffer;
    D3D12_VERTEX_BUFFER_VIEW    vtxBufferView;

    /// @brief �t���[�����ɕK�v�ȃ��\�[�X
    /// @struct FrameData
    struct FrameData {
        ComPtr<ID3D12CommandAllocator>      d3dCommandAllocator;
        ComPtr<ID3D12GraphicsCommandList>   d3dCommandList;
        ComPtr<ID3D12Resource>              d3dConstantBuffer;
        ComPtr<ID3D12Fence>                 d3dFence;
        UINT64                              fenceValue;
        bool                                syncGPU;

        /// @brief �R���X�g���N�^
        FrameData()
        : fenceValue(0)
        , syncGPU(false)
        {
            ;
        }
    };

    FrameData   frameDataArray[MAX_FRAME_COUNT];

    UINT    flags;
    UINT    backBufferIndex;
    UINT    backBufferCount;
    HANDLE  fenceEvent;

    CameraSceneActor            defaultCameraActor;
    TriangleSceneActor          defaultTriangleActor;
    std::vector<SceneActor *>   sceneActors;
    std::vector<SceneProxy *>   sceneProxies;
};
