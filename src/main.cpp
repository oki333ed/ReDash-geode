#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/ChallengesPage.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/cocos.hpp>

#include "Variables.hpp"

#include "ui/RDButton.hpp"
#include "ui/RDStatsNode.hpp"
#include "ui/RDMainButton.hpp"
#include "ui/timely/RDTimelyNode.hpp"
#include "ui/timely/RDDailyNode.hpp"
#include "ui/timely/RDWeeklyNode.hpp"
#include "ui/timely/RDEventNode.hpp"

#include "settings/StatsSettingV3.hpp"
#include "settings/StatsSettingNodeV3.hpp"
#include "settings/MainButtonsSettingV3.hpp"
#include "settings/MainButtonsSettingNodeV3.hpp"

#include <fmt/core.h>
#include <Macros.hpp>

#include <alphalaneous.pages_api/include/PageMenu.h>

std::string getPathString(int n) {
	switch (n) {
		case 1: return "Fire";
		case 2: return "Ice";
		case 3: return "Poison";
		case 4: return "Shadow";
		case 5: return "Lava";
		case 6: return "Earth";
		case 7: return "Blood";
		case 8: return "Metal";
		case 9: return "Light";
		case 10: return "Souls";
		case 100: return "All";
		default: return "None";
	}
}

std::string abbreviateNumber(int n) {
	if (n < 1000) return std::to_string(n);
	if (n < 1000000) return fmt::format("{:.1f}k", n / 1000.f);
	if (n < 1000000000) return fmt::format("{:.1f}m", n / 1000000.f);
	return fmt::format("{:.1f}b", n / 1000000000.f);
}

class RDButtonData {
public:
	std::string m_titleSpr;
	std::vector<std::string> m_description;
	std::string m_iconSpr;
	float m_iconScale;
	SEL_MenuHandler m_selector;

	RDButtonData() {}

	RDButtonData(std::string titleSpr, std::initializer_list<std::string> description, std::string iconSpr, float iconScale, SEL_MenuHandler selector) {
		m_titleSpr = titleSpr;
		m_description = description;
		m_iconSpr = iconSpr;
		m_iconScale = iconScale;
		m_selector = selector;
	}
};

class $modify(CrazyLayer, MenuLayer) {
	struct Fields {
		CCMenuItemSpriteExtra* m_questBtn;
		CCMenu* m_menu;
		CCMenu* m_mainMenu;
		CCMenu* m_timeliesMenu;
		CCMenu* m_statsMenu;
		CCMenu* m_bottomMenu;
		CCMenu* m_topMenu;
	};

	static void onModify(auto& self) {
		(void) self.setHookPriorityAfterPost("MenuLayer::init", "alphalaneous.vanilla_pages");
		(void) self.setHookPriorityAfterPost("MenuLayer::init", "devcmb.cleanermenu");
	}

