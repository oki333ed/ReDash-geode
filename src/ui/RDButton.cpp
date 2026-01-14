#include "RDButton.hpp"
#include "../Variables.hpp"

bool RDButton::init(CCObject* target, std::string title, std::vector<std::string> description, std::string sprite, float spriteScale, cocos2d::SEL_MenuHandler callback, std::string id) {
    auto spriteNode = CCNode::create();

	auto buttonSpr1 = CCScale9Sprite::create("longButtonHalf-1.png"_spr);
	buttonSpr1->setContentSize({150.f, 35.f});
	buttonSpr1->setPosition(ccp(75.f, 52.f));
	buttonSpr1->setID("bg-sprite-1");
	spriteNode->addChild(buttonSpr1);

	auto buttonSpr2 = CCScale9Sprite::create("longButtonHalf-2.png"_spr);
	buttonSpr2->setContentSize({150.f, 35.f});
	buttonSpr2->setPosition(ccp(75.f, 17.f));
	buttonSpr2->setID("bg-sprite-2");
	spriteNode->addChild(buttonSpr2);

	// auto buttonSpr = CCScale9Sprite::create("RD_longButton.png"_spr);
	// buttonSpr->setContentSize({150.f, 70.f});
	// buttonSpr->setPosition(ccp(75.f, 35.f));
	// spriteNode->addChild(buttonSpr, -1);

    auto icon = CCSprite::createWithSpriteFrameName(sprite.c_str());
    icon->setScale(spriteScale);
    icon->setPosition(ccp(148, 35) - ccp(icon->getScaledContentWidth()/2, 0));
	if (id == "saved-button") icon->setPositionX(icon->getPositionX() + 5.f);
	icon->setID("icon-sprite");
    spriteNode->addChild(icon);

	auto labelMenu = CCMenu::create();
	if (!Mod::get()->getSettingValue<bool>("ltr-texts")) {
		labelMenu->setAnchorPoint({ 1, 0.5f });
		labelMenu->setPosition( { icon->getPositionX() - icon->getScaledContentWidth()/2 - 2.5f, 35.f });
	} else {
		labelMenu->setAnchorPoint({ 0, 0.5f });
		labelMenu->setPosition( { 7.5f, 35.f });
	}
	labelMenu->setContentSize({ 80.f, 60.f });
	auto alignment = 0;
	if (!Mod::get()->getSettingValue<bool>("ltr-texts")) alignment = 2;
	labelMenu->setLayout(
		ColumnLayout::create()
			->setAxisReverse(true)
			->setCrossAxisAlignment(static_cast<AxisAlignment>(alignment))
			->setCrossAxisLineAlignment(static_cast<AxisAlignment>(alignment))
			->setAutoScale(false)
			->setGap(3.f)
	);
	labelMenu->setID("label-menu");
	spriteNode->addChild(labelMenu);
	m_labelMenu = labelMenu;

	auto titleSprite = CCSprite::create(title.c_str());
	titleSprite->setScale(0.8f);
	if (titleSprite->getScaledContentWidth() > 80.f) titleSprite->setScale(0.8f * (80.f / titleSprite->getScaledContentWidth()));
	titleSprite->setID("title-sprite");
	labelMenu->addChild(titleSprite);
	m_titleSprite = titleSprite;

	if (!Mod::get()->getSettingValue<bool>("hide-bottom-buttons-texts")) {
		int i = 0;
		for (auto desc : description) {
			auto descLabel = CCLabelBMFont::create(desc.c_str(), "NewPusab.fnt"_spr, -1, CCTextAlignment::kCCTextAlignmentRight);
			descLabel->setScale(0.4f);
			auto color = Mod::get()->getSettingValue<ccColor4B>("desc-label-color");
			descLabel->setColor({ color.r, color.g, color.b });
			descLabel->setOpacity(color.a);
			descLabel->setID(fmt::format("desc-label-{}", ++i));
			labelMenu->addChild(descLabel, i);
		}
	}
	labelMenu->updateLayout();
	for (int i = 1; i < labelMenu->getChildrenCount(); i++) {
		auto label = labelMenu->getChildByID(fmt::format("desc-label-{}", i));
		label->setPositionY(label->getPositionY() + label->getScaledContentHeight()/2);
	}

	auto loadingCircle = LoadingCircle::create();
    loadingCircle->setScale(0.65f);
    loadingCircle->setContentSize({ 0 , 0 });
	if (Mod::get()->getSettingValue<bool>("ltr-texts")) {
    	loadingCircle->setPosition(labelMenu->getPosition() + ccp(labelMenu->getContentWidth()/2, 0));
	} else {
    	loadingCircle->setPosition(labelMenu->getPosition() - ccp(labelMenu->getContentWidth()/2, 0));
	}
    loadingCircle->setVisible(false);
    loadingCircle->m_sprite->setPosition({ 0 , 0 });
    loadingCircle->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    loadingCircle->setID("leaderboards-loading-circle");
    spriteNode->addChild(loadingCircle, 2);
    m_loadingCircle = loadingCircle;

	if (!Mod::get()->getSettingValue<bool>("hide-bottom-buttons-texts")) {
		if (id == "leaderboards-button") {
			if (Variables::GlobalRank == 0) {
				loadingCircle->setVisible(true);
				labelMenu->setVisible(false);
			} else if (Variables::GlobalRank == -1) {
				static_cast<CCLabelBMFont*>(labelMenu->getChildByID("desc-label-2"))->setString("None");
				labelMenu->updateLayout();
			}
		}
	}

    if (!CCMenuItemSpriteExtra::init(spriteNode, nullptr, target, callback)) return false;

	this->setContentSize({150.f, 70.f});
	this->m_scaleMultiplier = 1.1f;
	this->setID(id);

	return true;
}

void RDButton::getLeaderboardRankFailed() {
	m_loadingCircle->setVisible(false);
	for (int i = 1; i < m_labelMenu->getChildrenCount(); i++) {
		m_labelMenu->getChildByID(fmt::format("desc-label-{}", i))->removeFromParent();
	}
	m_labelMenu->setVisible(true);
	m_labelMenu->updateLayout();
}

void RDButton::updateLeaderboardLabel() {
	m_loadingCircle->setVisible(false);
	m_labelMenu->setVisible(true);
	static_cast<CCLabelBMFont*>(m_labelMenu->getChildByID("desc-label-2"))->setString(fmt::format("#{}", Variables::GlobalRank).c_str());
	m_labelMenu->updateLayout();
}

void RDButton::rotateIcon(float rotation) {
	if (this->getID() != "search-button") {
		if (auto icon = this->getChildByIDRecursive("icon-sprite")) {
			icon->setRotation(rotation);
		}
	}
}

void RDButton::updateQuestsLabel() {
	m_completedQuests--;
	if (auto descLabel1 = m_labelMenu->getChildByID("desc-label")) {
		static_cast<CCLabelBMFont*>(descLabel1)->setString(fmt::format("{}/3 Quest(s)", m_completedQuests).c_str());
	}
	m_labelMenu->updateLayout();
}

RDButton* RDButton::create(CCObject* target, std::string title, std::vector<std::string> description, std::string sprite, float spriteScale, cocos2d::SEL_MenuHandler callback, std::string id) {
    auto ret = new RDButton();
    if (ret && ret->init(target, title, description, sprite, spriteScale, callback, id)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
