#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

// Buffer de salto: si das clic antes de tiempo, el mod guarda el clic
// y salta solo cuando tocas el suelo o un orbe dentro de la ventana.
class $modify(BufferLayer, GJBaseGameLayer) {
    struct Fields {
        float desdeInput = 999.f;
        bool pendiente = false;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        if (!Mod::get()->getSettingValue<bool>("activo")) return;

        if (down && isPlayer1) {
            m_fields->desdeInput = 0.f;
            m_fields->pendiente = true;
        }
    }

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        if (!Mod::get()->getSettingValue<bool>("activo")) return;
        if (!m_fields->pendiente) return;

        m_fields->desdeInput += dt;

        // Ventana configurable (en milisegundos)
        float ventana = static_cast<float>(
            Mod::get()->getSettingValue<int64_t>("ventana")
        ) / 1000.f;

        if (m_fields->desdeInput > ventana) {
            m_fields->pendiente = false;
            return;
        }

        bool enSuelo = m_player1->m_isOnGround;
        bool enOrbe = m_player1->m_touchingRings && m_player1->m_touchingRings->count() > 0;

        if (enSuelo || enOrbe) {
            m_player1->pushButton(PlayerButton::Jump);
            m_fields->pendiente = false;
        }
    }
};