	void setupButtons() {
		auto loader = Loader::get();
		auto bottomMenu = this->getChildByID("bottom-menu");
		auto rightMenu = this->getChildByID("right-side-menu");

		if (loader->isModLoaded("geode.texture-loader") && loader->isModLoaded("undefined0.gdtweaks")) {
			if (loader->getLoadedMod("undefined0.gdtweaks")->getSettingValue<bool>("replace-more-games-w-texture")) {
				if (auto button = this->getChildByIDRecursive("geode.texture-loader/texture-loader-button")) {
					rightMenu->addChild(button);
				}
			}
		}

		if (auto closeMenu = this->getChildByID("close-menu")) {
			if (!closeMenu->getChildByID("close-button")) {
				auto cls_spr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
				cls_spr->setScale(0.7);
				auto cls_btn = CCMenuItemSpriteExtra::create(
					cls_spr, 
					this, 
					menu_selector(MenuLayer::onQuit));
				cls_btn->setID("close-button");
				closeMenu->addChild(cls_btn);
				closeMenu->updateLayout();
			}

			if (loader->isModLoaded("weebify.restartbtn")) {
				if (auto restartBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(closeMenu->getChildByID("restart-button"))) {
					closeMenu->removeChild(restartBtn);
					bottomMenu->addChild(restartBtn);
					if (auto sprite = restartBtn->getChildByType<CCSprite>(0)) {
						restartBtn->setContentSize(restartBtn->getContentSize() * 1.5f);
						sprite->setScale(sprite->getScale() * 1.5f);
						sprite->setPosition(restartBtn->getContentSize() / 2);
					}
				}
				if (auto reloadBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(closeMenu->getChildByID("reload-button"))) {
					closeMenu->removeChild(reloadBtn);
					bottomMenu->addChild(reloadBtn);
					if (auto sprite = reloadBtn->getChildByType<CCSprite>(0)) {
						reloadBtn->setContentSize(reloadBtn->getContentSize() * 1.5f);
						sprite->setScale(sprite->getScale() * 1.5f);
						sprite->setPosition(reloadBtn->getContentSize() / 2);
					}
				}
			}

			if (loader->isModLoaded("smjs.gdintercept")) {
				auto id = "smjs.gdintercept/GDI";
				if (loader->getLoadedMod("smjs.gdintercept")->getVersion() < VersionInfo::parse("v0.5.0-alpha.5").unwrapOrDefault()) id = "smjs.gdintercept/blame-overcharged-menu";
				if (auto node = this->getChildByIDRecursive(id)) {
					CCMenuItemSpriteExtra* button = nullptr;
					if (typeinfo_cast<CCSprite*>(node)) button = static_cast<CCMenuItemSpriteExtra*>(node->getParent());
					else if (typeinfo_cast<CCMenuItemSpriteExtra*>(node)) button = static_cast<CCMenuItemSpriteExtra*>(node);

					if (button) {
						button->removeFromParent();
						bottomMenu->addChild(button);
						if (auto sprite = button->getChildByType<CCSprite>(0)) {
							button->setContentSize(button->getContentSize() * 1.5f);
							sprite->setScale(sprite->getScale() * 1.5f);
							sprite->setPosition(button->getContentSize() / 2);
						}
					}
				} else {
					if (loader->getLoadedMod("smjs.gdintercept")->getVersion() < VersionInfo::parse("v0.4.0-alpha.0").unwrapOrDefault()) {
						auto alert = geode::createQuickPopup(
							"Uh oh!",
							"It looks like your GDIntercept's version is below v0.4.0-alpha.0.\nPlease update it to fix the issue with the button (top left corner).",
							"Cancel", "OK",
							[loader](auto, bool btn2) {
								if (btn2) {
									geode::openInfoPopup(loader->getLoadedMod("smjs.gdintercept"));
								}
							},
							false
						);
						alert->m_scene = this;
						alert->show();
					}
				}
			}
		}

		RD_ADD_CREATOR_BUTTON("cvolton.betterinfo", "cvolton.betterinfo/main-button", Variables::BISelector, "RD_betterinfo.png"_spr);
		RD_ADD_CREATOR_BUTTON("xanii.super_expert", "super-expert-button", Variables::SupExSelector, "RD_superexpert.png"_spr);
		RD_ADD_CREATOR_BUTTON("minemaker0430.gddp_integration", "demon-progression-button", Variables::GDDPSelector, Mod::get()->getSettingValue<bool>("alt-gddp-texture") ? "RD_gddp2.png"_spr : "RD_gddp.png"_spr);
		RD_ADD_CREATOR_BUTTON("spaghettdev.gd-roulette", "spaghettdev.gd-roulette/roulette-button", Variables::RouletteSelector, "RD_roulette.png"_spr);
		RD_ADD_CREATOR_BUTTON("rainixgd.geome3dash", "rainixgd.geome3dash/map-button", Variables::G3DSelector, "RD_geome3dash.png"_spr);

		#ifdef GEODE_IS_MOBILE
		if (loader->isModLoaded("geode.devtools")) {
			auto btn = CCMenuItemSpriteExtra::create(
				CircleButtonSprite::createWithSprite("RD_devtools.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::MediumAlt),
				this,
				menu_selector(MenuLayer::onMoreGames)
			);
			btn->setID("devtools-button");
			bottomMenu->addChild(btn);
		}
		#endif
	}

	void onHideMenu(CCObject* sender) {
		bool toggle = static_cast<CCMenuItemToggler*>(sender)->isOn();

		m_fields->m_mainMenu->setVisible(toggle);
		m_fields->m_timeliesMenu->setVisible(toggle);
	}

	bool init() {
		if (!MenuLayer::init()) return false;

		auto loader = Loader::get();
		auto mod = Mod::get();
		auto gsm = GameStatsManager::sharedState();
		auto glm = GameLevelManager::sharedState();
		auto gm = GameManager::sharedState();
		auto winSize = CCDirector::sharedDirector()->getWinSize();
		
		if (Variables::WeeklyLeft < 1) {
			glm->getGJDailyLevelState(GJTimedLevelType::Weekly);
		}
		if (Variables::DailyLeft < 1) {
			glm->getGJDailyLevelState(GJTimedLevelType::Daily);
		}
		if (Variables::EventLeft < 1) {
			glm->getGJDailyLevelState(GJTimedLevelType::Event);
		}		

		if (!Mod::get()->getSettingValue<bool>("hide-bottom-buttons-texts")) {
			if (GJAccountManager::get()->m_accountID == 0) {
				Variables::GlobalRank = -1;
			} else if (Variables::OldStarsCount != gsm->getStat("6")) {
				Variables::GlobalRank = 0;
				glm->getLeaderboardScores("leaderboard_global");
			}
		}

		CrazyLayer::setupButtons();		

		// MAIN MENU CHANGES (MIGHT BE BREAKING SOME STUFF) - ninXout
		// no it ain't - Weebify
		if (loader->isModLoaded("sofabeddd.geometry_dash")) {
			RD_SETVISIBLE_SAFE(this, "sofabeddd.geometry_dash/main-title-menu", false);
		} else {
			RD_SETVISIBLE_SAFE(this, "main-title", false);
		}

		if (loader->isModLoaded("shineua.minecraft_splash")) {
			RD_SETVISIBLE_SAFE(this, "minecraft-splash", false);
		}

		RD_SETVISIBLE_SAFE(this, "more-games-menu", false);
		RD_SETVISIBLE_SAFE(this, "social-media-menu", false);
		RD_SETVISIBLE_SAFE(this, "main-menu", false);

		RD_SETVISIBLE_SAFE(this, "level-editor-hint", false);
		RD_SETVISIBLE_SAFE(this, "character-select-hint", false);

		// controller glyphs
		RD_SETVISIBLE_SAFE(this, "play-gamepad-icon", false);
		RD_SETVISIBLE_SAFE(this, "editor-gamepad-icon", false);
		RD_SETVISIBLE_SAFE(this, "icon-kit-gamepad-icon", false);
		RD_SETVISIBLE_SAFE(this, "settings-gamepad-icon", false);
		RD_SETVISIBLE_SAFE(this, "mouse-gamepad-icon", false);
		RD_SETVISIBLE_SAFE(this, "mouse-gamepad-label", false);
		RD_SETVISIBLE_SAFE(this, "click-gamepad-icon", false);
		RD_SETVISIBLE_SAFE(this, "click-gamepad-label", false);

		auto bottomMenu = this->getChildByID("bottom-menu");
		bottomMenu->setScale(0.75f);
		bottomMenu->setLayout(
			ColumnLayout::create()
				->setAxisReverse(true)
				->setAutoScale(true)
				->setGap(0.f)
				->setCrossAxisOverflow(false)
		);
		bottomMenu->setPositionX(winSize.width - 29.f);
		bottomMenu->setContentWidth(60.f);
		if (GJAccountManager::get()->m_accountID == 0) {
			bottomMenu->setPositionY(winSize.height/2);
			bottomMenu->setContentHeight(385.f);
		} else {
			bottomMenu->setPositionY(winSize.height/2 + 30.f);
			bottomMenu->setContentHeight(305.f);
		}
		bottomMenu->updateLayout();
		bottomMenu->setZOrder(1);
		static_cast<PageMenu*>(bottomMenu)->setPaged(5, PageOrientation::VERTICAL, bottomMenu->getContentHeight() - 2, -13);

		auto rightMenu = this->getChildByID("right-side-menu");
		rightMenu->setPosition(ccp(157.5f, 25.f));
		rightMenu->setScale(0.75f);
		rightMenu->setLayout(
			RowLayout::create()
				->setAxisAlignment(AxisAlignment::Start)
				->setAxisReverse(true)
				->setAutoScale(true)
				->setCrossAxisOverflow(false)
		);
		rightMenu->setContentWidth(425.f);
		rightMenu->setContentHeight(60.f);
		rightMenu->updateLayout();
		rightMenu->getChildByID("daily-chest-button")->setZOrder(4);
                static_cast<PageMenu*>(rightMenu)->setPaged(7, PageOrientation::HORIZONTAL, 315, -13);

		auto playerUsername = this->getChildByID("player-username");
		playerUsername->setScale(playerUsername->getScale() - 0.1f);
		playerUsername->setPositionX(bottomMenu->getPositionX());
		playerUsername->setPositionY(bottomMenu->getPositionY() - bottomMenu->getScaledContentHeight()/2 - playerUsername->getScaledContentHeight()/2 + 2.9f);
		if (playerUsername->getScaledContentWidth() > 54.f) {
			playerUsername->setScale(54.f / playerUsername->getContentWidth());
		}
		auto profileMenu = this->getChildByID("profile-menu");
		profileMenu->setScale(0.75f);
		profileMenu->setPositionX(playerUsername->getPositionX() + profileMenu->getScaledContentWidth()/2 - 20.625f);
		profileMenu->setPositionY(playerUsername->getPositionY() - playerUsername->getScaledContentHeight()/2 - profileMenu->getScaledContentHeight()/2 - 1.f);
		profileMenu->setZOrder(1);
		profileMenu->updateLayout();

		// NEW STUFF YAYY :D - ninXout
		// yay - Weebify

		// background for the menu on the right (the one with the Geode and settings buttons)

		CCScale9Sprite* bottomMenuBG = CCScale9Sprite::create("square02b_001.png");
		bottomMenuBG->setID("bottom-menu-bg"_spr);
		bottomMenuBG->setColor({0,0,0});
		bottomMenuBG->setOpacity(100);
		bottomMenuBG->setPosition(bottomMenu->getPosition());
		bottomMenuBG->setPositionY(CCDirector::get()->getWinSize().height / 2.f);
		bottomMenuBG->setContentSize({54.f, 302.f});
		addChild(bottomMenuBG);

		auto menu = CCMenu::create();
		menu->setID("redash-menu"_spr);
		menu->setPosition({ 0.f , 0.f });
		this->addChild(menu, 99);
		m_fields->m_menu = menu;

		CCMenu* mainMenu = CCMenu::create();
		mainMenu->setID("main-menu"_spr);
		mainMenu->setContentSize({675.f, 100.f});
		mainMenu->ignoreAnchorPointForPosition(false);
		mainMenu->setScale(0.75f);
		mainMenu->setPositionX((winSize.width - 56.f - mainMenu->getScaledContentWidth())/2 + mainMenu->getScaledContentWidth()/2);
		mainMenu->setPositionY(winSize.height/2 - 55.f);
		mainMenu->setLayout(
			RowLayout::create()
				->setGap(10.f)
				->setGrowCrossAxis(true)
				->setAutoScale(false)
		);
		m_fields->m_mainMenu = mainMenu;

		int activePath = gsm->m_activePath;
		int pathProgress = gsm->getStat(std::to_string(activePath).c_str());

		if (pathProgress > 1000) pathProgress = 1000;
		if (activePath == 0) {
			bool completedAll = true;
			for (int i = 1; i < 11; i++) {
				if (gsm->getStat(std::to_string(29 + i).c_str()) < 1000) {
					completedAll = false;
					break;
				}
			}
			if (completedAll) {
				activePath = 129;
				pathProgress = 1000;
			}
		}

		int questsCompleted = 0;
		for (int i = 1; i <= 3; i++) {
			if (auto challenge = gsm->getChallenge(i)) {
				if (challenge->m_canClaim) {
					questsCompleted++;
				}
			}
		}

		std::map<std::string, RDButtonData> mainButtons = {
			{"create-button", RDButtonData("RD_createLabel.png"_spr, {"You have", fmt::format("{} Levels", abbreviateNumber(LocalLevelManager::get()->m_localLevels->count())).c_str()}, "RD_create.png"_spr, 0.95f, menu_selector(CreatorLayer::onMyLevels))},
			{"saved-button", RDButtonData("RD_savedLabel.png"_spr, {"You have", fmt::format("{} Saved", abbreviateNumber(glm->getSavedLevels(false, 0)->count())), "Levels"}, "RD_saved.png"_spr, 0.95f, menu_selector(CreatorLayer::onSavedLevels))},
			{"paths-button", RDButtonData("RD_pathsLabel.png"_spr, {getPathString(activePath - 29), fmt::format("{}/1000", pathProgress)}, "RD_paths.png"_spr, 0.8f, menu_selector(CreatorLayer::onPaths))},
			{"leaderboards-button", RDButtonData("RD_leaderboardsLabel.png"_spr, {"Global", fmt::format("#{}", Variables::GlobalRank)}, "RD_leaderboards.png"_spr, 0.85f, menu_selector(CreatorLayer::onLeaderboards))},
			{"gauntlets-button", RDButtonData("RD_gauntletsLabel.png"_spr, {"Force", "Gauntlet", "Added"}, "RD_gauntlets.png"_spr, 1.f, menu_selector(CreatorLayer::onGauntlets))},
			{"featured-button", RDButtonData("RD_featuredLabel.png"_spr, {"Play new", "Featured", "levels"}, "RD_featured.png"_spr, 0.95f, menu_selector(CreatorLayer::onFeaturedLevels))},
			{"lists-button", RDButtonData("RD_listsLabel.png"_spr, {"Play rated", "Lists"}, "RD_lists.png"_spr, 1.f, menu_selector(CreatorLayer::onTopLists))},
			{"search-button", RDButtonData("RD_searchLabel.png"_spr, {"Search" , "For levels", "online"}, "RD_search.png"_spr, 0.9f, menu_selector(CreatorLayer::onOnlineLevels))},
			{"mappacks-button", RDButtonData("RD_mappacksLabel.png"_spr, {fmt::format("{}/{}", gsm->getCompletedMapPacks()->count(), gsm->m_completedMappacks->count()), "Map Packs", "Completed"}, "RD_mappacks.png"_spr, 0.75f, menu_selector(CreatorLayer::onMapPacks))},
			{"quests-button", RDButtonData("RD_questsLabel.png"_spr, {fmt::format("{}/3 Quest(s)", questsCompleted), "Completed"}, "RD_quests.png"_spr, 1.f, menu_selector(CreatorLayer::onChallenge))},
			{"the-map-button", RDButtonData("RD_mapLabel.png"_spr, {"Coming soon", "in 2.21!"}, "RD_map.png"_spr, 0.95f, menu_selector(CreatorLayer::onAdventureMap))},
			{"versus-button", RDButtonData("RD_versusLabel.png"_spr, {"Coming soon", "in 2.21!"}, "RD_versus.png"_spr, 0.95f, menu_selector(CreatorLayer::onMultiplayer))}
		};

		auto mainButtonsSetting = static_pointer_cast<MainButtonsSettingV3>(mod->getSetting("main-buttons-selection"));
		for (auto& button : mainButtonsSetting->getButtons()) {
			auto id = button.asString().unwrapOrDefault();
			if (mainButtons.find(id) != mainButtons.end()) {
				auto data = mainButtons[id];
				auto rdBtn = RDButton::create(this, data.m_titleSpr, data.m_description, data.m_iconSpr, data.m_iconScale, data.m_selector, id);
				if (id == "quests-button") rdBtn->m_completedQuests = questsCompleted;
				mainMenu->addChild(rdBtn);
			}
		}

		for (auto& b : mainButtonsSetting->getNonButtons()) {
			auto id = b.asString().unwrapOrDefault();
			if (id != "the-map-button" && id != "versus-button") {
				if (mainButtons.find(id) != mainButtons.end()) {
					auto data = mainButtons[id];
					auto button = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName((data.m_iconSpr.substr(0, data.m_iconSpr.length() - 4) + "2.png").c_str()), this, data.m_selector);
					button->m_baseScale = 0.9;
					button->setScale(0.9);
					button->setID(id);
					rightMenu->addChild(button);

					if (id == "quests-button") m_fields->m_questBtn = button;
				}
			}
		}

		if (m_fields->m_questBtn) {
			auto piVar2 = CCSprite::createWithSpriteFrameName("exMark_001.png");
			piVar2->setScale(0.55f);
			piVar2->setVisible(false);
			piVar2->setPosition(m_fields->m_questBtn->getContentSize() - ccp(4,8));
			piVar2->setID("quest-notif"_spr);
			m_fields->m_questBtn->addChild(piVar2, 1);
			
			if (questsCompleted > 0) {
				piVar2->setVisible(true);
			}
		}
		
		mainMenu->updateLayout();
		menu->addChild(mainMenu);

		if (mod->getSettingValue<bool>("rotate-buttons-logos")) {
			for (auto& button: CCArrayExt<RDButton*>(mainMenu->getChildren())) {
				if (button->getPositionX() < mainMenu->getContentWidth()/2) {
					button->rotateIcon(10.f);
				} else if (button->getPositionX() > mainMenu->getContentWidth()/2) {
					button->rotateIcon(-10.f);
				} else {
					button->rotateIcon(0.f);
				}
			}
		}

		auto dailiesMenu = CCMenu::create();
		dailiesMenu->setID("dailies-menu"_spr);
		dailiesMenu->setContentSize({ 630.f , 135.f });
		dailiesMenu->ignoreAnchorPointForPosition(false);
		dailiesMenu->setPositionX(mainMenu->getPositionX());
		dailiesMenu->setPositionY(winSize.height/2 + 59.375f);
		dailiesMenu->setScale(0.75f);
		dailiesMenu->setLayout(
			RowLayout::create()
				->setGap(10.f)
				->setAutoScale(false)
		);
		m_fields->m_timeliesMenu = dailiesMenu;

		// if (Mod::get()->getSettingValue<bool>("preview-2.21")) {
			dailiesMenu->addChild(RDMainButton::create({ 25.f , 0.f }, { 230.f , 152.7f }, "main-levels-button", 600/(230*4.f)));
			dailiesMenu->addChild(RDDailyNode::create({ 230.f , 152.7f }, "daily-node", 600/(230*4.f)));
			dailiesMenu->addChild(RDWeeklyNode::create({ 230.f , 152.7f }, "weekly-node", 600/(230*4.f)));
			dailiesMenu->addChild(RDEventNode::create({ 230.f , 152.7f }, "event-node", 600/(230*4.f)));
			mainMenu->setPositionY(mainMenu->getPositionY() + 12.5f);
		// } else {
			// if (Mod::get()->getSettingValue<bool>("main-levels-leftmost")) {
			// 	dailiesMenu->addChild(RDMainButton::create({ 25.f , 0.f }, { 150.f , 135.f }, "main-levels-button", 1.f));
			// 	dailiesMenu->addChild(RDTimelyNode::create(0, { 230.f , 135.f }, "daily-node", 1.f));
			// } else {
			// 	dailiesMenu->addChild(RDTimelyNode::create(0, { 230.f , 135.f }, "daily-node", 1.f));
			// 	dailiesMenu->addChild(RDMainButton::create({ 265.f , 0.f }, { 150.f , 135.f }, "main-levels-button", 1.f));
			// }
			// dailiesMenu->addChild(RDTimelyNode::create(1, { 230.f , 135.f }, "weekly-node", 1.f));
		// }


		dailiesMenu->updateLayout();
		menu->addChild(dailiesMenu);

		if (winSize.width < 562.25f) {
			auto newWidth = winSize.width - 56;
			dailiesMenu->setScale(0.75f * newWidth / 506.25f);
			dailiesMenu->setPositionX(newWidth / 2);
			mainMenu->setScale(dailiesMenu->getScale());
			mainMenu->setPositionX(dailiesMenu->getPositionX());
			mainMenu->setPositionY(dailiesMenu->getPositionY() - dailiesMenu->getScaledContentHeight()/2 - 7.5f - mainMenu->getScaledContentHeight()/2);
		}

		auto statsMenu = CCMenu::create();
		statsMenu->setID("stats-menu"_spr);
		statsMenu->setContentSize({ winSize.width/0.6f , 25.f });
		statsMenu->ignoreAnchorPointForPosition(false);
		statsMenu->setAnchorPoint({ 0.f, 0.5f });
		statsMenu->setPosition({ 52.f , winSize.height - 6.25f });
		statsMenu->setScale(0.6f);
		statsMenu->setLayout(
			RowLayout::create()
				->setAxisAlignment(AxisAlignment::Start)
				->setGap(25.f)
				->setAutoScale(false)
		);
		m_fields->m_statsMenu = statsMenu;

		std::map<std::string, std::pair<std::string, std::string>> statNodes = {
			{"stars-stats", {fmt::format("{}", gsm->getStat("6")), "GJ_starsIcon_001.png"}},
			{"moons-stats", {fmt::format("{}", gsm->getStat("28")), "GJ_moonsIcon_001.png"}},
			{"secret-coins-stats", {fmt::format("{}/{}", gsm->getStat("8"), MAX_SECRET_COINS), "GJ_coinsIcon_001.png"}},
			{"user-coins-stats", {fmt::format("{}", gsm->getStat("12")), "GJ_coinsIcon2_001.png"}},
			{"demons-stats", {fmt::format("{}", gsm->getStat("5")), "GJ_demonIcon_001.png"}},
			{"diamonds-stats", {fmt::format("{}", gsm->getStat("13")), "GJ_diamondsIcon_001.png"}},
			{"diamond-diamonds-stats", {fmt::format("{}", gsm->getStat("29")), "currencyDiamondIcon_001.png"}},
			{"orbs-stats", {fmt::format("{}", gsm->getStat("14")), "currencyOrbIcon_001.png"}}
		};
		auto statsSettings = static_pointer_cast<StatsSettingV3>(mod->getSetting("stats-nodes-selection"));
		for (auto& stat : statsSettings->getStats()) {
			auto id = stat.asString().unwrapOr("");
			if (statNodes.find(id) != statNodes.end()) {
				auto& [desc, sprite] = statNodes[id];
				statsMenu->addChild(RDStatsNode::create(sprite, desc, id));
			}
		}

		statsMenu->updateLayout();
		menu->addChild(statsMenu);

		auto menuButUnder = CCMenu::create();
		menuButUnder->setID("bottom-menu"_spr);
		menuButUnder->setAnchorPoint({ 1.f, 0.5f });
		menuButUnder->setContentSize({ 150.f, 48.25f });
		menuButUnder->setPosition({ winSize.width - 124.f, 25.f }); // ...
		auto bottomX = winSize.width - 124.f;
		menuButUnder->setScale(0.75f);
		menuButUnder->setLayout(
			RowLayout::create()
				->setAxisReverse(true)
				->setAxisAlignment(AxisAlignment::End)
				->setAutoScale(false)
				->setGap(0.f)
		);
		m_fields->m_bottomMenu = menuButUnder;

		if (!Mod::get()->getSettingValue<bool>("disable-creator-button")) {
			auto creatorSpr = CCSprite::createWithSpriteFrameName("GJ_creatorBtn_001.png");
			creatorSpr->setScale(0.9f);
			auto creatorBtn = CCMenuItemSpriteExtra::create(creatorSpr, this, menu_selector(MenuLayer::onCreator));
			creatorBtn->setID("creator-layer-button");
			menuButUnder->addChild(creatorBtn);
			bottomX -= creatorBtn->getScaledContentWidth()*menuButUnder->getScale();
		}
		auto treasureSpr = CCSprite::createWithSpriteFrameName(gm->getUGV("5") ? "secretDoorBtn_open_001.png" : "secretDoorBtn_closed_001.png");
		treasureSpr->setScale(1.25f);
		auto treasureBtn = CCMenuItemSpriteExtra::create(treasureSpr, this, menu_selector(CreatorLayer::onTreasureRoom));
		treasureBtn->setID("treasure-room-button");
		menuButUnder->addChild(treasureBtn);
		bottomX -= treasureBtn->getScaledContentWidth()*menuButUnder->getScale();

		menuButUnder->updateLayout();
		rightMenu->setContentWidth((bottomX - rightMenu->getPositionX() + rightMenu->getScaledContentWidth()/2) / rightMenu->getScale());
		rightMenu->setPositionX(bottomX - rightMenu->getScaledContentWidth()/2);

		menu->addChild(menuButUnder);

		auto topMenu = CCMenu::create();
		topMenu->setID("top-menu"_spr);
		topMenu->setAnchorPoint({ 1.f, 0.5f});
		topMenu->setContentSize({ 100.f, 48.25f });
		topMenu->setPosition({ winSize.width - 79.f, winSize.height - 20.f });
		auto topX = winSize.width - 79.f;
		topMenu->setScale(0.75f);
		topMenu->setLayout(
			RowLayout::create()
				->setAxisReverse(true)
				->setAxisAlignment(AxisAlignment::End)
				->setAutoScale(false)
				->setGap(10.f)
		);
		m_fields->m_topMenu = topMenu;

		auto garageSpr = CCSprite::createWithSpriteFrameName("garageRope_001.png");
		auto garageButton = CCMenuItemSpriteExtra::create(garageSpr, this, menu_selector(MenuLayer::onGarage));
		garageButton->setID("garage-rope");
		garageButton->setScale(4/3);
		topMenu->addChild(garageButton);
		topX -= 7.5;
		topX -= garageButton->getScaledContentWidth()*topMenu->getScale();

		auto vaultBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName(gsm->getStat("13") > 49 ? "GJ_lock_open_001.png" : "GJ_lockGray_001.png"), this, menu_selector(CreatorLayer::onSecretVault));
		vaultBtn->setID("secret-vault-button");
		topMenu->addChild(vaultBtn);
		topX -= 7.5;
		topX -= vaultBtn->getScaledContentWidth()*topMenu->getScale();

