#pragma once

#include <Engine/Gui/GuiWindow.h>
#include <Engine/Game/GameObject.h>

struct BlaSceneGraphElement 
{
	BLAengine::GameObjectReference m_object;
};

class BlaGuiTreeElement
{
    virtual blaString GetName() = 0;
};

class BlaGuiTree
{

};