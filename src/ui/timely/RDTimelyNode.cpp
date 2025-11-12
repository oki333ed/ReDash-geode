#include "RDTimelyNode.hpp"

bool RDTimelyNode::init(CCSize size, std::string id, float scale) {
    if (!CCNode::init()) return false;

    auto GLM = GameLevelManager::get();

    auto background = createBG(size, scale);
    this->addChild(background, -1);

    auto node = CCNode::create();
    node->setContentSize(size);
    node->setScale(scale);
    node->setID("main-node");
    this->addChild(node, 0);
    m_mainNode = node;

    // std::vector<const char*> crownVctr = {"gj_dailyCrown_001.png", "gj_weeklyCrown_001.png", "RD_eventCrown_001.png"_spr};
    // auto crownSprite = CCSprite::createWithSpriteFrameName(crownVctr[levelType]);
    // crownSprite->setScale(0.75f);
    // crownSprite->setPosition({ size.width/2, size.height + 8.f });
    // crownSprite->setID("crown-sprite");
    // node->addChild(crownSprite, 1);

    // std::vector<const char*> titleArr = {"dailyLevelLabel_001.png", "weeklyLevelLabel_001.png", "eventLevelLabel_001.png"};
    // auto titleSprite = CCSprite::createWithSpriteFrameName(titleArr[levelType]);
    // titleSprite->setScale(0.7f);
    // titleSprite->setPosition({ size.width/2, size.height - 22.5f });
    // titleSprite->setID("title-sprite");
    // node->addChild(titleSprite, 1);

    auto innerBG = CCScale9Sprite::create("square02b_001.png");
    innerBG->setScale(0.5f);
    innerBG->setContentSize({ size.width*2 - 30.f, size.height / 1.16f });
    innerBG->setPosition({ size.width/2, size.height/2 });
    innerBG->setColor({ 0, 0, 0 });
    innerBG->setOpacity(50);
    innerBG->setID("inner-background");
    node->addChild(innerBG, 1);
    m_innerBG = innerBG;

    auto menu = CCMenu::create();
    menu->setPosition({ 0.f, 0.f });
    menu->setContentSize(size);
    menu->setID("level-menu");
    node->addChild(menu, 2);
    m_menu = menu;

    auto loadingCircle = LoadingCircle::create();
    loadingCircle->setScale(0.75f);
    loadingCircle->setContentSize({ 0 , 0 });
    loadingCircle->setPosition({ size.width/2, size.height/2 });
    loadingCircle->setVisible(false);
    loadingCircle->m_sprite->setPosition({ 0 , 0 });
    loadingCircle->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    loadingCircle->setID("loading-circle");
    node->addChild(loadingCircle, 2);
    m_loadingCircle = loadingCircle;


    auto bonusBG = CCScale9Sprite::create("GJ_square02.png");
    bonusBG->setScale(0.5f);
    bonusBG->setContentSize({ size.width/2.5f, size.height/5.5f });
    bonusBG->setContentSize(bonusBG->getContentSize() * 2);
    bonusBG->setPosition({ size.width/5.f + 7.5f , size.height/11.f + 10.f });
    bonusBG->setColor({ 0, 0, 0 });
    bonusBG->setOpacity(50);
    bonusBG->setID("bonus-background");
    node->addChild(bonusBG, 1);

    auto bonusMenu = CCMenu::create();
    bonusMenu->setContentSize(bonusBG->getScaledContentSize());
    bonusMenu->setPosition(bonusBG->getPosition());
    // bonusMenu->setPositionX(bonusMenu->getPositionX() + (levelType != 0 ? 5.f : 2.f));
    bonusMenu->setID("bonus-menu");
    bonusMenu->setLayout(
        RowLayout::create()
        ->setGap(0.f)
        ->setAxisAlignment(AxisAlignment::Start)
        ->setAutoScale(false)
    );
    node->addChild(bonusMenu, 2);
    m_bonusMenu = bonusMenu;

    auto safeMenu = CCMenu::create();
    safeMenu->setPosition({ 0.f, 0.f });
    safeMenu->setContentSize(size);
    safeMenu->setID("safe-button-menu");
    node->addChild(safeMenu, 2);

    auto safeButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_safeBtn_001.png"),
        this,
        menu_selector(RDTimelyNode::onTheSafe)
    );
    safeButton->setScale(0.5f);
    // safeButton->setPosition(size - ccp(2, 2));
    safeButton->setPosition({ size.width/2.f, bonusBG->getPositionY() });
    safeButton->m_scaleMultiplier = 1.15f;
    safeButton->m_baseScale = 0.5f;
    safeButton->setID("safe-button");
    safeMenu->addChild(safeButton);
    m_safeButton = safeButton;

    // std::vector<int> timelyUnk = {GLM->m_activeDailyID, GLM->m_activeWeeklyID, GLM->m_activeEventID};
    // if (auto level = GLM->getSavedDailyLevel(timelyUnk[levelType])) {
    //     RDTimelyNode::setupLevelMenu(level);
    // } else {
    //     std::vector<int> downloadType = {-1, -2, -3};
    //     GLM->downloadLevel(downloadType[levelType], false);
    //     m_loadingCircle->setVisible(true);
    //     m_bonusMenu->setVisible(false);
    // }
    
    // std::vector<int> timelyLeft = {Variables::DailyLeft, Variables::WeeklyLeft, Variables::EventLeft};
    // auto time = timelyLeft[levelType];
    // if (time < 0) time = 0;
    
    // auto timeLabel = CCLabelBMFont::create(GameToolbox::getTimeString(time, true).c_str(), Mod::get()->getSettingValue<bool>("use-pusab") ? "bigFont.fnt" : "gjFont16.fnt");
    // timeLabel->setScale(0.55f);
    // timeLabel->setAnchorPoint({ 1, 0.5f });
    // timeLabel->setPosition({ innerBG->getPositionX() + innerBG->getScaledContentSize().width/2 , size.height/9 });
    // timeLabel->setID("time-label");
    // node->addChild(timeLabel, 1);
    // m_timeLabel = timeLabel;
    // if (levelType == 2) {
    //     // if (roundf(rand() / 32767.f * 1000) == 187) {
    //     //     string = "This message is extremely rare."; // why specifically 187??? what the fuck rob
    //     // } else {
    //     //     std::vector<std::string> dude = {"New event level in ... ?", "Events are pretty random", "Next event in random hours from now", "When is the next event?", "NEW EVENT! .... soon", "Why did I program this...", "I believe in you", "You can do it", "Next one is a banger", "This is fun", "I can\'t find anything", "Why is this a thing?", "Do or do not. There is no try", "The cake is a lie", "Challenge accepted", "I wonder what\'s next", "Welcome to the event", "Jump cube, jump", "No event for you", "This looks interesting", "Something epic is coming...", "Unlocking something... or not", "This could be legendary", "I wonder if it\'s a demon?", "A wild event appears!", "Let me cook", "Please wait, cooking.", "I don\'t think it\'s coming", "Geometry Dashn\'t", "Uber gamer moment", "Free event is free"};
    //     //     string = dude[rand() % dude.size()];
    //     // }
    //     timeLabel->setString("who knows...");
    //     timeLabel->setScale(0.35f);
    //     timeLabel->setColor({ 255, 0, 0 });
    // }

    // std::vector<const char*> timeLabelVctr = {"New Daily in:", "New Weekly in:", "New Event in:"};
    // auto timeLeftLabel = CCLabelBMFont::create(timeLabelVctr[levelType], "bigFont.fnt");
    // timeLeftLabel->setScale(0.3f);
    // timeLeftLabel->setAnchorPoint({ 1, 0.5f });
    // timeLeftLabel->setPosition({ timeLabel->getPositionX(), (safeButton->getPositionY() + innerBG->getPositionY() - innerBG->getScaledContentHeight()/2)/2});
    // timeLeftLabel->setColor({ 200, 200, 200 });
    // timeLeftLabel->setID("time-left-label");
    // node->addChild(timeLeftLabel, 1);
    // m_timeLeftLabel = timeLeftLabel;

    // auto timerLoadingCircle = LoadingCircle::create();
    // timerLoadingCircle->setScale(0.45f);
    // timerLoadingCircle->setContentSize({ 0 , 0 });
    // timerLoadingCircle->setPositionX(timeLeftLabel->getPositionX() - timeLeftLabel->getScaledContentWidth()/2);
    // timerLoadingCircle->setPositionY((timeLeftLabel->getPositionY() + timeLabel->getPositionY())/2);
    // timerLoadingCircle->setVisible(false);
    // timerLoadingCircle->m_sprite->setPosition({ 0 , 0 });
    // timerLoadingCircle->m_sprite->runAction(CCRepeatForever::create(CCRotateBy::create(1, 360)));
    // timerLoadingCircle->setID("timer-loading-circle");
    // node->addChild(timerLoadingCircle, 2);
    // m_timerLoadingCircle = timerLoadingCircle;

    // if (time == 0) {
    //     timeLabel->setVisible(false);
    //     timeLeftLabel->setVisible(false);
    //     timerLoadingCircle->setVisible(true);
    // }

    // if (levelType == 2) {
    //     timeLabel->setVisible(true);
    //     timeLeftLabel->setVisible(true);
    //     timerLoadingCircle->setVisible(false);
    // }

    this->setContentSize(size * scale);
    this->setID(id);

    return true;
}

