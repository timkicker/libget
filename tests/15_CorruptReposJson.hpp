#include "tests.hpp"
#include <fstream>

// A repos.json full of NUL bytes (what a FAT card can leave behind after a
// power cut) should be kept as repos.json.bad and replaced by the default.

class CorruptReposJsonTest : public Test {
	public:
	CorruptReposJsonTest() {
		purpose = "Recover from a corrupted repos.json";
	}
	bool execute()
	{
		std::string dir = "./tests/.get-corrupt/";
		std::string reposPath = dir + "repos.json";
		std::string badPath = reposPath + ".bad";

		mkpath("./tests/.get-corrupt");
		std::remove(badPath.c_str());

		{
			std::ofstream file(reposPath, std::ios::binary);
			file << std::string(200, '\0');
		}

		Get corrupt(dir, URL "a", false);
		auto& repos = corrupt.getRepos();
		if (repos.size() != 1 || repos[0]->getUrl() != URL "a")
		{
			error << "Expected only the default repo, found " << repos.size() << " repos";
			return false;
		}

		if (!exists(badPath.c_str()))
		{
			error << "The broken file wasn't kept as " << badPath;
			return false;
		}

		// the new file on disk has to load again
		Get reloaded(dir, URL "a", false);
		if (reloaded.getRepos().size() != 1)
		{
			error << "The regenerated repos.json didn't load again";
			return false;
		}

		return true;
	}
};
