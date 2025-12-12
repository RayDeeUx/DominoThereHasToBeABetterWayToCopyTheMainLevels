#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/FLAlertLayer.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <Geode/binding/LevelAreaInnerLayer.hpp>

using namespace geode::prelude;

bool shouldCopy = false;
GJGameLevel* levelToCopy = nullptr;

constexpr std::array<std::pair<std::string_view,int>, 4> table{{
	{"The Tower", 5001},
	{"The Sewers", 5002},
	{"The Cellar", 5003},
	{"The Secret Hollow", 5004}
}};

constexpr const int* levelNameToLevelID(std::string_view key) {
	for (auto const& kv : table) {
		if (kv.first == key) return &kv.second;
	}
	return nullptr;
}

class $modify(InfoPopupHook, FLAlertLayer) {

	struct Fields {
		~Fields() {
			shouldCopy = false;
			levelToCopy = nullptr;
		}
	};

	bool init(FLAlertLayerProtocol* p0, char const* title, gd::string desc, char const* btn1, char const* btn2, float width, bool scroll, float height, float textScale) {
		if (!FLAlertLayer::init(p0, title, desc, btn1, btn2, width, scroll, height, textScale)) return false;
		if (!title) return true;
		if (!shouldCopy || !levelToCopy) {
			log::info("checking if in the tower");
			if (!CCScene::get() || !CCScene::get()->getChildByType<LevelAreaInnerLayer>(0)) {
				log::info("not in the tower");
				return true;
			}

			auto towerLevelID = levelNameToLevelID(title);
			if (!towerLevelID || *towerLevelID < 5001) {
				log::info("level ID not found in map");
				return true;
			}

			auto level = typeinfo_cast<GJGameLevel*>(GameLevelManager::sharedState()->m_mainLevels->objectForKey(fmt::to_string(*towerLevelID)));
			if (!level || level->m_levelID.value() < 1) {
				log::info("no level found or level ID less than 1");
				return true;
			}

			log::info("tower level found let's goooo");
			shouldCopy = true;
			levelToCopy = level;
		}

		auto cButton = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Copy Level", 80, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f),
			this, menu_selector(InfoPopupHook::onCopyMainLevel)
		);

		cButton->setPositionX(cButton->getPositionX() - 61.f);

		m_buttonMenu->addChild(cButton);

		return true;
	}

	void onCopyMainLevel(CCObject* sender) {
        if (!shouldCopy || !levelToCopy) return;
		// code adapted from zmx/qimiko under the mozilla public license 2.0

		if (levelToCopy->m_levelString.empty()) {
			levelToCopy->m_levelString = LocalLevelManager::sharedState()->getMainLevelString(levelToCopy->m_levelID);
		}

		EditLevelLayer::create(levelToCopy)->onClone();

		shouldCopy = false;
		levelToCopy = nullptr;
	}
};

class $modify(MyLevelPage, LevelPage) {
	void onInfo(CCObject* sender) {
		LevelPage::onInfo(sender);
		shouldCopy = false;
		levelToCopy = nullptr;
		if (!m_level || m_level->m_levelID.value() < 1) return;
		shouldCopy = true;
		levelToCopy = m_level;
	}
};