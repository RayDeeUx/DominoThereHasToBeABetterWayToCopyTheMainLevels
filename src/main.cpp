#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/BoomScrollLayer.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <Geode/modify/FLAlertLayer.hpp>

using namespace geode::prelude;

int cpage = 0;
std::string alertHeader = "";

std::vector<std::string> mainLevels = {
	"Stereo Madness",
	"Back On Track",
	"Polargeist",
	"Dry Out",
	"Base After Base",
	"Cant Let Go",
	"Jumper",
	"Time Machine",
	"Cycles",
	"xStep",
	"Clutterfunk",
	"Theory of Everything",
	"Electroman Adventure",
	"Clubstep",
	"Electrodynamix",
	"Hexagon Force",
	"Blast Processing",
	"ToE 2",
	"Geometrical Dominato",
	"Deadlocked",
	"Fingerdash",
	"Dash",
    "The Tower",
    "The Sewers",
    "The Cellar",
    "The Secret Hollow"
};

std::vector<std::string> otherAllowedLevels = {
    "The Tower",
    "The Sewers",
    "The Cellar",
    "The Secret Hollow"
};

static std::map<std::string, std::string> levelMapping = {
	{"Stereo Madness", "Stereo Madness"},
	{"Back On Track", "Back On Track"},
	{"Polargeist", "Polargeist"},
	{"Dry Out", "Dry Out"},
	{"Base After Base", "Base After Base"},
	{"Cant Let Go", "Cant Let Go"},
	{"Jumper", "Jumper"},
	{"Time Machine", "Time Machine"},
	{"Cycles", "Cycles"},
	{"xStep", "xStep"},
	{"Clutterfunk", "Clutterfunk"},
	{"Theory of Everything", "Theory of Everything"},
	{"Electroman Adventures", "Electroman Adventure"},
	{"Clubstep", "Clubstep"},
	{"Electrodynamix", "Electrodynamix"},
	{"Hexagon Force", "Hexagon Force"},
	{"Blast Processing", "Blast Processing"},
	{"Theory of Everything 2", "ToE 2"},
	{"Geometrical Dominator", "Geometrical Dominato"},
	{"Deadlocked", "Deadlocked"},
	{"Fingerdash", "Fingerdash"},
	{"Dash", "Dash"},
    {"The Tower", "The Tower"},
    {"The Cellar", "The Cellar"},
    {"The Sewers", "The Sewers"},
    {"The Secret Hollow", "The Secret Hollow"}

};

std::string fixLevelName(const std::string& inputValue) {
	for (const auto& pair : levelMapping) {
		if (pair.second == inputValue) {
			return pair.first;
		}
	}
	return inputValue;
}

class $modify(CMLbsc, BoomScrollLayer) {
	bool init(cocos2d::CCArray* pages, int offset, bool looped, cocos2d::CCArray* dynamicObjects, DynamicScrollDelegate* delegate) {
		if (!BoomScrollLayer::init(pages, offset, looped, dynamicObjects, delegate))
			return false;

		this->schedule(schedule_selector(CMLbsc::updPageN));
		return true;
	}

	void updPageN(float dt) {
		cpage = (m_page % 24 + 24) % 24;
	}
};

void importLevel(std::string const& shortName) {
	std::string fileName = fmt::format("{}.gmd", shortName);
	auto path = Mod::get()->getResourcesDir() / fileName;

	auto result = gmd::importGmdAsLevel(path.string());

	if (result.isOk()) {
		auto level = result.unwrap();
		level->m_levelType = GJLevelType::Editor;

		LocalLevelManager::sharedState()->m_localLevels->insertObject(level, 0);

		std::string fixedName = fixLevelName(shortName);
		FLAlertLayer::create(nullptr, "Success", fmt::format("Copied {}!", fixedName), "OK", nullptr, 300.f)->show();
	}
	else {
		FLAlertLayer::create(nullptr, "Error", "Import failed. Report to DominoKiddoo.", "OK", nullptr, 300.f)->show();
	}
}

class $modify(InfoPopupHook, FLAlertLayer) {
	struct Fields {
		std::string m_targetFileName;
	};

	bool init(FLAlertLayerProtocol* p0, char const* title, gd::string desc, char const* btn1, char const* btn2, float width, bool scroll, float height, float textScale) {
		if (!FLAlertLayer::init(p0, title, desc, btn1, btn2, width, scroll, height, textScale))
			return false;

        log::info("{}", desc);
    
		if (!title || !levelMapping.count(title)) {
			return true;
        }

		auto mainLayer = this->getChildByID("main-layer");
		auto bg = mainLayer ? mainLayer->getChildByID("background") : nullptr;
        alertHeader = title;
		auto cButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png"),
			this,
			menu_selector(InfoPopupHook::onCopyMainLevel)
		);

		cButton->setPosition(
			(cButton->getPositionX() - 61.f),
			cButton->getPositionY()
		);

		m_buttonMenu->addChild(cButton);
		m_buttonMenu->updateLayout();

		return true;
	}

	void onCopyMainLevel(CCObject*) {
        if (cpage != 22 && cpage != 23) {
            std::string cLevel = mainLevels[cpage];
            importLevel(cLevel);
        } else {
            if  (std::find(otherAllowedLevels.begin(), otherAllowedLevels.end(), alertHeader) != otherAllowedLevels.end()) {
                importLevel(alertHeader);
            } else {
                FLAlertLayer::create("Error", fmt::format("You are not on a page with a level!"), "OK")->show();
            }
			
		}
	}
};