////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Gugu/Editor/Editor.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Editor/EditorVersion.h"
#include "Gugu/Editor/Core/ProjectSettings.h"
#include "Gugu/Editor/Modal/AboutDialog.h"
#include "Gugu/Editor/Modal/BaseModalDialog.h"
#include "Gugu/Editor/Modal/OpenProjectDialog.h"
#include "Gugu/Editor/Panel/AssetsExplorerPanel.h"
#include "Gugu/Editor/Panel/OutputLogPanel.h"
#include "Gugu/Editor/Panel/DependenciesPanel.h"
#include "Gugu/Editor/Panel/Document/DatasheetPanel.h"
#include "Gugu/Editor/Panel/Document/AnimSetPanel.h"
#include "Gugu/Editor/Panel/Document/ImageSetPanel.h"
#include "Gugu/Editor/Panel/Document/ParticleEffectPanel.h"
#include "Gugu/Editor/Panel/Document/TexturePanel.h"
#include "Gugu/Editor/Parser/DatasheetParser.h"
#include "Gugu/Editor/Resources/VirtualDatasheet.h"

#include "Gugu/Engine.h"
#include "Gugu/Inputs/ManagerInputs.h"
#include "Gugu/Resources/ManagerResources.h"
#include "Gugu/Resources/Resource.h"
#include "Gugu/System/SystemUtility.h"
#include "Gugu/External/ImGuiUtility.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {
    
Editor::Editor()
    : m_project(nullptr)
    , m_checkDirtyDocuments(false)
    , m_pendingCloseEditor(false)
    , m_pendingCloseProject(false)
    , m_pendingCloseDocument(false)
    , m_resetPanels(false)
    , m_showImGuiDemo(false)
    , m_assetsExplorerPanel(nullptr)
    , m_outputLogPanel(nullptr)
    , m_dependenciesPanel(nullptr)
    , m_lastActiveDocument(nullptr)
    , m_datasheetParser(nullptr)
{
    // This constructor should stay empty.
    // Because it's a singleton, if a GetInstance() is called inside by another system but the constructor isn't finished,
    // the GetInstance will try to create an other instance (loop).
}

Editor::~Editor()
{
    // Because of the constructor problem, I prefer to let the destructor also empty.
}

void Editor::Init(const EditorConfig& editorConfig)
{
    m_editorConfig = editorConfig;

    NormalizePathSelf(m_editorConfig.projectPathFile);

    // Register Inputs.
    ManagerInputs* inputs = GetInputs();
    inputs->RegisterInput("ResetPanels", inputs->BuildKeyboardEvent(sf::Keyboard::F1));
    inputs->RegisterInput("SaveDocument", inputs->BuildKeyboardEvent(sf::Keyboard::S, true, false, false));
    inputs->RegisterInput("SaveAllDocuments", inputs->BuildKeyboardEvent(sf::Keyboard::S, true, true, false));
    inputs->RegisterInput("Undo", inputs->BuildKeyboardEvent(sf::Keyboard::Z, true, false, false));
    inputs->RegisterInput("Redo", inputs->BuildKeyboardEvent(sf::Keyboard::Y, true, false, false));

    // Additional ImGui Setup.
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // TODO: Does not seem supported by the SFML backend.
    io.ConfigWindowsResizeFromEdges = true;

    // Create Panels.
    m_assetsExplorerPanel = new AssetsExplorerPanel;
    m_outputLogPanel = new OutputLogPanel;
    m_dependenciesPanel = new DependenciesPanel;

    // Open last project if available.
    if (m_editorConfig.projectPathFile != "")
    {
        OpenProject(m_editorConfig.projectPathFile);
    }
}

void Editor::Release()
{
    CloseProjectImpl();

    ClearStdVector(m_modalDialogs);

    SafeDelete(m_assetsExplorerPanel);
    SafeDelete(m_outputLogPanel);
    SafeDelete(m_dependenciesPanel);

    Editor::DeleteInstance();
}

const EditorConfig& Editor::GetEditorConfig() const
{
    return m_editorConfig;
}

void Editor::OpenProject(const std::string& projectPathFile)
{
    m_pendingOpenProjectPathFile = projectPathFile;
}

void Editor::OpenProjectImpl(const std::string& projectPathFile)
{
    if (CloseProject())
    {
        m_project = new ProjectSettings;

        if (m_project->LoadFromFile(projectPathFile))
        {
            // Parse assets.
            GetResources()->ParseDirectory(m_project->projectAssetsPath);

            // Create the DatasheetParser.
            m_datasheetParser = new DatasheetParser;
            m_datasheetParser->ParseBinding(m_project->projectBindingPathFile);

            m_assetsExplorerPanel->RaiseDirtyContent();
        }
        else
        {
            SafeDelete(m_project);
        }
    }
}

