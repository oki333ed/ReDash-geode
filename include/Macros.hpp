#pragma once

#define RD_SETVISIBLE_SAFE(parent, id, visible) \
    if (auto child = parent->getChildByIDRecursive(id)) child->setVisible(visible);

#define RD_GET_SELECTOR(layer, id, var) \
	if (auto button = layer->getChildByIDRecursive(id)) var = static_cast<CCMenuItemSpriteExtra*>(button)->m_pfnSelector;
	

#define RD_ADD_CREATOR_BUTTON(mod, id, selector, texture) \
	if (loader->isModLoaded(mod) && selector != nullptr) {			\
		auto btn = CCMenuItemSpriteExtra::create(					\
			CCSprite::createWithSpriteFrameName(texture),			\
			this,													\
			selector												\
		);															\
		btn->setID(id);												\
		rightMenu->addChild(btn); 									\
	}

#define MAX_SECRET_COINS 164

#define RD_HANDLE_LEVEL(nodeID, IDUnk) \
    if (auto node = typeinfo_cast<RDTimelyNode*>(layer->getChildByID("redash-menu"_spr)->getChildByID("dailies-menu"_spr)->getChildByID(nodeID))) {         \
        if (response != "-1") {                                                                                                                             \
            if (auto level = this->getSavedDailyLevel(IDUnk)) {                                                                                             \
                node->setupLevelMenu(level);                                                                                                                \
            } else {                                                                                                                                        \
                log::error("where did my {} level go??", nodeID);                                                                                           \
            }                                                                                                                                               \
        } else {                                                                                                                                            \
            node->downloadLevelFailed();                                                                                                                    \
        } \
    }

#define RD_HANDLE_GET_DAILY(NodeType, nodeID, ID, IDUnk) \
if (auto layer = CCDirector::sharedDirector()->getRunningScene()->getChildByType<MenuLayer>(0)) {                                                           \
        if (auto node = typeinfo_cast<NodeType*>(layer->getChildByID("redash-menu"_spr)->getChildByID("dailies-menu"_spr)->getChildByID(nodeID))) {         \
            node->updateTimeLabel(1.f);                                                                                                                     \
            node->schedule(schedule_selector(NodeType::updateTimeLabel), 1.f);                                                                              \
            if (node->m_skipButton) {                                                                                                                       \
                if (IDUnk < ID && (node->m_currentLevel->m_normalPercent < 100 || GameStatsManager::sharedState()->hasCompletedDailyLevel(IDUnk))) {        \
                    node->m_skipButton->setVisible(true);                                                                                                   \
                } else {                                                                                                                                    \
                    node->m_skipButton->setVisible(false);                                                                                                  \
                }                                                                                                                                           \
            }                                                                                                                                               \
        } \
    }
