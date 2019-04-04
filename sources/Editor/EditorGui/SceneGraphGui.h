#pragma once

#include <Engine/Gui/GuiWindow.h>
#include <Engine/Game/GameObject.h>

struct BlaSceneGraphElement 
{
    GameObjectReference m_object;
};

class BlaGuiTreeElement
{
    virtual std::string GetName() = 0;
};

class BlaGuiTree
{

};