bool Editor::CloseProject()
{
    if (IsProjectOpen())
    {
        if (!RaiseCheckDirtyDocuments())
        {
            CloseProjectImpl();
            return true;
        }
        else
        {
            m_pendingCloseProject = true;
            return false;
        }
    }

    return true;
}

void Editor::CloseProjectImpl()
{
    if (IsProjectOpen())
    {
        m_assetsExplorerPanel->ClearContent();

        m_lastActiveDocument = nullptr;
        ClearStdVector(m_documentPanels);
        ClearStdVector(m_modalDialogs);

        SafeDelete(m_datasheetParser);

        GetResources()->RemoveResourcesFromPath(m_project->projectAssetsPath);

        SafeDelete(m_project);
    }
}

bool Editor::IsProjectOpen() const
{
    return m_project != nullptr;
}

const ProjectSettings* Editor::GetProjectSettings() const
{
    return m_project;
}

bool Editor::OnSFEvent(const sf::Event& event)
{
    ManagerInputs* inputs = GetInputs();

    if (inputs->IsInputEventReleased("ResetPanels", event))
    {
        ResetPanels();
        return false;
    }
    else if (inputs->IsInputEventReleased("SaveAllDocuments", event))
    {
        SaveAllDirtyDocuments();
        return false;
    }
    else if (inputs->IsInputEventReleased("SaveDocument", event))
    {
        SaveActiveDocument();
        return false;
    }
    else if (inputs->IsInputEventReleased("Undo", event))
    {
        UndoActiveDocument();
        return false;
    }
    else if (inputs->IsInputEventReleased("Redo", event))
    {
        RedoActiveDocument();
        return false;
    }

    return true;
}

