#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace Variables {
    extern int DailyLeft;
    extern int WeeklyLeft;
    extern int EventLeft;

    extern int GlobalRank;
    extern int OldStarsCount;

    extern cocos2d::SEL_MenuHandler BISelector;
    extern cocos2d::SEL_MenuHandler SupExSelector;
    extern cocos2d::SEL_MenuHandler GDDPSelector;
    extern cocos2d::SEL_MenuHandler RouletteSelector;
    extern cocos2d::SEL_MenuHandler G3DSelector;
    extern cocos2d::SEL_MenuHandler HigherOrLowerSelector;
    extern cocos2d::SEL_MenuHandler JFPSelector;

    extern Ref<CreatorLayer> PhantomCreatorLayer;

    extern Ref<cocos2d::CCDictionary> ThumbnailsDict;

    extern bool didGetGJChallenge;
    extern std::vector<ChallengesPage*> challengesPages;

    extern bool isFromRedash;
}
