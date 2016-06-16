#include "../Engine/InputHandler/InputHandler.h"
#include "../Common/Maths.h"

class EditorControls
{
public:
	EditorControls();
	~EditorControls();

	void GetCameraPosition(vec3& cameraPosition);
	

	InputHandler* m_inputHandler;
};