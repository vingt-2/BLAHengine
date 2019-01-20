#include "GuiManager.h"

#include <Common/dirent.h>

using namespace BLAengine;


//void BlaGuiManager::FileBrowser(const std::string startingDirectory, std::string& selection)
//{
//	enum EntryType : blaU32
//	{
//		DIRECTORY = 0,
//		REGULAR_FILE
//	};
//
//	std::vector<std::string, EntryType> result;
//	DIR *dir;
//	struct dirent *ent;
//
//	dir = opendir(startingDirectory.c_str());
//	if (dir != NULL)
//	{
//		while ((ent = readdir(dir)) != NULL)
//		{
//			switch (ent->d_type)
//			{
//			case DT_DIR:
//				result.push_back(std::pair<std::string, EntryType>(ent->d_name, DIRECTORY));
//				break;
//			case DT_REG:
//				result.push_back(std::pair<std::string, EntryType>(ent->d_name, REGULAR_FILE));
//				break;
//			}
//		}
//	}
//	closedir(dir);
//}
