#include "RDDailyNode.hpp"

RDDailyNode* RDDailyNode::create(CCSize size, std::string id, float scale) {
    auto ret = new RDDailyNode();
    if (ret && ret->init(size, id, scale)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void RDDailyNode::updateTimeLabel(float time) {
    if (Variables::DailyLeft < 1) {
        m_timeLabel->setVisible(false);
        m_timeLeftLabel->setVisible(false);
        m_timerLoadingCircle->setVisible(true);
        this->unschedule(schedule_selector(RDDailyNode::updateTimeLabel));
    } else {
        m_timeLabel->setString(GameToolbox::getTimeString(Variables::DailyLeft, true).c_str());
        if (m_timeLabel->getPositionX() - m_safeButton->getPositionX() - m_safeButton->getScaledContentWidth()/2 - 5.f < m_timeLabel->getScaledContentWidth()) {
            m_timeLabel->setScale(m_timeLabel->getScale() * (m_timeLabel->getPositionX() - m_safeButton->getPositionX() - m_safeButton->getScaledContentWidth()/2 - 5.f) / m_timeLabel->getScaledContentWidth());
        }
        m_timeLabel->setVisible(true);
        m_timeLeftLabel->setVisible(true);
        m_timerLoadingCircle->setVisible(false);
    }
}

int RDDailyNode::getBonusDiamonds() {
    if (m_currentLevel->m_demon.value()) return 20;
    if (m_currentLevel->m_stars.value() == 1) return 0;
    return m_currentLevel->getAverageDifficulty();
}

CCScale9Sprite* RDDailyNode::createBG(CCSize size, float scale) {
    auto bg = CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(size * scale);
    bg->setPosition(size*scale/2);
    bg->setID("background");
    return bg;
}

bool RDDailyNode::init(CCSize size, std::string id, float scale) {
    if (!RDTimelyNode::init(size, id, scale)) return false;

    auto GLM = GameLevelManager::get();

    auto crownSprite = CCSprite::createWithSpriteFrameName("gj_dailyCrown_001.png");
    crownSprite->setScale(0.75f);
    crownSprite->setPosition({ size.width/2, size.height + 8.f });
    crownSprite->setID("crown-sprite");
    m_mainNode->addChild(crownSprite, 1);

    auto titleSprite = CCSprite::createWithSpriteFrameName("dailyLevelLabel_001.png");
    titleSprite->setScale(0.7f);
    titleSprite->setPosition({ size.width/2, size.height - 22.5f });
    titleSprite->setID("title-sprite");
    m_mainNode->addChild(titleSprite, 1);

    m_bonusMenu->setPositionX(m_bonusMenu->getPositionX() + 2.f);

    if (auto level = GLM->getSavedDailyLevel(GLM->m_activeDailyID)) {
        RDDailyNode::setupLevelMenu(level);
    } else {
        GLM->downloadLevel(-1, false);
        m_loadingCircle->setVisible(true);
        m_bonusMenu->setVisible(false);
    }

    auto time = Variables::DailyLeft;
    if (time < 0) time = 0;

    auto timeLabel = CCLabelBMFont::create(
        GameToolbox::getTimeString(time, true).c_str(),
        Mod::get()->getSettingValue<bool>("use-pusab") ? "bigFont.fnt" : "gjFont16.fnt"
    );
    timeLabel->setScale(0.55f);
    timeLabel->setAnchorPoint({ 1, 0.5f });
    timeLabel->setPosition({ m_innerBG->getPositionX() + m_innerBG->getScaledContentSize().width/2 , size.height/9 });
    timeLabel->setID("time-label");
    m_mainNode->addChild(timeLabel, 1);
    m_timeLabel = timeLabel;

    auto timeLeftLabel = CCLabelBMFont::create(
        "New Daily in:",
        "bigFont.fnt"
    );
    timeLeftLabel->setScale(0.3f);
    timeLeftLabel->setAnchorPoint({ 1, 0.5f });
    timeLeftLabel->setPosition({ timeLabel->getPositionX(), (m_safeButton->getPositionY() + m_innerBG->getPositionY() - m_innerBG->getScaledContentHeight()/2)/2});
    timeLeftLabel->setColor({ 200, 200, 200 });
    timeLeftLabel->setID("time-left-label");
    m_mainNode->addChild(timeLeftLabel, 1);
    m_timeLeftLabel = timeLeftLabel;

    auto timerLoadingCircle = LoadingCircle::create();
    timerLoadingCircle->setScale(0.45f);
    timerLoadingCircle->setContentSize({ 0 , 0 });
    timerLoadingCircle->setPositionX(m_timeLeftLabel->getPositionX() - m_timeLeftLabel->getScaledContentWidth()/2);
    timerLoadingCircle->setPositionY((m_timeLeftLabel->getPositionY() + m_timeLabel->getPositionY())/2);
    timerLoadingCircle->setVisible(false);
    timerLoadingCircle->m_sprite->setPosition({ 0 , 0 });
    timerLoadingCircle->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    timerLoadingCircle->setID("timer-loading-circle");
    m_mainNode->addChild(timerLoadingCircle, 2);
    m_timerLoadingCircle = timerLoadingCircle;

    if (time == 0) {
        timeLabel->setVisible(false);
        timeLeftLabel->setVisible(false);
        timerLoadingCircle->setVisible(true);
    }

    return true;
}

void RDDailyNode::onSkipLevel(CCObject* sender) {
    geode::createQuickPopup(
        "Skip level",
        "There is a <cy>new</c> daily level available.\nSkip the current level and load the next?",
        "Cancel", "Skip",
        [this](auto popup, bool btn2) {
            if (btn2) {
                m_loadingCircle->setVisible(true);
                m_menu->removeAllChildren();
                m_bonusMenu->setVisible(false);
                GameLevelManager::get()->downloadLevel(-1, false);
                Mod::get()->setSavedValue("claimed-daily", false);
            }
        }
    );
}

void RDDailyNode::onReload(CCObject* sender) {
    m_loadingCircle->setVisible(true);
    m_menu->removeAllChildren();

    if (Variables::DailyLeft == 0) GameLevelManager::get()->getGJDailyLevelState(GJTimedLevelType::Daily);
    GameLevelManager::get()->downloadLevel(-1, false);
}

void RDDailyNode::onTheSafe(CCObject* sender) {
    auto search = GJSearchObject::create(SearchType::DailySafe);
    auto scene = LevelBrowserLayer::scene(search);
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
}

void RDDailyNode::onClaimReward(CCObject* sender) {
    auto GLM = GameLevelManager::get();

    auto reward = GameStatsManager::sharedState()->completedDailyLevel(m_currentLevel);
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    Mod::get()->setSavedValue("claimed-daily", true);

    if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(sender)) {
            auto point = ccp(232.49, 200); // i dont remember anything i did so wtf is this???
            if (auto menu = layer->getChildByID("redash-menu"_spr)) {
                if (auto dailiesMenu = menu->getChildByID("dailies-menu"_spr)) {
                    point.x = dailiesMenu->getPositionX() - dailiesMenu->getScaledContentSize().width/2 + this->getPositionX()*dailiesMenu->getScaleX() + button->getPositionX() - button->getScaledContentSize().width*1.5f;
                    point.y = dailiesMenu->getPositionY() - dailiesMenu->getScaledContentSize().height/2 + this->getPositionY()*dailiesMenu->getScaleY() + button->getPositionY() - button->getScaledContentSize().height/2;
                }
            }

            auto rewardLayer = CurrencyRewardLayer::create(
                0, 0, 0, getBonusDiamonds(),
                CurrencySpriteType::Icon, 0, // demonKeys
                CurrencySpriteType::Icon, 0, // shards
                point,
                static_cast<CurrencyRewardType>(0), // dunno
                0, 1
            );
            layer->addChild(rewardLayer, 1000);

            m_viewButton->setScale(0.6f);
            m_viewButton->m_baseScale = 0.6f;
            m_viewButton->m_scaleMultiplier = 1.15f;
            m_viewButton->setPosition({ m_menu->getContentWidth() * 11/13, m_menu->getContentHeight() / 2 });
            button->removeFromParent();

            m_skipButton->setVisible(GLM->m_activeDailyID < GLM->m_dailyID);
        }
    }

    FMODAudioEngine::sharedEngine()->playEffect("gold02.ogg");
}

