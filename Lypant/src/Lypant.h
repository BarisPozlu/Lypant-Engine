#pragma once

// Intended to be included by lypant applications.

#include "Lypant/Event/Event.h"
#include "Lypant/Event/KeyEvent.h"
#include "Lypant/Event/MouseEvent.h"
#include "Lypant/Event/WindowEvent.h"

#include "Lypant/Input/Input.h"
#include "imgui.h"

#include "Lypant/Core/Layer.h"
#include "Lypant/Core/Application.h"
#include "Lypant/Core/EntryPoint.h"


// temp
#include <glm/glm.hpp>
#include "Lypant/Renderer/Buffer.h"
#include "Lypant/Renderer/Shader.h"
#include "Lypant/Renderer/Texture.h"
#include "Lypant/Renderer/VertexArray.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Renderer/RenderCommand.h"

#include "Lypant/Camera/EditorPerspectiveCamera.h"

#include "Lypant/Renderer/Light.h"
#include "Lypant/Renderer/Mesh.h"
#include "Lypant/Renderer/Model.h"

#include "Lypant/Scene/Scene.h"
#include "Lypant/Scene/Components.h"
#include "Lypant/Scene/BehaviorComponent.h"
#include "Lypant/Scene/Entity.h"