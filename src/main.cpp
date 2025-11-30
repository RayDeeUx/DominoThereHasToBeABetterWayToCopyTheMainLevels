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

std::vector<std::string> mainLevels = {
	"Stereo Madness",
	"Back on Track",
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
	"Dash"
};

static std::map<std::string, std::string> levelMapping = {
    {"Stereo Madness", "Stereo Madness"},
    {"Back on Track", "Back on Track"},
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
    {"Electroman Adventure", "Electroman Adventure"},
    {"Clubstep", "Clubstep"},
    {"Electrodynamix", "Electrodynamix"},
    {"Hexagon Force", "Hexagon Force"},
    {"Blast Processing", "Blast Processing"},
    {"Theory of Everything 2", "ToE 2"},          
    {"Geometrical Dominator", "Geometrical Dominato"}, 
    {"Deadlocked", "Deadlocked"},
    {"Fingerdash", "Fingerdash"},
    {"Dash", "Dash"}
};

std::string fixLevelName(const std::string& inputValue) {
	for (const auto& pair : levelMapping) {
		if (pair.second == inputValue) {
			return pair.first;
		}
	}
	return inputValue;
        level->m_levelType = GJLevelType::Editor;
        
        LocalLevelManager::sharedState()->m_localLevels->insertObject(level, 0);
		std::string cLevel = mainLevels[cpage];
		std::string message = fmt::format("Copied {}!", cLevel);

        FLAlertLayer::create("Success", message, "OK")->show();
    } 
    else {
        FLAlertLayer::create("Error", "An error occured. Please report this to @DominoKiddoo on discord", "OK")->show();
    }
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
        if (!FLAlertLayer::init(p0, title, desc, btn1, btn2, width, scroll, height, textScale)) {
            return false;
        }

		if (!title || !levelMapping.count(title))
			return true;
        if (title) {
			auto mainLayer = this->getChildByID("main-layer");
            auto bg = mainLayer ? mainLayer->getChildByID("background") : nullptr;

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

            
        }

        return true;
    }

    void onCopyMainLevel(CCObject*) {
		if (cpage != 22 && cpage != 23){
			std::string cLevel = mainLevels[cpage];
			importLevel(cLevel);

		} else {
			FLAlertLayer::create("Error", fmt::format("You are not on a page with a level!"), "OK")->show();
		}
    }
};