void RDDailyNode::setupLevelMenu(GJGameLevel* level) {
    RDTimelyNode::setupLevelMenu(level);

    auto GLM = GameLevelManager::get();

    auto skipButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
        this,
        menu_selector(RDDailyNode::onSkipLevel)
    );
    skipButton->setVisible(GLM->m_activeDailyID < GLM->m_dailyID);
    skipButton->setScale(0.5f);
    skipButton->setPositionX(m_innerBG->getPositionX() - m_innerBG->getScaledContentSize().width/2 + 5.f);
    skipButton->setPositionY(m_innerBG->getPositionY() + m_innerBG->getScaledContentSize().height/2);
    skipButton->m_baseScale = 0.5f;
    skipButton->m_scaleMultiplier = 1.2f;
    skipButton->setID("skip-button");
    m_menu->addChild(skipButton, 10);
    m_skipButton = skipButton;

    if (level->m_normalPercent.value() == 100
    && !GameStatsManager::sharedState()->hasCompletedDailyLevel(GLM->m_activeDailyID)) {
        auto claimButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png"),
            this,
            menu_selector(RDDailyNode::onClaimReward)
        );
        claimButton->setScale(0.6f);
        claimButton->m_baseScale = 0.6f;
        claimButton->m_scaleMultiplier = 1.15f;
        claimButton->setPosition(m_viewButton->getPosition() + ccp(10.f, 0));
        claimButton->setID("claim-button");
        m_menu->addChild(claimButton, 10);
        
        m_viewButton->setScale(0.2f);
        m_viewButton->m_baseScale = 0.2f;
        m_viewButton->setPosition(claimButton->getPosition() - ccp(claimButton->getScaledContentWidth()/2 + m_viewButton->getScaledContentWidth()/2 + 2.5f, 0));

        skipButton->setVisible(false);
    }
}

void RDDailyNode::setupBonusMenu(GJGameLevel* level) {
    m_bonusMenu->removeAllChildren();

    auto bonusSprite = CCSprite::createWithSpriteFrameName("GJ_bigDiamond_001.png");
    bonusSprite->setScale(0.4f);
    bonusSprite->setID("bonus-sprite");
    m_bonusMenu->addChild(bonusSprite);

    auto bonusLabel = CCLabelBMFont::create(
        fmt::format("x{} bonus", getBonusDiamonds()).c_str(),
        "bigFont.fnt"
    );
    bonusLabel->setScale(0.4f);
    bonusLabel->setID("bonus-label");
    m_bonusMenu->addChild(bonusLabel);

    m_bonusMenu->updateLayout();
    m_bonusMenu->setVisible(true);
}
