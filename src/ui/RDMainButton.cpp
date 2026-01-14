#include "RDMainButton.hpp"

void RDMainButton::onPlay(CCObject* sender) {
    int page = Mod::get()->getSavedValue<int64_t>("last-main-level", 1);
    if (page >= 5000 && page < 6000) page = 23;
    if (!Mod::get()->getSettingValue<bool>("goto-last-played-main")) page = 1;
    auto sc = LevelSelectLayer::scene(page-1);
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, sc));
}

bool RDMainButton::init(CCPoint position, CCSize size, std::string id, float scale) {
    auto GSM = GameStatsManager::sharedState();
    auto GLM = GameLevelManager::get();
    auto spriteNode = CCNode::create();

    if (!CCMenuItemSpriteExtra::init(spriteNode, nullptr, this, menu_selector(RDMainButton::onPlay))) return false;

    auto background = CCScale9Sprite::create("GJ_square02.png");
    background->setContentSize(size * scale);
    background->setPosition({ size.width/2*scale, size.height/2*scale });
    background->setID("background");
    this->addChild(background, -1);

    auto crownSprite = CCSprite::createWithSpriteFrameName("RD_mainLevel.png"_spr);
    crownSprite->setScale(0.3f);
    crownSprite->setPosition({ size.width/2, size.height + 3.f });
    crownSprite->setID("crown-sprite");
    spriteNode->addChild(crownSprite, 1);

    auto titleSprite = CCSprite::createWithSpriteFrameName("RD_mainLevelsLabel.png"_spr);
    titleSprite->setScale(0.58f);
    titleSprite->setPosition({ size.width/2, size.height - 22.5f });
    titleSprite->setID("title-sprite");
    spriteNode->addChild(titleSprite, 1);


    auto innerBG = CCScale9Sprite::create("square02b_001.png");
    innerBG->setScale(0.5f);
    innerBG->setContentSize({ size.width - 15.f, size.height / 2.32f });
    innerBG->setContentSize(innerBG->getContentSize() * 2);
    innerBG->setPosition({ size.width/2, size.height/2 });
    innerBG->setColor({ 0, 0, 0 });
    innerBG->setOpacity(125);
    innerBG->setID("inner-background");
    spriteNode->addChild(innerBG, 1);

    auto menu = CCMenu::create();
    menu->setPosition({ 0.f, 0.f });
    menu->setContentSize(size);
    menu->setID("level-menu");
    spriteNode->addChild(menu, 2);

    auto level = GLM->getMainLevel(Mod::get()->getSavedValue<int64_t>("last-main-level", 1), true);

    auto innerMenu = CCMenu::create();
    innerMenu->setPosition({ size.width/2, size.height/2 });
    innerMenu->setContentWidth(size.width*0.8f);
    innerMenu->setContentHeight(size.height);
    innerMenu->setLayout(
        RowLayout::create()
            ->setGap(5.f)
            ->setAutoScale(false)
    );
    innerMenu->setID("level-name-menu");
    menu->addChild(innerMenu, 1);

    auto diffSprite = CCSprite::createWithSpriteFrameName(fmt::format("diffIcon_0{}_btn_001.png", static_cast<int>(level->m_difficulty)).c_str());
    diffSprite->setScale(0.8f);
    diffSprite->setID("difficulty-sprite");
    // diffSprite->setPosition({ size.width/6.25f, size.height/2.f });
    innerMenu->addChild(diffSprite);

    auto menuSize = innerBG->getScaledContentSize();
    auto baseY = innerBG->getPositionY() - menuSize.height/2;
    auto maxX = innerBG->getPositionX() + menuSize.width/2 - diffSprite->getPositionX() - diffSprite->getScaledContentWidth()/2 - 5.f;

    auto nameLabel = CCLabelBMFont::create(level->m_levelName.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.7f);
    nameLabel->setID("level-name-label");
    // if (nameLabel->getScaledContentWidth() > maxX) {
    //     nameLabel->setScale(maxX / nameLabel->getContentWidth());
    // }

    // nameLabel->setAnchorPoint({ 0, 0.5f });
    // nameLabel->setPositionX(diffSprite->getPositionX() + diffSprite->getScaledContentWidth()/2 + 5.f);
    innerMenu->addChild(nameLabel);

    innerMenu->updateLayout();
    if (diffSprite->getScaledContentWidth() + nameLabel->getScaledContentWidth() + 5.f > size.width*0.8f) {
        innerMenu->setScale(size.width*0.8f / (diffSprite->getScaledContentWidth() + nameLabel->getScaledContentWidth() + 5.f));
        innerMenu->setContentSize(innerMenu->getContentSize() / innerMenu->getScale());
        innerMenu->updateLayout();
    }

    nameLabel->setPositionY(nameLabel->getPositionY() + 2.f);

    auto starSprite = CCSprite::createWithSpriteFrameName("star_small01_001.png");
    starSprite->setScale(0.8f);
    starSprite->setPositionX(innerBG->getPositionX() + menuSize.width/2 - 7.f);
    starSprite->setPositionY(innerBG->getPositionY() + menuSize.height/2 - 7.f);
    starSprite->setID("star-sprite");
    menu->addChild(starSprite, 2);

    auto starsLabel = CCLabelBMFont::create(std::to_string(level->m_stars).c_str(), "bigFont.fnt");
    starsLabel->setScale(0.32f);
    starsLabel->setPositionX(starSprite->getPositionX() - starSprite->getScaledContentWidth()/2 - starsLabel->getScaledContentWidth()/2);
    starsLabel->setPositionY(starSprite->getPositionY());
    starsLabel->setColor({ 249, 255, 50});
    starsLabel->setID("stars-label");
    menu->addChild(starsLabel, 2);

    auto coin3Sprite = CCSprite::createWithSpriteFrameName(GSM->hasSecretCoin(level->getCoinKey(3)) ? "GJ_coinsIcon_001.png" : "GJ_coinsIcon_gray_001.png");
    coin3Sprite->setScale(0.5f);
    coin3Sprite->setAnchorPoint({ 1, 0 });
    coin3Sprite->setPositionX(innerBG->getPositionX() + menuSize.width/2 - 3.f);
    coin3Sprite->setPositionY(innerBG->getPositionY() - menuSize.height/2 + 3.f);
    coin3Sprite->setID("coin-3-sprite");
    menu->addChild(coin3Sprite, 3);
    
    auto coin2Sprite = CCSprite::createWithSpriteFrameName(GSM->hasSecretCoin(level->getCoinKey(2)) ? "GJ_coinsIcon_001.png" : "GJ_coinsIcon_gray_001.png");
    coin2Sprite->setScale(0.5f);
    coin2Sprite->setAnchorPoint({ 1, 0 });
    coin2Sprite->setPositionX(coin3Sprite->getPositionX() - coin3Sprite->getScaledContentWidth() - 1.f);
    coin2Sprite->setPositionY(coin3Sprite->getPositionY());
    coin2Sprite->setID("coin-2-sprite");
    menu->addChild(coin2Sprite, 3);
    
    auto coin1Sprite = CCSprite::createWithSpriteFrameName(GSM->hasSecretCoin(level->getCoinKey(1)) ? "GJ_coinsIcon_001.png" : "GJ_coinsIcon_gray_001.png");
    coin1Sprite->setScale(0.5f);
    coin1Sprite->setAnchorPoint({ 1, 0 });
    coin1Sprite->setPositionX(coin2Sprite->getPositionX() - coin2Sprite->getScaledContentWidth() - 1.f);
    coin1Sprite->setPositionY(coin2Sprite->getPositionY());
    coin1Sprite->setID("coin-1-sprite");
    menu->addChild(coin1Sprite, 3);
    
    auto baseCurrency = (level->m_stars.value() + 1) * 25;
    if (level->m_difficulty == GJDifficulty::Demon) baseCurrency = 500;
    auto orbsLabel = CCLabelBMFont::create(fmt::format("{}/{}", GSM->getAwardedCurrencyForLevel(level), baseCurrency).c_str(), "bigFont.fnt");
    if (GSM->getAwardedCurrencyForLevel(level) == baseCurrency) orbsLabel->setColor({ 49, 241, 255 });
    orbsLabel->setScale(0.25f);
    orbsLabel->setAnchorPoint({ 0, 0.5f });
    orbsLabel->setPositionX(innerBG->getPositionX() - menuSize.width/2 + 3.f);
    orbsLabel->setPositionY(baseY + orbsLabel->getScaledContentHeight()/2 + 3.f);
    orbsLabel->setID("orbs-label");
    menu->addChild(orbsLabel, 2);

    auto orbSprite = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
    orbSprite->setScale(0.5f);
    orbSprite->setAnchorPoint({ 0, 0.5f });
    orbSprite->setPositionX(orbsLabel->getPositionX() + orbsLabel->getScaledContentWidth());
    orbSprite->setPositionY(orbsLabel->getPositionY());
    orbSprite->setID("orb-sprite");
    menu->addChild(orbSprite, 2);

    auto percentLabel = CCLabelBMFont::create(fmt::format("{}%", level->m_normalPercent.value()).c_str(), "bigFont.fnt");
    percentLabel->setScale(0.25f);
    percentLabel->setAnchorPoint({ 0, 0.5f });
    percentLabel->setPositionX(size.width/2);
    percentLabel->setPositionY(baseY);
    percentLabel->setPositionX(innerBG->getPositionX() - menuSize.width/2 + 3.f);
    percentLabel->setPositionY(innerBG->getPositionY() + menuSize.height/2 - percentLabel->getScaledContentHeight()/2 - 2.f);
    percentLabel->setID("percent-label");
    menu->addChild(percentLabel, 2);

    auto checkSprite = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
    checkSprite->setScale(0.35f);
    checkSprite->setAnchorPoint({ 0, 0.5f });
    checkSprite->setPositionX(percentLabel->getPositionX() + percentLabel->getScaledContentWidth()  );
    checkSprite->setPositionY(percentLabel->getPositionY());
    checkSprite->setVisible(false);
    checkSprite->setID("check-sprite");
    menu->addChild(checkSprite, 2);

    if (level->m_normalPercent.value() == 100) {
        percentLabel->setColor({ 104, 255, 49 });
        checkSprite->setVisible(true);
    }

    if (Mod::get()->getSettingValue<bool>("hide-main-levels-stats")) {
        coin1Sprite->setVisible(false);
        coin2Sprite->setVisible(false);
        coin3Sprite->setVisible(false);
        orbsLabel->setVisible(false);
        orbSprite->setVisible(false);
        percentLabel->setVisible(false);
        checkSprite->setVisible(false);
    }

    auto lastPlayedLabel = CCLabelBMFont::create("Last Played:", "goldFont.fnt");
    lastPlayedLabel->setScale(0.4f);
    lastPlayedLabel->setPosition({ size.width/2, innerBG->getPositionY() + innerBG->getScaledContentHeight()/2 + 2.f });
    lastPlayedLabel->setID("last-played-label");
    spriteNode->addChild(lastPlayedLabel, 3);

    std::vector<int> mainLevels = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 5001, 5002, 5003, 5004 };
    int mainLevelsCompleted = 0;
    for (auto level : mainLevels) {
        if (GSM->hasCompletedMainLevel(level)) mainLevelsCompleted++;
    }
    
    auto completedLabel = CCLabelBMFont::create("Completed Levels:", "goldFont.fnt");
    completedLabel->setScale(0.5f);
    completedLabel->setPosition({ size.width/2, size.height/4 });
    completedLabel->setID("completed-levels-label");
    spriteNode->addChild(completedLabel, 1);

    auto progressBG = CCSprite::create("RD_progressBar.png"_spr);
    progressBG->setScale(1.2f);
    progressBG->setPosition({ size.width/2, size.height/8.f });
    progressBG->setColor({ 0, 0, 0 });
    progressBG->setOpacity(125);
    progressBG->setID("progress-bar-background");
    spriteNode->addChild(progressBG, 2);

    auto progressBar = CCSprite::create("RD_progressBar.png"_spr);
    progressBar->setScaleX(0.965f);
    progressBar->setScaleY(0.8f);
    progressBar->setPosition({ progressBG->getContentWidth()/2, progressBG->getContentHeight()/2 });
    progressBar->setID("progress-bar");

    auto clippingNode = CCClippingNode::create();
    auto stencil = CCScale9Sprite::create("square02_001.png");
	stencil->setAnchorPoint({ 0, 0.5f });
	stencil->setContentWidth(progressBar->getContentWidth());
    stencil->setContentHeight(100);
    stencil->setScaleX(0);
    clippingNode->setStencil(stencil);
    clippingNode->setAnchorPoint({ 0, 0 });
	clippingNode->setPosition({ 0, 0 });
	clippingNode->setContentSize(progressBG->getContentSize());
    clippingNode->setID("progress-bar-clipping-node");
	clippingNode->addChild(progressBar);
    progressBG->addChild(clippingNode);
    stencil->runAction(CCEaseSineInOut::create(CCScaleTo::create(1.f, mainLevelsCompleted / (float)mainLevels.size(), 1)));

    auto progressLabel = CCLabelBMFont::create(fmt::format("{}/{}", mainLevelsCompleted, mainLevels.size()).c_str(), "bigFont.fnt");
    progressLabel->setScale(0.5f);
    progressLabel->setPosition({ size.width/2, size.height/8 + 1});
    progressLabel->setID("progress-label");
    spriteNode->addChild(progressLabel, 3);

    this->setContentSize(size * scale);
    this->setPosition(position + size*scale/2);
    spriteNode->setScale(scale);
	this->m_scaleMultiplier = 1.1f;
    this->setID(id);

    return true;
}

RDMainButton* RDMainButton::create(CCPoint position, CCSize size, std::string id, float scale) {
    auto ret = new RDMainButton();
    if (ret && ret->init(position, size, id, scale)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