void RDTimelyNode::onView(CCObject* sender) {
    auto sc = LevelInfoLayer::scene(m_currentLevel, false);
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, sc));
}

void RDTimelyNode::onCreatorLabel(CCObject* sender) {
    ProfilePage::create(m_currentLevel->m_accountID.value(), false)->show();
}

void RDTimelyNode::downloadLevelFailed() {
    m_loadingCircle->setVisible(false);
    m_menu->removeAllChildren();

    auto reloadButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        this,
        menu_selector(RDTimelyNode::onReload)
    );
    reloadButton->m_scaleMultiplier = 1.15f;
    reloadButton->setPosition({ m_menu->getContentWidth()*5.5f/6.5f, m_menu->getContentHeight()/2.f });
    reloadButton->setID("reload-button");
    m_menu->addChild(reloadButton);

    auto label = CCLabelBMFont::create("Something went wrong...", "goldFont.fnt");
    label->setScale(0.5f);
    label->setPositionX((m_innerBG->getPositionX() - m_innerBG->getScaledContentWidth()/2 + reloadButton->getPositionX() - reloadButton->getScaledContentWidth()/2)/2);
    label->setPositionY(reloadButton->getPositionY());
    label->setID("error-label");
    m_menu->addChild(label);
}

void RDTimelyNode::setupLevelMenu(GJGameLevel* level) {
    auto GLM = GameLevelManager::get();

    m_currentLevel = level;
    m_menu->removeAllChildren();
    m_loadingCircle->setVisible(false);
    setupBonusMenu(level);

    auto viewButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png"),
        this,
        menu_selector(RDTimelyNode::onView)
    );
    viewButton->setScale(0.6f);
    viewButton->m_baseScale = 0.6f;
    viewButton->m_scaleMultiplier = 1.15f;
    viewButton->setPosition({ m_menu->getContentWidth()*5.5f/6.5f, m_menu->getContentHeight()/2.f }); // dont ask...
    viewButton->setID("view-button");
    m_menu->addChild(viewButton, 10);
    m_viewButton = viewButton;

    int difficultyRating = 0;
    int featureRating = 0;
    if (level->m_demon.value()) {
        if (level->m_demonDifficulty == 0) difficultyRating += 2;
        difficultyRating += level->m_demonDifficulty + 4;
    } else {
        difficultyRating = level->getAverageDifficulty();
    }

    if (level->m_featured) featureRating += 1;
    featureRating += level->m_isEpic;

    auto difficultySprite = GJDifficultySprite::create(difficultyRating, GJDifficultyName::Short);
    difficultySprite->updateFeatureState(static_cast<GJFeatureState>(featureRating));
    difficultySprite->setScale(0.8f);
    difficultySprite->setPosition({ m_menu->getContentWidth()/7.5f, viewButton->getPositionY() + 2.5f });
    difficultySprite->setID("difficulty-sprite");
    m_menu->addChild(difficultySprite, 9);

    auto menuSize = m_innerBG->getScaledContentSize();
    auto baseY = m_innerBG->getPositionY() - menuSize.height/2;

    auto maxX = viewButton->getPositionX() - viewButton->getScaledContentWidth()/2 - difficultySprite->getPositionX() - difficultySprite->getScaledContentWidth()/2 - 5.f;

    auto nameLabel = CCLabelBMFont::create(level->m_levelName.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.5f);
    if (nameLabel->getScaledContentWidth() > maxX) {
        nameLabel->setScale(maxX / nameLabel->getContentWidth());
    }
    nameLabel->setAnchorPoint({ 0, 0.5f });
    nameLabel->setPositionX(difficultySprite->getPositionX() + difficultySprite->getScaledContentWidth()/2 + 5.f);
    nameLabel->setPositionY(baseY + menuSize.height*3/4);
    nameLabel->setID("name-label");
    m_menu->addChild(nameLabel, 8);

    auto creatorLabel = CCLabelBMFont::create(fmt::format("by {}", level->m_creatorName).c_str(), "goldFont.fnt");
    creatorLabel->setScale(0.5f);
    if (creatorLabel->getScaledContentWidth() > maxX) {
        creatorLabel->setScale(maxX / creatorLabel->getContentWidth());
    }
    auto creatorButton = CCMenuItemSpriteExtra::create(
        creatorLabel,
        this,
        menu_selector(RDTimelyNode::onCreatorLabel)
    );
    creatorButton->setPositionX(nameLabel->getPositionX() + creatorLabel->getScaledContentWidth()/2);
    creatorButton->setPositionY(baseY + menuSize.height/2);
    creatorButton->m_scaleMultiplier = 1.1f;
    creatorButton->setID("creator-button");
    m_menu->addChild(creatorButton, 8);

    auto songName = "[Failed to load song name]";
    if (auto song = MusicDownloadManager::sharedState()->getSongInfoObject(level->m_songID)) {
        songName = song->m_songName.c_str();
    }
    auto songLabel = CCLabelBMFont::create(songName, "bigFont.fnt");
    songLabel->setScale(0.35f);
    if (songLabel->getScaledContentWidth() > maxX) {
        songLabel->setScale(maxX / songLabel->getContentWidth());
    }
    songLabel->setAnchorPoint({ 0, 0.5f });
    songLabel->setColor({ 250, 110, 245 });
    songLabel->setPositionX(nameLabel->getPositionX());
    songLabel->setPositionY(baseY + menuSize.height/4);
    songLabel->setID("song-label");
    m_menu->addChild(songLabel, 8);

    auto starsLabel = CCLabelBMFont::create(std::to_string(level->m_stars).c_str(), "bigFont.fnt");
    starsLabel->setScale(0.32f);
    starsLabel->setPositionX(difficultySprite->getPositionX() - 5.f);
    starsLabel->setPositionY(baseY + menuSize.height/6);
    starsLabel->setID("stars-label");
    m_menu->addChild(starsLabel, 7);

    auto starSprite = CCSprite::createWithSpriteFrameName("star_small01_001.png");
    starSprite->setScale(0.8f);
    starSprite->setPositionX(starsLabel->getPositionX() + starsLabel->getScaledContentWidth()/2 + 5.f);
    starSprite->setPositionY(baseY + menuSize.height/6);
    starSprite->setID("star-sprite");
    m_menu->addChild(starSprite, 8);

    // pretty much copied from the level thumbnails mod code lmao
    if (auto image = typeinfo_cast<CCImage*>(Variables::ThumbnailsDict->objectForKey(fmt::format("thumbnail-{}", level->m_levelID.value())))) {
        downloadThumbnailFinished(image);
    } else {
        m_listener.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (!res->ok()) {
                    downloadThumbnailFail();
                } else {
                    auto data = res->data();
                    auto thread = std::thread([this, data]() {
                        auto image = Ref(new CCImage());
                        image->autorelease();
                        image->initWithImageData(const_cast<uint8_t*>(data.data()), data.size());
                        Variables::ThumbnailsDict->setObject(image, fmt::format("thumbnail-{}", m_currentLevel->m_levelID.value()));

                        geode::Loader::get()->queueInMainThread([this, image](){
                            downloadThumbnailFinished(image);
                        });
                    });
                    thread.detach();
                }
            }
        });

        auto req = web::WebRequest();
        m_listener.setFilter(req.get(fmt::format("https://raw.githubusercontent.com/cdc-sys/level-thumbnails/main/thumbs/{}.png", level->m_levelID.value())));
    }
}

