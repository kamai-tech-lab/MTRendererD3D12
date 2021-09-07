/// @file MTRendererD3D12.h
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
    Camera,     ///< @~ CameraActor
    Triangle,   ///< @~ TriangleActor
};

/// @enum SceneProxyType
enum class SceneProxyType : UINT {
    Camera,     ///< @~ CameraActor
    Triangle,   ///< @~ TriangleActor
};


/// @class SceneActor
class SceneActor {
public:
    /// @~english Translation component
    /// @~japanese 平行移動成分
    DirectX::XMVECTOR   translation;

    /// @~english Rotation component
    /// @~japanese 回転成分
    DirectX::XMVECTOR   rotation;

    /// @~english Scale component
    /// @~japanese スケール成分
    DirectX::XMVECTOR   scale;

    /// @~english
    /// @brief Update process
    /// @param[in] delta Time taken between frames (seconds
    /// @~japanese
    /// @brief 更新処理
    /// @param[in] delta フレーム間に掛かった時間（秒
    virtual void Update(float delta) = 0;

    /// @~english
    /// @brief Get actor type
    /// @return SceneActorType
    /// @~japanese
    /// @brief SceneActorTypeを取得
    /// @return SceneActorType
    SceneActorType GetActorType() const {
        return type;
    }

    /// @~english 
    /// @brief Constructor
    /// @param[in] inType Actor type
    /// @~japanese
    /// @brief コンストラクタ
    /// @param[in] inType アクタの種類
    SceneActor(SceneActorType inType);

    /// @~english
    /// @brief Destructor
    /// @~japanese
    /// @brief デストラクタ
    virtual ~SceneActor();

protected:
    /// @~english Actor type
    /// @~japanese Actorの種類
    SceneActorType      type;
};

/// @class TriangleSceneActor
class TriangleSceneActor : public SceneActor {
friend class TriangleSceneProxy;

public:
    /// @~english
    /// @brief Update process
    /// @param[in] delta Time taken between frames (seconds
    /// @~japanese
    /// @brief 更新処理
    /// @param[in] delta フレーム間に掛かった時間（秒
    virtual void Update(float delta) override;

    /// @~english
    /// @brief Set the speed at which the rotation is updated
    /// @~japanese
    /// @brief 回転を更新する速度をセット
    void SetRotSpeed(const float newSpeed) {
        rotSpeed = newSpeed;
    }

    /// @~english
    /// @brief Get the speed at which the rotation is updated
    /// @return Rotation speed
    /// @~japanese
    /// @brief 回転を更新する速度を取得
    /// @return 回転速度
    float GetRotSpeed() const {
        return rotSpeed;
    }

    /// @~english
    /// @brief Set the speed at which the scale is updated
    /// @~japanese
    /// @brief スケールを更新する速度をセット
    void SetScaleSpeed(const float newSpeed) {
        scaleSpeed = newSpeed;
    }

    /// @~english
    /// @brief Get the speed at which the scale is updated
    /// @return Scale speed
    /// @~japanese
    /// @brief スケールを更新する速度を取得
    /// @return スケール速度
    float GetScaleSpeed() const {
        return scaleSpeed;
    }
    
    /// @~english 
    /// @brief Constructor
    /// @~japanese
    /// @brief コンストラクタ
    TriangleSceneActor();

    /// @~english
    /// @brief Destructor
    /// @~japanese
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
    /// @~english
    /// @brief Update process
    /// @param[in] delta Time taken between frames (seconds
    /// @~japanese
    /// @brief 更新処理
    /// @param[in] delta フレーム間に掛かった時間（秒
    virtual void Update(float delta) override;

    /// @~english
    /// @brief Initialize
    /// @param[in] width  Screen width
    /// @param[in] height Screen height
    /// @param[in] defFov Field of view in Degree
    /// @return True if initialization succeeded, false otherwise
    /// @~japanese
    /// @brief 初期化
    /// @param[in] width  スクリーン幅
    /// @param[in] height スクリーン高さ
    /// @param[in] defFov 視野角（角度
    /// @return 初期化に成功した場合にはTrue、そうでないならFalseを返す
    bool Init(const UINT width, const UINT height, const float degFov);

    /// @~english 
    /// @brief Constructor
    /// @~japanese
    /// @brief コンストラクタ
    CameraSceneActor();

