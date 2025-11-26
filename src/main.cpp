#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

using namespace geode::prelude;

class $modify(GJBaseGameLayerHook, GJBaseGameLayer) {
    struct Fields {
        bool containsConfigObj;
        bool isPlaying;

        bool forceMouseVisible;

        int followGroupID;
        int itemFlagID;
        int xItemID;
        int yItemID;
        int xGameItemID;
        int yGameItemID;
    };

    TextGameObject* findSettingsObj() {
        for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
            if (obj->m_objectID != 914) continue;

            auto textObj = static_cast<TextGameObject*>(obj);
            if (std::string(textObj->m_text).starts_with("MGC!")) return textObj;
        }
        return nullptr;
    }

    void findArg(std::string_view pStr, const char* pArg, int& pVar) {
        const auto i = pStr.find(pArg);
        const auto val = i + strlen(pArg) + 1;
        pVar = i == std::string::npos || val >= pStr.size() ?
            0 : numFromString<int>(pStr.substr(val)).unwrapOrDefault();
    }
    void findArg(std::string_view pStr, const char* pArg, bool& pVar) {
        const auto i = pStr.find(pArg);
        pVar = i != std::string::npos;
    }
    void parseSettingsObj(TextGameObject* pObj) {
        auto fields = m_fields.self();

        fields->containsConfigObj = pObj;

        const auto str = pObj  ? std::string(pObj->m_text) : std::string();

        findArg(str, "FG", fields->followGroupID);
        findArg(str, "IF", fields->itemFlagID);
        findArg(str, "XI", fields->xItemID);
        findArg(str, "YI", fields->yItemID);
        findArg(str, "XGI", fields->xGameItemID);
        findArg(str, "YGI", fields->yGameItemID);
        findArg(str, "FMV", fields->forceMouseVisible);
    }

    // borrowed from eclipse cuz of preveters comment like 6 months ago
    CCPoint getGameMousePos() {
        const auto mousePos = cocos::getMousePos();

        const auto angle = m_gameState.m_cameraAngle / 0.01745328;
        const auto rotatedPos = ccp(
            mousePos.x * cos(angle) - mousePos.y * sin(angle),
            mousePos.x * sin(angle) + mousePos.y * cos(angle)
        );

        return m_gameState.m_cameraPosition + rotatedPos / m_gameState.m_cameraZoom;
    }

    void setItem(int pID, int pVal) {
        if (!pID) return;

        m_effectManager->updateCountForItem(pID, pVal);
        updateCounters(pID, pVal);
    }

    void updateMouseObjs() {
        auto fields = m_fields.self();
        if (!fields->followGroupID || !m_effectManager->countForItem(fields->itemFlagID)) return;

        CCPoint center;
        auto objs = static_cast<CCArray*>(getGroup(fields->followGroupID));
        if (!objs || !objs->count()) return;

        auto parent = getGroupParent(fields->followGroupID);
        if (parent && objs->containsObject(parent)) {
            center = parent->getPosition();
        }
        else if (objs->count() == 1) {
            center = static_cast<GameObject*>(objs->firstObject())->getPosition();
        }
        else {
            CCPoint max = {-FLT_MAX, -FLT_MAX}, min = {FLT_MAX, FLT_MAX};
            for (auto obj : CCArrayExt<GameObject*>(objs)) {
                const auto pos = obj->getPosition();
                if (max.x < pos.x) max.x = pos.x;
                if (max.y < pos.y) max.y = pos.y;
                if (min.x > pos.x) min.x = pos.x;
                if (min.y > pos.y) min.y = pos.y;
            }
            center = (min + max) / 2;
        }

        const auto mousePos = getGameMousePos();
        moveObjects(objs, mousePos.x - center.x, mousePos.y - center.y, false);
    }

    void updateMousePos() {
        auto fields = m_fields.self();

        if (fields->xItemID || fields->yItemID) {
            const auto pos = cocos::getMousePos();
            setItem(fields->xItemID, pos.x);
            setItem(fields->yItemID, pos.y);
        }

        if (fields->xGameItemID || fields->yGameItemID) {
            const auto pos = getGameMousePos();
            setItem(fields->xGameItemID, pos.x);
            setItem(fields->yGameItemID, pos.y);
        }
    }




    
    bool init() {
        if (!GJBaseGameLayer::init()) return false;
        // 0s out everything
        parseSettingsObj(nullptr);
        m_fields->isPlaying = false;
        return true;
    }

    void update(float p0) {
        GJBaseGameLayer::update(p0);
        auto fields = m_fields.self();

        if (!fields->containsConfigObj || !fields->isPlaying) return;

        updateMouseObjs();
        updateMousePos();
        if (fields->forceMouseVisible) PlatformToolbox::showCursor();
    }
};

class $modify(PlayLayer) {
    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto gjbgl = reinterpret_cast<GJBaseGameLayerHook*>(this);
        gjbgl->parseSettingsObj(gjbgl->findSettingsObj());
        gjbgl->m_fields->isPlaying = true;
    }
};

class $modify(LevelEditorLayer) {
    void onPlaytest() {
        auto gjbgl = reinterpret_cast<GJBaseGameLayerHook*>(this);
        gjbgl->parseSettingsObj(gjbgl->findSettingsObj());
        gjbgl->m_fields->isPlaying = true;

        LevelEditorLayer::onPlaytest();
    }

    void onStopPlaytest() {
        reinterpret_cast<GJBaseGameLayerHook*>(this)->m_fields->isPlaying = false;
        LevelEditorLayer::onStopPlaytest();
    }
};
