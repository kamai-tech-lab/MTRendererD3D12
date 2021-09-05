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
    SceneActorType      type;           // Actorの種類
    DirectX::XMVECTOR   translation;    // 平行移動成分
    DirectX::XMVECTOR   rotation;       // 回転成分
    DirectX::XMVECTOR   scale;          // スケール成分


    /// @brief 更新処理
    virtual void Update(float delta) = 0;

    /// @brief コンストラクタ
    SceneActor(SceneActorType inType);

    /// @brief デストラクタ
    virtual ~SceneActor();
};

/// @class TriangleSceneActor
class TriangleSceneActor : public SceneActor {
friend class TriangleSceneProxy;

public:
    /// @brief 更新処理
    virtual void Update(float delta) override;

    /// @brief 回転速度をセット
    void SetRotSpeed(const float newSpeed) {
        rotSpeed = newSpeed;
    }

    /// @brief 回転速度を取得
    float GetRotSpeed() const {
        return rotSpeed;
    }

    /// @brief スケール速度をセット
    void SetScaleSpeed(const float newSpeed) {
        scaleSpeed = newSpeed;
    }

    /// @brief スケール速度を取得
    float GetScaleSpeed() const {
        return scaleSpeed;
    }
    
    /// @brief コンストラクタ
    TriangleSceneActor();

    /// @brief デストラクタ
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
    /// @brief 更新処理
    virtual void Update(float delta) override;

    /// @brief 初期化
    bool Init(const UINT width, const UINT height, const float degFov);

    /// @brief コンストラクタ
    CameraSceneActor();

    /// @brief デストラクタ
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
    /// @brief ActorからProxyへ描画情報を伝達
    virtual void Commit(SceneActor *actor) = 0;

    /// @brief SceneActorTypeを取得
    SceneActorType GetActorType() const {
        return type;
    }

    /// @brief コンストラクタ
    SceneProxy(SceneActorType inType);

    /// @brief デストラクタ
    virtual ~SceneProxy();

private:
    SceneActorType type;
};

/// @class TriangleSceneProxy
class TriangleSceneProxy : public SceneProxy {
public:
    /// @brief ActorからProxyへ描画情報を伝達
    virtual void Commit(SceneActor *actor) override;

    /// @brief WorldMatrix取得
    void GetWorldMatrix(DirectX::XMFLOAT4X4 *dstWorldMtx) {
        XMStoreFloat4x4(dstWorldMtx, worldMtx);
    }

    /// @brief コンストラクタ
    TriangleSceneProxy();

    /// @brief デストラクタ
    virtual ~TriangleSceneProxy();

protected:
    DirectX::XMMATRIX   worldMtx;
};

/// @class CameraSceneProxy
class CameraSceneProxy : public SceneProxy {
public:
    /// @brief ActorからProxyへ描画情報を伝達
    virtual void Commit(SceneActor *actor) override;

    /// @brief Viewport取得
    D3D12_VIEWPORT& GetViewport() {
        return viewport;
    }

    /// @brief ScissorRect取得
    D3D12_RECT& GetScissorRect() {
        return scissorRect;
    }

    /// @brief ViewMatrix取得
    void GetViewMatrix(DirectX::XMFLOAT4X4 *dstViewMtx) {
        XMStoreFloat4x4(dstViewMtx, viewMtx);
    }

    /// @brief ProjectionMatrix取得
    void GetProjectionMatrix(DirectX::XMFLOAT4X4 *dstProjMtx) {
        XMStoreFloat4x4(dstProjMtx, projMtx);
    }

    /// @brief コンストラクタ
    CameraSceneProxy();

    /// @brief デストラクタ
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
    /// @brief 初期化処理
    bool Init();

    /// @brief 初期化処理
    bool Init(const UINT width, const UINT height, const UINT inFrameCount, LPCWSTR titleStr);

    /// @brief 実行
    /// @return 終了ステータス
    int Run();

    /// @brief 終了処理
    void Deinit();

    /// @brief 指定したフラグをセット
    void SetFlag(const GlobalFlag flag);

    /// @brief 指定したフラグをクリア
    void ClearFlag(const GlobalFlag flag);

    /// @brief 指定したフラグがセットされているか調べる
    bool TestFlag(const GlobalFlag flag);

    /// @brief アクタを追加
    void AddSceneActor(SceneActor *actor);

    /// @brief コンストラクタ
    MTRenderer(HINSTANCE hInstance, int nCmdShow);

    /// @brief デストラクタ
    ~MTRenderer();

private:
    /// @brief メインスレッド処理関数
    /// @param renderer MTRendererへのポインタ
    /// @return 終了ステータス
    static int MainThreadFunc(MTRenderer *renderer);

    /// @brief 描画スレッド処理関数
    /// @param renderer MTRenererへのポインタ
    /// @return 終了ステータス
    static int RenderThreadFunc(MTRenderer *renderer);

    /// @brief 描画デバイスの初期化
    bool InitDevices();

    /// @brief 各種リソースを初期化
    bool InitResources();

    /// @name MainThread処理
    /// @{
    void PreUpdate();
    void Update(float delta);
    void SyncRenderThread();
    void CommitSceneProxy();
    void RestartRenderThread();
    /// @}

    /// @name RenderThead処理
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

    // ConstantBufferのサイズは256-byteアライメントが必要
    static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant buffer size must be 256-byte aligned.");

    /// @brief 頂点データ定義
    /// @struct Vertex
    struct Vertex {
        float pos[3];
        float color[4];
    };

    // Resources
    ComPtr<ID3D12Resource>      vtxBuffer;
    D3D12_VERTEX_BUFFER_VIEW    vtxBufferView;

    /// @brief フレーム毎に必要なリソース
    /// @struct FrameData
    struct FrameData {
        ComPtr<ID3D12CommandAllocator>      d3dCommandAllocator;
        ComPtr<ID3D12GraphicsCommandList>   d3dCommandList;
        ComPtr<ID3D12Resource>              d3dConstantBuffer;
        ComPtr<ID3D12Fence>                 d3dFence;
        UINT64                              fenceValue;
        bool                                syncGPU;

        /// @brief コンストラクタ
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