    /// @~english
    /// @brief Destructor
    /// @~japanese
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
    /// @~english
    /// @brief Transfer render information from Actor to Proxy
    /// @param[in] actor Pointer to actor
    /// @~japanese
    /// @brief ActorからProxyへ描画情報を伝達
    /// @param[in] actor アクタへのポインタ
    virtual void Commit(SceneActor *actor) = 0;

    /// @~english
    /// @brief Get proxy type
    /// @return SceneProxyType
    /// @~japanese
    /// @brief SceneProxyTypeを取得
    /// @return SceneProxyType
    SceneProxyType GetProxyType() const {
        return type;
    }

    /// @~english 
    /// @brief Constructor
    /// @param[in] inType Actor type
    /// @~japanese
    /// @brief コンストラクタ
    /// @param[in] inType アクタの種類
    SceneProxy(SceneProxyType inType);

    /// @~english
    /// @brief Destructor
    /// @~japanese
    /// @brief デストラクタ
    virtual ~SceneProxy();

protected:
    SceneProxyType type;
};

/// @class TriangleSceneProxy
class TriangleSceneProxy : public SceneProxy {
public:
    /// @~english
    /// @brief Transfer render information from Actor to Proxy
    /// @param[in] actor Pointer to actor
    /// @~japanese
    /// @brief ActorからProxyへ描画情報を伝達
    /// @param[in] actor アクタへのポインタ
    virtual void Commit(SceneActor *actor) override;

    /// @~english
    /// @brief Get the world transformation matrix
    /// @return World transformation matrix
    /// @~japanese
    /// @brief World変換行列を取得
    /// @return World変換行列
    void GetWorldMatrix(DirectX::XMFLOAT4X4 *dstWorldMtx) {
        XMStoreFloat4x4(dstWorldMtx, worldMtx);
    }

    /// @~english 
    /// @brief Constructor
    /// @~japanese
    /// @brief コンストラクタ
    TriangleSceneProxy();

    /// @~english
    /// @brief Destructor
    /// @~japanese
    /// @brief デストラクタ
    virtual ~TriangleSceneProxy();

protected:
    DirectX::XMMATRIX   worldMtx;
};

/// @class CameraSceneProxy
class CameraSceneProxy : public SceneProxy {
public:
    /// @~english
    /// @brief Transfer render information from Actor to Proxy
    /// @param[in] actor Pointer to actor
    /// @~japanese
    /// @brief ActorからProxyへ描画情報を伝達
    /// @param[in] actor アクタへのポインタ
    virtual void Commit(SceneActor *actor) override;

    /// @~english
    /// @brief Get view port
    /// @return D3D12_VIEWPORT
    /// @~japanese
    /// @brief Viewportを取得
    /// @return D3D12_VIEWPORT
    D3D12_VIEWPORT& GetViewport() {
        return viewport;
    }

    /// @~english
    /// @brief Get scissor rect
    /// @return D3D12_RECT
    /// @~japanese
    /// @brief ScissorRectを取得
    /// @return D3D12_RECT
    D3D12_RECT& GetScissorRect() {
        return scissorRect;
    }

    /// @~english
    /// @brief Get the view transformation matrix
    /// @return View transformation matrix
    /// @~japanese
    /// @brief View変換行列を取得
    /// @return View変換行列
    void GetViewMatrix(DirectX::XMFLOAT4X4 *dstViewMtx) {
        XMStoreFloat4x4(dstViewMtx, viewMtx);
    }

    /// @~english
    /// @brief Get the projection matrix
    /// @return Projection matrix
    /// @~japanese
    /// @brief 射影行列を取得
    /// @return 射影行列
    void GetProjectionMatrix(DirectX::XMFLOAT4X4 *dstProjMtx) {
        XMStoreFloat4x4(dstProjMtx, projMtx);
    }

    /// @~english 
    /// @brief Constructor
    /// @~japanese
    /// @brief コンストラクタ
    CameraSceneProxy();

    /// @~english
    /// @brief Destructor
    /// @~japanese
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
    
    /// @~english
    /// @brief Initialize
    /// @return True if initialization succeeded, false otherwise
    /// @~japanese
    /// @brief 初期化
    /// @return 初期化に成功した場合にはTrue、そうでないならFalseを返す
    bool Init();