void RDTimelyNode::downloadThumbnailFinished(CCImage* image) {
    if (m_mainNode && m_menu) {
        auto size = m_mainNode->getContentSize();
        auto key = fmt::format("thumbnail-{}", m_currentLevel->m_levelID.value());
        CCTexture2D* texture = new CCTexture2D();
        texture->autorelease();
        texture->initWithImage(image);

        auto clippingNode = CCClippingNode::create();
        clippingNode->setAnchorPoint({ 0.5f, 0.5f });
        clippingNode->setPosition(m_innerBG->getPosition());
        clippingNode->setContentSize(m_innerBG->getScaledContentSize());
        clippingNode->setAlphaThreshold(0.03f);
        clippingNode->setID("thumbnail-node");

        auto stencil = CCScale9Sprite::create("square02b_001.png");
        stencil->setScale(0.5f);
        stencil->setPosition(clippingNode->getContentSize()/2);
        stencil->setContentSize({ size.width*2 - 30.f, size.height / 1.16f });

        auto sprite = CCSprite::createWithTexture(texture);
        sprite->setPosition(stencil->getPosition());
        sprite->setScale(stencil->getScaledContentWidth() / sprite->getContentWidth());
        sprite->setOpacity(0);

        clippingNode->setStencil(stencil);
        clippingNode->addChild(sprite);
        m_menu->addChild(clippingNode, 0);
        sprite->runAction(CCFadeIn::create(0.25f));

        auto overlay = CCScale9Sprite::create("innerBG_overlay.png"_spr);
        overlay->setPosition(m_innerBG->getPosition());
        overlay->setScale(m_innerBG->getScale());
        overlay->setContentSize(m_innerBG->getContentSize());
        m_menu->addChild(overlay, 1);    
    }
}

void RDTimelyNode::downloadThumbnailFail() {
    auto failSprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    failSprite->setScale(0.35f);
    failSprite->setPosition(m_innerBG->getPosition() + m_innerBG->getScaledContentSize()/2 - failSprite->getScaledContentSize()/2 - ccp(2.f, 2.f));
    m_menu->addChild(failSprite);
}
