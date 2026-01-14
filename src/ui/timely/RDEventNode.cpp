#include "RDEventNode.hpp"

RDEventNode* RDEventNode::create(CCSize size, std::string id, float scale) {
    auto ret = new RDEventNode();
    if (ret && ret->init(size, id, scale)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CCScale9Sprite* RDEventNode::createBG(CCSize size, float scale) {
    auto bg = CCScale9Sprite::create("GJ_square05.png");
    bg->setContentSize(size * scale);
    bg->setPosition(size*scale/2);
    bg->setColor({ 200, 50, 255 });
    bg->setID("background");
    return bg;
}

bool RDEventNode::init(CCSize size, std::string id, float scale) {
    if (!RDTimelyNode::init(size, id, scale)) return false;

    auto GLM = GameLevelManager::get();

    auto crownSprite = CCSprite::createWithSpriteFrameName("RD_eventCrown_001.png"_spr);
    crownSprite->setScale(0.75f);
    crownSprite->setPosition({ size.width/2, size.height + 8.f });
    crownSprite->setID("crown-sprite");
    m_mainNode->addChild(crownSprite, 1);

    auto titleSprite = CCSprite::createWithSpriteFrameName("eventLevelLabel_001.png");
    titleSprite->setScale(0.7f);
    titleSprite->setPosition({ size.width/2, size.height - 22.5f });
    titleSprite->setID("title-sprite");
    m_mainNode->addChild(titleSprite, 1);

    m_bonusMenu->setPositionX(m_bonusMenu->getPositionX() + 5.f);

    if (auto level = GLM->getSavedDailyLevel(GLM->m_activeEventID)) {
        RDEventNode::setupLevelMenu(level);
    } else {
        GLM->downloadLevel(-3, false);
        m_loadingCircle->setVisible(true);
        m_bonusMenu->setVisible(false);
    }

    auto timeLabel = CCLabelBMFont::create(
        "who knows...",
        Mod::get()->getSettingValue<bool>("use-pusab") ? "bigFont.fnt" : "gjFont16.fnt"
    );
    timeLabel->setScale(0.35f);
    timeLabel->setAnchorPoint({ 1, 0.5f });
    timeLabel->setPosition({ m_innerBG->getPositionX() + m_innerBG->getScaledContentSize().width/2 , size.height/9 });
    timeLabel->setColor({ 255, 0, 0 });
    timeLabel->setID("time-label");
    m_mainNode->addChild(timeLabel, 1);
    m_timeLabel = timeLabel;

    auto timeLeftLabel = CCLabelBMFont::create(
        "New Event in:",
        "bigFont.fnt"
    );
    timeLeftLabel->setScale(0.3f);
    timeLeftLabel->setAnchorPoint({ 1, 0.5f });
    timeLeftLabel->setPosition({ timeLabel->getPositionX(), (m_safeButton->getPositionY() + m_innerBG->getPositionY() - m_innerBG->getScaledContentHeight()/2)/2});
    timeLeftLabel->setColor({ 200, 200, 200 });
    timeLeftLabel->setID("time-left-label");
    m_mainNode->addChild(timeLeftLabel, 1);
    m_timeLeftLabel = timeLeftLabel;

    return true;
}

void RDEventNode::onSkipLevel(CCObject* sender) {
    geode::createQuickPopup(
        "Skip level",
        "There is a <cy>new</c> event level available.\nSkip the current level and load the next?",
        "Cancel", "Skip",
        [this](auto popup, bool btn2) {
            if (btn2) {
                m_loadingCircle->setVisible(true);
                m_menu->removeAllChildren();
                m_bonusMenu->setVisible(false);
                GameLevelManager::get()->downloadLevel(-3, false);
                Mod::get()->setSavedValue("claimed-event", false);
            }
        }
    );
}

void RDEventNode::onReload(CCObject* sender) {
    m_loadingCircle->setVisible(true);
    m_menu->removeAllChildren();

    if (Variables::EventLeft == 0) GameLevelManager::get()->getGJDailyLevelState(GJTimedLevelType::Event);
    GameLevelManager::get()->downloadLevel(-3, false);
}

void RDEventNode::onTheSafe(CCObject* sender) {
    auto search = GJSearchObject::create(SearchType::EventSafe);
    auto scene = LevelBrowserLayer::scene(search);
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
}

void RDEventNode::onClaimReward(CCObject* sender) {
    auto GLM = GameLevelManager::get();

    auto reward = GameStatsManager::sharedState()->completedDailyLevel(m_currentLevel);
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    Mod::get()->setSavedValue("claimed-event", true);

    if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(sender)) {
            auto rewardLayer = RewardUnlockLayer::create(static_cast<int>(reward->m_rewardType), nullptr);
            layer->addChild(rewardLayer, 1000);
            rewardLayer->showCollectReward(reward);

            m_viewButton->setScale(0.6f);
            m_viewButton->m_baseScale = 0.6f;
            m_viewButton->m_scaleMultiplier = 1.15f;
            m_viewButton->setPosition({ m_menu->getContentWidth() * 11/13, m_menu->getContentHeight() / 2 });
            button->removeFromParent();

            m_skipButton->setVisible(GLM->m_activeEventID < GLM->m_eventID);
        }
    }

    FMODAudioEngine::sharedEngine()->playEffect("gold02.ogg");
}

void RDEventNode::setupLevelMenu(GJGameLevel* level) {
    RDTimelyNode::setupLevelMenu(level);

    auto GLM = GameLevelManager::get();

    auto skipButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
        this,
        menu_selector(RDEventNode::onSkipLevel)
    );
    skipButton->setVisible(GLM->m_activeEventID < GLM->m_eventID);
    skipButton->setScale(0.5f);
    skipButton->setPositionX(m_innerBG->getPositionX() - m_innerBG->getScaledContentSize().width/2 + 5.f);
    skipButton->setPositionY(m_innerBG->getPositionY() + m_innerBG->getScaledContentSize().height/2);
    skipButton->m_baseScale = 0.5f;
    skipButton->m_scaleMultiplier = 1.2f;
    skipButton->setID("skip-button");
    m_menu->addChild(skipButton, 10);
    m_skipButton = skipButton;

    if (level->m_normalPercent.value() == 100
    && !GameStatsManager::sharedState()->hasCompletedDailyLevel(GLM->m_activeEventID)) {
        auto claimButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png"),
            this,
            menu_selector(RDEventNode::onClaimReward)
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

void RDEventNode::setupBonusMenu(GJGameLevel* level) {
    m_bonusMenu->removeAllChildren();

    auto bonusSprite = CCSprite::createWithSpriteFrameName("chest_03_02_001.png");
    bonusSprite->setScale(0.16f);
    bonusSprite->setID("bonus-sprite");
    m_bonusMenu->addChild(bonusSprite);

    auto bonusLabel = CCLabelBMFont::create(
        " / Bonus",
        "bigFont.fnt"
    );
    bonusLabel->setScale(0.4f);
    bonusLabel->setID("bonus-label");
    m_bonusMenu->addChild(bonusLabel);

    m_bonusMenu->updateLayout();
    m_bonusMenu->setVisible(true);
}