		topMenu->updateLayout();
		auto statsX = statsMenu->getPositionX();
		for (auto& statNode : CCArrayExt<RDStatsNode*>(statsMenu->getChildren())) {
			statsX += (statNode->getScaledContentWidth() + 25.f)*statsMenu->getScale();
		}
		statsX -= 25.f*statsMenu->getScale();
		if (statsX > topX) {
			statsMenu->setScale(0.6f * (topX - statsMenu->getPositionX()) / (statsX - statsMenu->getPositionX()));
		}

		garageButton->m_animationType = MenuAnimationType::Move;
		garageButton->m_startPosition = garageSpr->getPosition();
		garageButton->m_offset = ccp(0.f, -8.f);

		menu->addChild(topMenu);

		auto hideBtnMenu = CCMenu::create();
		hideBtnMenu->setID("hide-button-menu"_spr);
		hideBtnMenu->setAnchorPoint({ 1.f, 0.5f });
		hideBtnMenu->setContentSize({ 48.25f, 48.25f });
		hideBtnMenu->setPosition({ winSize.width - 129.f, 0.f });
		hideBtnMenu->setScale(0.75f);
		this->addChild(hideBtnMenu, 100);

		auto spr1 = CircleButtonSprite::createWithSprite("RD_hide_01.png"_spr, 1.45f, CircleBaseColor::Green, CircleBaseSize::MediumAlt);
		auto spr2 = CircleButtonSprite::createWithSprite("RD_hide_02.png"_spr, 1.45f, CircleBaseColor::Green, CircleBaseSize::MediumAlt);
		// spr2->setOpacity(50);
		auto hideToggler = CCMenuItemToggler::create(spr1, spr2, this, menu_selector(CrazyLayer::onHideMenu));
		hideToggler->setID("hide-button");
		hideToggler->setPosition({ hideBtnMenu->getContentWidth() / 2.f, hideBtnMenu->getContentHeight() / 2.f });
		hideBtnMenu->addChild(hideToggler);

