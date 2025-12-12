#include <Geode/modify/LevelAreaInnerLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/FLAlertLayer.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/binding/LocalLevelManager.hpp>

using namespace geode::prelude;

bool shouldCopy = false;
GJGameLevel* levelToCopy = nullptr;

class $modify(InfoPopupHook, FLAlertLayer) {

	bool init(FLAlertLayerProtocol* p0, char const* title, gd::string desc, char const* btn1, char const* btn2, float width, bool scroll, float height, float textScale) {
		if (!FLAlertLayer::init(p0, title, desc, btn1, btn2, width, scroll, height, textScale)) return false;
		if (!shouldCopy || !levelToCopy) return true;

		auto cButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_duplicateBtn_001.png"),
			this, menu_selector(InfoPopupHook::onCopyMainLevel)
		);

		cButton->setPosition(
			(cButton->getPositionX() - 61.f),
			cButton->getPositionY()
		);

		m_buttonMenu->addChild(cButton);
		m_buttonMenu->updateLayout();

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

class $modify(MyLevelAreaInnerLayer, LevelAreaInnerLayer) {
	void onLevelInfo(CCObject* sender) {
		LevelAreaInnerLayer::onLevelInfo(sender);
		shouldCopy = false;
		levelToCopy = nullptr;
        auto level = GameLevelManager::sharedState()->m_mainLevels->objectForKey(fmt::to_string(static_cast<CCMenuItemSpriteExtra*>(sender)->getTag()));
		if (!level || level->m_levelID.value() < 1) return;
		shouldCopy = true;
		levelToCopy = level;
    }
};