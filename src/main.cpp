#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/BoomScrollLayer.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <Geode/binding/GJGameLevel.hpp> 
#include <Geode/binding/LocalLevelManager.hpp>
#include <vector>
#include <string>
#include <iostream>
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

void importLevel(std::string const& fileName) {
    auto path = Mod::get()->getResourcesDir() / fileName;
    auto result = gmd::importGmdAsLevel(path.string());
    if (result.isOk()) {
        auto level = result.unwrap();

        level->m_levelType = GJLevelType::Editor; // Ensures it shows in the "Create" list
        
        LocalLevelManager::sharedState()->m_localLevels->insertObject(level, 0);
		std::string cLevel = mainLevels[cpage];
		std::string message = fmt::format("Copied {}!", cLevel);

        FLAlertLayer::create("Success", message, "OK")->show();
    } 
    else {
        FLAlertLayer::create("Error", "An error occured. Please report this to @DominoKiddoo on discord", "OK")->show();
    }
}


class $modify(CMLlsl, LevelSelectLayer) {
	bool init(int page) {

		if (!LevelSelectLayer::init(page)) {
			return false;
		}

		auto cButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png"),
			this,
			menu_selector(CMLlsl::onCButton)
		);

		auto menu = this->getChildByID("info-menu");
		menu->addChild(cButton);
		
		cButton->setID("cml-button"_spr);
		cButton->setPosition(0, -38.000);
		menu->updateLayout();
		return true;
	}



	void onCButton(CCObject*) {
		if (cpage != 22 && cpage != 23){
			std::string cLevel = mainLevels[cpage];
			std::string withDotGMD = fmt::format("{}.gmd", cLevel);
			importLevel(withDotGMD);

		} else {
			FLAlertLayer::create("Error", fmt::format("You are not on a page with a level!"), "OK")->show();
		}
		
	}
};

class $modify(CMLbsc, BoomScrollLayer) {
    bool init(cocos2d::CCArray* pages, int offset, bool looped, cocos2d::CCArray* dynamicObjects, DynamicScrollDelegate* delegate) {
        if (!BoomScrollLayer::init(pages, offset, looped, dynamicObjects, delegate)) return false;

		this->schedule(schedule_selector(CMLbsc::updPageN));
        return true;
    }

	void updPageN(float dt) {
		cpage = (m_page % 24 + 24) % 24;
		// log::info("page: {}", cpage);
	}

};
