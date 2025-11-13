#include <Geode/Geode.hpp>
#include "../Variables.hpp"
using namespace geode::prelude;

#include <Geode/modify/GauntletSelectLayer.hpp>
class $modify(GauntletSelectLayer) {
        #ifndef GEODE_IS_MOBILE
        void keyBackClicked() {
                if (!Variables::isFromRedash) return GauntletSelectLayer::keyBackClicked();
                CCScene* sc = MenuLayer::scene(false);
                CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, sc));
                Variables::isFromRedash = false;
        }
        #endif
	void onBack(cocos2d::CCObject* sender) {
		if (!Variables::isFromRedash) return GauntletSelectLayer::onBack(sender);
		CCScene* sc = MenuLayer::scene(false);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, sc));
		Variables::isFromRedash = false;
	}
};

#include <Geode/modify/SecretRewardsLayer.hpp>
class $modify(SecretRewardsLayer) {
	// crashing on android
	#ifndef GEODE_IS_MOBILE
	void keyBackClicked() {
		if (!Variables::isFromRedash) return SecretRewardsLayer::keyBackClicked();
		CCScene* sc = MenuLayer::scene(false);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, sc));
                Variables::isFromRedash = false;
	}
	#endif

	void onBack(cocos2d::CCObject* sender) {
                if (!Variables::isFromRedash) return SecretRewardsLayer::onBack(sender);
		CCScene* sc = MenuLayer::scene(false);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, sc));
                Variables::isFromRedash = false;
	}
};

#include <Geode/modify/SecretLayer2.hpp>
class $modify(SecretLayer2) {
	// same here
	#ifndef GEODE_IS_MOBILE
	void keyBackClicked() {
                if (!Variables::isFromRedash) return SecretLayer2::keyBackClicked();
		CCScene* sc = MenuLayer::scene(false);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, sc));
                Variables::isFromRedash = false;
	}
	#endif

	void onBack(cocos2d::CCObject* sender) {
                if (!Variables::isFromRedash) return SecretLayer2::onBack(sender);
		CCScene* sc = MenuLayer::scene(false);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, sc));
                Variables::isFromRedash = false;
	}
};