    /// @~english
    /// @brief Initialize
    /// @param[in] width  Screen width
    /// @param[in] height Screen height
    /// @param[in] inBackBufferCount Number of back buffers
    /// @param[in] titleStr Window title string
    /// @return True if initialization succeeded, false otherwise
    /// @~japanese
    /// @brief 初期化
    /// @param[in] width  スクリーン幅
    /// @param[in] height スクリーン高さ
    /// @param[in] inBackBufferCount バックバッファ数
    /// @param[in] titleStr ウィンドウタイトル文字列
    /// @return 初期化に成功した場合にはTrue、そうでないならFalseを返す
    bool Init(const UINT width, const UINT height, const UINT inBackBufferCount, LPCWSTR titleStr);

    /// @~english
    /// @brief Run the MTRenderer
    /// @return Return code
    /// @~japanese
    /// @brief MTRendererを実行
    /// @return 終了ステータス
    int Run();

    /// @~english
    /// @brief Deinitialize
    /// @~japanese
    /// @brief 終了処理
    void Deinit();

    /// @~english
    /// @brief Set the specified flag
    /// @~japanese
    /// @brief 指定したフラグをセット
    void SetFlag(const GlobalFlag flag);

    /// @~english
    /// @brief Clear the specified flag
    /// @~japanese
    /// @brief 指定したフラグをクリア
    void ClearFlag(const GlobalFlag flag);

    /// @~english
    /// @brief Test the specified flag
    /// @return True if specified flag is set, false otherwise
    /// @~japanese
    /// @brief 指定したフラグがセットされているか調べる
    /// @return 指定したフラグがセットされていればTrue、そうでないならFalse
    bool TestFlag(const GlobalFlag flag);

    /// @~english
    /// @brief Add actor
    /// @param[in] actor Pointer to actor
    /// @~japanese
    /// @brief アクタを追加
    /// @param[in] actor アクタへのポインタ
    void AddSceneActor(SceneActor *actor);

    /// @~english 
    /// @brief Constructor
    /// @~japanese
    /// @brief コンストラクタ
    MTRenderer(HINSTANCE hInstance, int nCmdShow);

    /// @~english
    /// @brief Destructor
    /// @~japanese
    /// @brief デストラクタ
    ~MTRenderer();

private:
    /// @~english
    /// @brief Main thread function
    /// @param[in] renderer Pointer to MTRenderer
    /// @return Return code
    /// @~japanese
    /// @brief メインスレッド処理関数
    /// @param renderer MTRendererへのポインタ
    /// @return 終了ステータス
    static int MainThreadFunc(MTRenderer *renderer);

    /// @~english
    /// @brief Render thread function
    /// @param[in] renderer Pointer to MTRenderer
    /// @return Return code
    /// @~japanese
    /// @brief 描画スレッド処理関数
    /// @param renderer MTRendererへのポインタ
    /// @return 終了ステータス
    static int RenderThreadFunc(MTRenderer *renderer);

    /// @~english
    /// @brief Initialize devices
    /// @return True if initialization succeeded, false otherwise
    /// @~japanese
    /// @brief デバイスの初期化
    /// @return 初期化に成功した場合にはTrue、そうでないならFalseを返す
    bool InitDevices();

    /// @~english
    /// @brief Initialize resources
    /// @return True if initialization succeeded, false otherwise
    /// @~japanese
    /// @brief 各種リソースを初期化
    /// @return 初期化に成功した場合にはTrue、そうでないならFalseを返す
    bool InitResources();

    /// @~english
    /// @name Main thread processes
    /// @~japanese
    /// @name MainThread処理
    /// @{
    void PreUpdate();
    void Update(float delta);
    void SyncRenderThread();
    void CommitSceneProxy();
    void RestartRenderThread();
    /// @}

    /// @~english
    /// @name Render thread processes
    /// @~japanese
    /// @name RenderThead処理
    /// @{
    void RestartMainThread();
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

    // ConstantBuffer size must be 256-byte aligned
    // ConstantBufferのサイズは256-byteアライメントが必要
    static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant buffer size must be 256-byte aligned.");

    /// @~english
    /// @brief Vertex data definition
    /// @~japanese
    /// @brief 頂点データ定義
    /// @~
    /// @struct Vertex
    struct Vertex {
        float pos[3];
        float color[4];
    };

    // Resources
    ComPtr<ID3D12Resource>      vtxBuffer;
    D3D12_VERTEX_BUFFER_VIEW    vtxBufferView;

    /// @~english
    /// @brief Resources needed each frames
    /// @~japanese
    /// @brief フレーム毎に必要なリソース
    /// @~
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