void Editor::Update(const DeltaTime& dt)
{
    // Handle pending open project.
    if (!m_pendingOpenProjectPathFile.empty())
    {
        OpenProjectImpl(m_pendingOpenProjectPathFile);
        m_pendingOpenProjectPathFile = "";
    }

    // Main menu bar.
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Editor"))
        {
            if (ImGui::MenuItem("Open Project..."))
            {
                GetEditor()->OpenModalDialog(new OpenProjectDialog(m_editorConfig.projectPathFile));
            }

            if (ImGui::MenuItem("Close Project"))
            {
                CloseProject();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("About..."))
            {
                GetEditor()->OpenModalDialog(new AboutDialog());
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Alt+F4"))
            {
                CloseEditor();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Document"))
        {
            ImGui::BeginDisabled(m_lastActiveDocument == nullptr);

            if (ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
                UndoActiveDocument();
            }

            if (ImGui::MenuItem("Redo", "Ctrl+Y"))
            {
                RedoActiveDocument();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                SaveActiveDocument();
            }

            if (ImGui::MenuItem("Save All", "Ctrl+Shift+S"))
            {
                SaveAllDirtyDocuments();
            }

            ImGui::EndDisabled();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Reset Panels", "F1"))
            {
                ResetPanels();
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Imgui Demo"))
        {
            m_showImGuiDemo = !m_showImGuiDemo;
        }

        ImGui::EndMainMenuBar();
    }

    // This popup has a higher priority than any other popup and thus can force reopening itself every frame, discarding any other popup.
    if (m_checkDirtyDocuments && !ImGui::IsPopupOpen("Save Documents"))
    {
        //TODO: for clarity, I could explicitly force close other popups here, instead of letting imgui handling it.
        ImGui::OpenPopup("Save Documents");
    }

    if (ImGui::BeginPopupModal("Save Documents", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("You are trying to close documents with unsaved modifications.");

        ImGui::Spacing();
        if (ImGui::Button("Cancel"))
        {
            if (m_pendingCloseDocument)
            {
                CancelClosingDirtyDocuments();
            }

            m_pendingCloseEditor = false;
            m_pendingCloseProject = false;
            m_pendingCloseDocument = false;

            m_checkDirtyDocuments = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Close"))
        {
            if (m_pendingCloseDocument)
            {
                ValidateClosingDirtyDocuments();
            }

            m_checkDirtyDocuments = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Save And Close"))
        {
            if (m_pendingCloseEditor || m_pendingCloseProject)
            {
                SaveAllDirtyDocuments();
            }
            else if (m_pendingCloseDocument)
            {
                SaveAllClosingDirtyDocuments();
            }

            m_checkDirtyDocuments = false;
        }

        if (!m_checkDirtyDocuments)
        {
            if (m_pendingCloseEditor)
            {
                CloseEditorImpl();
            }
            else if (m_pendingCloseProject)
            {
                CloseProjectImpl();
            }

            m_pendingCloseEditor = false;
            m_pendingCloseProject = false;
            m_pendingCloseDocument = false;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Update Modals.
    bool closedModals = false;
    for (size_t i = 0; i < m_modalDialogs.size(); ++i)
    {
        m_modalDialogs[i]->UpdateModal(dt);

        if (m_modalDialogs[i]->IsClosed())
        {
            closedModals = true;
            SafeDelete(m_modalDialogs[i]);
        }
    }

    if (closedModals)
    {
        StdVectorRemove<BaseModalDialog*>(m_modalDialogs, nullptr);
    }

    // Docking panels ids.
    const char* dockspaceWindowName = "EditorDockSpace Window";
    const ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");

    // Begin Dockspace Window.
    ImGuiWindowFlags window_flags = /*ImGuiWindowFlags_MenuBar |*/ ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin(dockspaceWindowName, false, window_flags);
    ImGui::PopStyleVar(3);

    // Editor panels preset.
    bool resetDocuments = false;
    if (ImGui::DockBuilderGetNode(dockspace_id) == NULL || m_resetPanels)
    {
        resetDocuments = true;
        m_resetPanels = false;

        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
        //ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.20f, NULL, &dock_main_id);
        ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

        ImGui::DockBuilderDockWindow("Assets Explorer", dock_id_left);
        ImGui::DockBuilderDockWindow("Dependencies", dock_id_left);
        ImGui::DockBuilderDockWindow("Output Log", dock_id_down);
        ImGui::DockBuilderDockWindow("Search Results", dock_id_down);
        ImGui::DockBuilderDockWindow("Properties", dock_id_right);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    // Update standard panels.
    m_assetsExplorerPanel->UpdatePanel(dt);
    m_outputLogPanel->UpdatePanel(dt);
    m_dependenciesPanel->UpdatePanel(dt);

    //TODO: search
    //if (m_showSearchResults)
    //{
    //    ImGui::Begin("Search Results", &m_showSearchResults);
    //    ImGui::End();
    //}

    // Handle Document panels closed during last frame (I use a frame delay to avoid flickers).
    bool closedDocuments = false;
    for (size_t i = 0; i < m_documentPanels.size(); ++i)
    {
        DocumentPanel* document = m_documentPanels[i];

        if (document->IsClosing())
        {
            if (document->IsDirty())
            {
                m_pendingCloseDocument = true;
                m_checkDirtyDocuments = true;
            }
            else
            {
                document->ValidateClosing();
            }
        }

        if (document->IsClosed())
        {
            if (m_lastActiveDocument == document)
            {
                m_lastActiveDocument = nullptr;
            }

            closedDocuments = true;
            SafeDelete(m_documentPanels[i]);
        }
    }

    if (closedDocuments)
    {
        StdVectorRemove<DocumentPanel*>(m_documentPanels, nullptr);
    }

    // Update Document panels.
    for (size_t i = 0; i < m_documentPanels.size(); ++i)
    {
        DocumentPanel* document = m_documentPanels[i];

        ImGui::SetNextWindowDockID(dockspace_id, resetDocuments ? ImGuiCond_Always  : ImGuiCond_FirstUseEver);
        document->UpdatePanel(dt);

        if (document->IsFocused())
        {
            m_lastActiveDocument = document;
        }
    }

    StdVectorRemove<DocumentPanel*>(m_documentPanels, nullptr);

    // Update Properties panel.
    if (ImGui::Begin("Properties", false))
    {
        if (m_lastActiveDocument)
        {
            m_lastActiveDocument->UpdateProperties(dt);
        }
    }
    ImGui::End();

    if (m_showImGuiDemo)
    {
        ImGui::ShowDemoWindow();
    }

    // End Dockspace Window.
    ImGui::End();
}

bool Editor::OpenModalDialog(BaseModalDialog* modalDialog)
{
    if (!modalDialog)
        return false;

    m_modalDialogs.push_back(modalDialog);
    return true;
}

bool Editor::OpenDocument(const std::string& resourceID)
{
    FileInfo resourceFileInfo;
    if (!GetResources()->GetResourceFileInfo(resourceID, resourceFileInfo))
        return false;

    for (DocumentPanel* document : m_documentPanels)
    {
        if (document->IsSameResource(resourceID))
        {
            document->ForceFocus();
            return true;
        }
    }

    DocumentPanel* newDocument = nullptr;

    // Check native resource types.
    EResourceType::Type resourceType = GetResources()->GetResourceType(resourceFileInfo);
    if (resourceType == EResourceType::ImageSet)
    {
        newDocument = new ImageSetPanel(GetResources()->GetImageSet(resourceID));
    }
    else if (resourceType == EResourceType::AnimSet)
    {
        newDocument = new AnimSetPanel(GetResources()->GetAnimSet(resourceID));
    }
    else if (resourceType == EResourceType::ParticleEffect)
    {
        newDocument = new ParticleEffectPanel(GetResources()->GetParticleEffect(resourceID));
    }
    else if (resourceType == EResourceType::Texture)
    {
        newDocument = new TexturePanel(GetResources()->GetTexture(resourceID));
    }
    else if (resourceType == EResourceType::Unknown)
    {
        // Check datasheets.
        if (m_datasheetParser && m_datasheetParser->IsDatasheet(resourceFileInfo))
        {
            VirtualDatasheet* datasheet = nullptr;

            // TODO: I should encapsulate this in some kind of GetOrLoad method.
            if (GetResources()->IsResourceLoaded(resourceID))
            {
                datasheet = dynamic_cast<VirtualDatasheet*>(GetResources()->GetResource(resourceID));
            }
            else
            {
                datasheet = m_datasheetParser->InstanciateDatasheetResource(resourceID);
            }

            if (datasheet)
            {
                newDocument = new DatasheetPanel(datasheet);
            }
        }
    }

    if (newDocument)
    {
        m_documentPanels.push_back(newDocument);
    }

    return true;
}

bool Editor::CloseDocument(const std::string& resourceID, bool forceIgnoreDirty)
{
    for (DocumentPanel* document : m_documentPanels)
    {
        if (document->IsSameResource(resourceID))
        {
            bool closed = document->Close();

            if (!closed && forceIgnoreDirty)
            {
                document->ValidateClosing();
            }

            return document->IsClosed();
        }
    }

    return true;
}

const std::vector<DocumentPanel*>& Editor::GetDocuments() const
{
    return m_documentPanels;
}

DocumentPanel* Editor::GetLastActiveDocument() const
{
    return m_lastActiveDocument;
}

bool Editor::RaiseCheckDirtyDocuments()
{
    bool hasDirtyDocuments = false;
    for (DocumentPanel* document : m_documentPanels)
    {
        hasDirtyDocuments |= document->IsDirty();
    }

    m_checkDirtyDocuments |= hasDirtyDocuments;
    return m_checkDirtyDocuments;
}

void Editor::CancelClosingDirtyDocuments()
{
    for (DocumentPanel* document : m_documentPanels)
    {
        if (document->IsClosing() && document->IsDirty())
        {
            document->CancelClosing();
        }
    }
}

void Editor::ValidateClosingDirtyDocuments()
{
    for (DocumentPanel* document : m_documentPanels)
    {
        if (document->IsClosing() && document->IsDirty())
        {
            document->ValidateClosing();

            Resource* resource = document->GetResource();
            resource->LoadFromFile();

            GetResources()->UpdateResourceDependencies(resource);
        }
    }
}

bool Editor::SaveActiveDocument()
{
    if (!m_lastActiveDocument)
        return false;

    return m_lastActiveDocument->Save();
}

bool Editor::UndoActiveDocument()
{
    if (!m_lastActiveDocument)
        return false;

    return m_lastActiveDocument->Undo();
}

bool Editor::RedoActiveDocument()
{
    if (!m_lastActiveDocument)
        return false;

    return m_lastActiveDocument->Redo();
}

bool Editor::SaveAllDirtyDocuments()
{
    bool result = true;
    for (DocumentPanel* document : m_documentPanels)
    {
        if (document->IsDirty())
        {
            result &= document->Save();
        }
    }

    return result;
}

bool Editor::SaveAllClosingDirtyDocuments()
{
    bool result = true;
    for (DocumentPanel* document : m_documentPanels)
    {
        if (document->IsClosing() && document->IsDirty())
        {
            result &= document->Save();
        }
    }

    return result;
}

void Editor::RefreshAssets()
{
    m_assetsExplorerPanel->RaiseDirtyContent();
}

void Editor::ResetPanels()
{
    m_resetPanels = true;
}

bool Editor::CloseEditor()
{
    if (!RaiseCheckDirtyDocuments())
    {
        CloseEditorImpl();
        return true;
    }
    else
    {
        m_pendingCloseEditor = true;
        return false;
    }
}

void Editor::CloseEditorImpl()
{
    // TODO: Only close the application in standalone mode, prefer hiding the editor when used as an overlay.
    GetEngine()->StopMainLoop();
}

DatasheetParser* Editor::GetDatasheetParser() const
{
    return m_datasheetParser;
}

Editor* GetEditor()
{
    return Editor::GetInstance();
}

}   //namespace gugu
