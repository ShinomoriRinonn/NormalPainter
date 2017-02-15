﻿#pragma once

#ifdef mscDebug
    #define mscTrace(...) ::ms::LogImpl("MeshSync trace: " __VA_ARGS__)
#else
    #define mscTrace(...)
#endif

class MeshSyncClientMaya
{
public:
    enum class TargetScope
    {
        Unknown,
        Selection,
        All,
    };
    static MeshSyncClientMaya& getInstance();

    MeshSyncClientMaya(MObject obj);
    ~MeshSyncClientMaya();

    void setServerAddress(const char *v);
    void setServerPort(uint16_t v);
    void setAutoSync(bool v);

    void onIdle();
    void onSelectionChanged();
    void onSceneUpdated();

    void notifyUpdateTransform(MObject obj);
    void notifyUpdateMesh(MObject obj);
    void sendUpdatedObjects();
    void sendScene(TargetScope scope = TargetScope::All);
    bool importScene();

private:
    bool isAsyncSendInProgress() const;
    void waitAsyncSend();
    void registerGlobalCallbacks();
    void registerSelectionCallbacks();
    void removeGlobalCallbacks();
    void removeSelectionCallbacks();
    int getMaterialID(MUuid uid);
    int getObjectID(MUuid uid);
    void extractAllMaterialData();
    void extractTransformData(ms::Transform& dst, MObject src);
    void extractMeshData(ms::Mesh& dst, MObject src);
    void kickAsyncSend();

private:
    using ClientMeshes = std::vector<ms::MeshPtr>;
    using HostMeshes = std::map<int, ms::MeshPtr>;
    using ExistRecords = std::map<std::string, bool>;
    using Materials = std::vector<ms::Material>;
    using Transforms = std::vector<ms::TransformPtr>;

    MObject m_obj;
    MFnPlugin m_iplugin;
    bool m_auto_sync = true;
    int m_timeout_ms = 5000;

    std::vector<MCallbackId> m_cids_global;
    std::vector<MCallbackId> m_cids_selection;
    std::vector<MUuid> m_material_id_table;
    std::vector<MUuid> m_object_id_table;
    std::vector<MObject> m_mtransforms;
    std::vector<MObject> m_mmeshes;

    ms::ClientSettings m_client_settings;
    float m_scale_factor = 1.0f;
    ClientMeshes m_client_meshes;
    Transforms m_client_transforms;
    HostMeshes m_host_meshes;
    Materials m_materials;
    ExistRecords m_exist_record;
    std::future<void> m_future_send;
    bool m_pending_send_meshes = false;

    bool m_bake_skin = false;
    bool m_bake_cloth = false;
};