		if (auto pagesMod = loader->getLoadedMod("alphalaneous.vanilla_pages")) {
			if (pagesMod->getSettingValue<bool>("menulayer-bottom-menu")) {
				bottomMenu->setScale(0.975);
				bottomMenu->setContentHeight(bottomMenu->getContentHeight() - 90.f);
				bottomMenu->setUserObject("orientation", CCInteger::create(0)); // VERTICAL
				bottomMenu->setUserObject("element-count", CCInteger::create(5));	
			}
			
			if (pagesMod->getSettingValue<bool>("menulayer-right-menu")) {
				rightMenu->setLayout(static_cast<RowLayout*>(rightMenu->getLayout())->setAutoScale(false)->setAxisAlignment(AxisAlignment::Center));
				rightMenu->setScale(0.95);
				rightMenu->setContentWidth(rightMenu->getContentWidth() - 75.f);
				rightMenu->setUserObject("orientation", CCInteger::create(1)); // HORIZONTAL
				rightMenu->setUserObject("element-count", CCInteger::create((int)((rightMenu->getContentWidth() + 75.f) / 60.f)));
			}
		}

		bottomMenu->updateLayout();
		rightMenu->updateLayout();

		return true;
	}
};

class $modify(test, ChallengesPage) {
	void claimItem(ChallengeNode* node, GJChallengeItem* item, cocos2d::CCPoint point) {
		ChallengesPage::claimItem(node, item, point);

		if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
			if (CCMenuItemSpriteExtra* button = static_cast<CrazyLayer*>(layer)->m_fields->m_questBtn) {
				auto piVar2 = static_cast<CrazyLayer*>(layer)->m_fields->m_questBtn->getChildByID("quest-notif"_spr);
				bool claimable = false;
				for (int i = 1; i <= 3; i++) {
					if (auto challenge = GameStatsManager::sharedState()->getChallenge(i)) {
						if (challenge->m_canClaim) {
							claimable = true;
							break;
						}
					}
				}
				piVar2->setVisible(claimable);
			} else {
				if (auto button = typeinfo_cast<RDButton*>(layer->getChildByIDRecursive("quests-button"))) {
					button->updateQuestsLabel();
				}
			}
		}
	}
};
