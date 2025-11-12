#include "RDWeeklyNode.hpp"

RDWeeklyNode* RDWeeklyNode::create(CCSize size, std::string id, float scale) {
    auto ret = new RDWeeklyNode();
    if (ret && ret->init(size, id, scale)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void RDWeeklyNode::updateTimeLabel(float time) {
    if (Variables::WeeklyLeft < 1) {
        m_timeLabel->setVisible(false);
        m_timeLeftLabel->setVisible(false);
        m_timerLoadingCircle->setVisible(true);
        this->unschedule(schedule_selector(RDWeeklyNode::updateTimeLabel));
    } else {
        m_timeLabel->setString(GameToolbox::getTimeString(Variables::WeeklyLeft, true).c_str());
        if (m_timeLabel->getPositionX() - m_safeButton->getPositionX() - m_safeButton->getScaledContentWidth()/2 - 5.f < m_timeLabel->getScaledContentWidth()) {
            m_timeLabel->setScale(m_timeLabel->getScale() * (m_timeLabel->getPositionX() - m_safeButton->getPositionX() - m_safeButton->getScaledContentWidth()/2 - 5.f) / m_timeLabel->getScaledContentWidth());
        }
        m_timeLabel->setVisible(true);
        m_timeLeftLabel->setVisible(true);
        m_timerLoadingCircle->setVisible(false);
    }
}

CCScale9Sprite* RDWeeklyNode::createBG(CCSize size, float scale) {
    auto bg = CCScale9Sprite::create("GJ_square05.png");
    bg->setContentSize(size * scale);
    bg->setPosition(size*scale/2);
    bg->setID("background");
    return bg;
}

bool RDWeeklyNode::init(CCSize size, std::string id, float scale) {
    if (!RDTimelyNode::init(size, id, scale)) return false;

    auto GLM = GameLevelManager::get();

    auto crownSprite = CCSprite::createWithSpriteFrameName("gj_weeklyCrown_001.png");
    crownSprite->setScale(0.75f);
    crownSprite->setPosition({ size.width/2, size.height + 8.f });
    crownSprite->setID("crown-sprite");
    m_mainNode->addChild(crownSprite, 1);

    auto titleSprite = CCSprite::createWithSpriteFrameName("weeklyLevelLabel_001.png");
    titleSprite->setScale(0.7f);
    titleSprite->setPosition({ size.width/2, size.height - 22.5f });
    titleSprite->setID("title-sprite");
    m_mainNode->addChild(titleSprite, 1);

    m_bonusMenu->setPositionX(m_bonusMenu->getPositionX() + 5.f);

    if (auto level = GLM->getSavedDailyLevel(GLM->m_activeWeeklyID)) {
        RDWeeklyNode::setupLevelMenu(level);
    } else {
        GLM->downloadLevel(-2, false);
        m_loadingCircle->setVisible(true);
        m_bonusMenu->setVisible(false);
    }

    auto time = Variables::WeeklyLeft;
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
        "New Weekly in:",
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

void RDWeeklyNode::onSkipLevel(CCObject* sender) {
    geode::createQuickPopup(
        "Skip level",
        "There is a <cy>new</c> weekly level available.\nSkip the current level and load the next?",
        "Cancel", "Skip",
        [this](auto popup, bool btn2) {
            if (btn2) {
                m_loadingCircle->setVisible(true);
                m_menu->removeAllChildren();
                m_bonusMenu->setVisible(false);
                GameLevelManager::get()->downloadLevel(-2, false);
                Mod::get()->setSavedValue("claimed-weekly", false);
            }
        }
    );
}

void RDWeeklyNode::onReload(CCObject* sender) {
    m_loadingCircle->setVisible(true);
    m_menu->removeAllChildren();

    if (Variables::WeeklyLeft == 0) GameLevelManager::get()->getGJDailyLevelState(GJTimedLevelType::Weekly);
    GameLevelManager::get()->downloadLevel(-2, false);
}

void RDWeeklyNode::onTheSafe(CCObject* sender) {
    auto search = GJSearchObject::create(SearchType::WeeklySafe);
    auto scene = LevelBrowserLayer::scene(search);
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
}

void RDWeeklyNode::onClaimReward(CCObject* sender) {
    auto GLM = GameLevelManager::get();

    auto reward = GameStatsManager::sharedState()->completedDailyLevel(m_currentLevel);
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    Mod::get()->setSavedValue("claimed-weekly", true);

    if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {
        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(sender)) {
            /* in case RewardUnlockLayer is fucked again */

            // auto point = ccp(232.49, 200); // i dont remember anything i did so wtf is this???
            // if (auto menu = layer->getChildByID("redash-menu"_spr)) {
            //     if (auto dailiesMenu = menu->getChildByID("dailies-menu"_spr)) {
            //         point.x = dailiesMenu->getPositionX() - dailiesMenu->getScaledContentSize().width/2 + this->getPositionX()*dailiesMenu->getScaleX() + button->getPositionX() - button->getScaledContentSize().width*1.5f;
            //         point.y = dailiesMenu->getPositionY() - dailiesMenu->getScaledContentSize().height/2 + this->getPositionY()*dailiesMenu->getScaleY() + button->getPositionY() - button->getScaledContentSize().height/2;
            //     }
            // }

            // int diamonds = 0;
            // int orbs = 0;
            // int keySprite = 0;
            // int key = 0;
            // int shardsType = 0;
            // int shards = 0;
            // for (auto t : CCArrayExt<GJRewardObject*>(reward->m_rewardObjects)) {
            //     switch (static_cast<int>(t->m_specialRewardItem)) {
            //         case 7:
            //             orbs = t->m_total;
            //             break;
            //         case 8:
            //             diamonds = t->m_total;
            //             break;
            //         case 6:
            //             keySprite = 9;
            //             key = t->m_total;
            //         // shards
            //         case 1:
            //             shardsType = 4;
            //             shards = t->m_total;
            //             break;
            //         case 2:
            //             shardsType = 5;
            //             shards = t->m_total;
            //             break;
            //         case 3:
            //             shardsType = 6;
            //             shards = t->m_total;
            //             break;
            //         case 4:
            //             shardsType = 7;
            //             shards = t->m_total;
            //             break;
            //         case 5:
            //             shardsType = 8;
            //             shards = t->m_total;
            //             break;
            //         case 10:
            //             shardsType = 10;
            //             shards = t->m_total;
            //             break;
            //         case 11:
            //             shardsType = 11;
            //             shards = t->m_total;
            //             break;
            //         case 12:
            //             shardsType = 12;
            //             shards = t->m_total;
            //             break;
            //         case 13:
            //             shardsType = 13;
            //             shards = t->m_total;
            //             break;
            //         case 14:
            //             shardsType = 14;
            //             shards = t->m_total;
            //             break;
            //     }
            // }

            // auto rewardLayer = CurrencyRewardLayer::create(orbs, 0, 0, diamonds, static_cast<CurrencySpriteType>(keySprite), key, static_cast<CurrencySpriteType>(shardsType), shards, point, static_cast<CurrencyRewardType>(1), 0, 1);
            // layer->addChild(rewardLayer, 1000);

            auto rewardLayer = RewardUnlockLayer::create(static_cast<int>(reward->m_rewardType), nullptr);
            layer->addChild(rewardLayer, 1000);
            rewardLayer->showCollectReward(reward);

            m_viewButton->setScale(0.6f);
            m_viewButton->m_baseScale = 0.6f;
            m_viewButton->m_scaleMultiplier = 1.15f;
            m_viewButton->setPosition({ m_menu->getContentWidth() * 11/13, m_menu->getContentHeight() / 2 });
            button->removeFromParent();

            m_skipButton->setVisible(GLM->m_activeWeeklyID < GLM->m_weeklyID);
        }
    }

    FMODAudioEngine::sharedEngine()->playEffect("gold02.ogg");
}

void RDWeeklyNode::setupLevelMenu(GJGameLevel* level) {
    RDTimelyNode::setupLevelMenu(level);

    auto GLM = GameLevelManager::get();

    auto skipButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
        this,
        menu_selector(RDWeeklyNode::onSkipLevel)
    );
    skipButton->setVisible(GLM->m_activeWeeklyID < GLM->m_weeklyID);
    skipButton->setScale(0.5f);
    skipButton->setPositionX(m_innerBG->getPositionX() - m_innerBG->getScaledContentSize().width/2 + 5.f);
    skipButton->setPositionY(m_innerBG->getPositionY() + m_innerBG->getScaledContentSize().height/2);
    skipButton->m_baseScale = 0.5f;
    skipButton->m_scaleMultiplier = 1.2f;
    skipButton->setID("skip-button");
    m_menu->addChild(skipButton, 10);
    m_skipButton = skipButton;

    if (level->m_normalPercent.value() == 100
    && !GameStatsManager::sharedState()->hasCompletedDailyLevel(GLM->m_activeWeeklyID)) {
        auto claimButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png"),
            this,
            menu_selector(RDWeeklyNode::onClaimReward)
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

void RDWeeklyNode::setupBonusMenu(GJGameLevel* level) {
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
