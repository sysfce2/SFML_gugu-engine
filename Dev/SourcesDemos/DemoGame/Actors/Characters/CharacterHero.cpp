////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Actors/Characters/CharacterHero.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Actors/Projectiles/Projectile.h"
#include "UI/ElementBar.h"
#include "Level/Grid.h"
#include "Skills/SkillUtility.h"

#include "DatasheetBinding.h"

#include "Gugu/World/Level.h"
#include "Gugu/Element/2D/ElementSpriteAnimated.h"
#include "Gugu/Manager/ManagerResources.h"
#include "Gugu/Utility/Math.h"
#include "Gugu/Utility/Random.h"
#include "Gugu/Utility/System.h"

using namespace gugu;

////////////////////////////////////////////////////////////////
// File Implementation

namespace demoproject {

CharacterHero::CharacterHero()
{
    m_attackSpeed = 10;
    m_attackStaminaCost = 0.1f;

    m_attackCooldown = 0.f;
    m_skillCooldown = 0.f;
    m_staminaRecoveryCooldown = 0.f;

    m_experience = 0;
    m_totalPoints = 0;
    m_spentPoints = 0;
}

CharacterHero::~CharacterHero()
{
}

void CharacterHero::InitHero(DS_Hero* sheetHero, float _fSpeed, Grid* grid)
{
    m_grid = grid;

    // Init stats from datasheet
    m_maxLife = sheetHero->health;
    m_walkSpeed = sheetHero->speed;

    m_maxStamina = sheetHero->stamina;
    m_staminaRecovery = sheetHero->staminaRecovery;
    m_staminaRecoveryDelay = sheetHero->staminaRecoveryDelay;

    m_currentLife = m_maxLife;
    m_currentStamina = m_maxStamina;

    // Sprite
    m_sprite = m_level->GetRootNode()->AddChild<ElementSpriteAnimated>();
    m_sprite->ChangeAnimSet(sheetHero->sprite->animSet);
    m_sprite->StartAnimation("IdleDown");
    m_sprite->SetUnifiedOrigin(UDim2::POSITION_CENTER);

    //m_pSprite->SetColor(sf::Color::Cyan);
    m_sprite->SetZIndex(1000);
}

void CharacterHero::Attack(const sf::Vector2f& _kCoords, const DeltaTime& dt)
{
    m_isActingThisFrame = true;

    // Use a skill for the base attack.
    //DS_Skill* sheetSkill = GetResources()->GetDatasheet<DS_Skill>("Fireball.skill");
    DS_Skill* sheetSkill = GetResources()->GetDatasheet<DS_Skill>("BowAttack.skill");

    if (CanUseSkill(sheetSkill))
    {
        int nbUse = 1;
        if (sheetSkill->useAttackSpeed)
        {
            float attackDelay = (m_attackSpeed > 0.f) ? 1.f / m_attackSpeed : 1.f;
            nbUse = Max(1, (int)(dt.s() / attackDelay));

            m_attackCooldown = attackDelay * nbUse;

            if (sheetSkill->staminaCost > 0.f)
            {
                m_staminaRecoveryCooldown = m_staminaRecoveryDelay;
            }
        }
        else
        {
            m_skillCooldown = sheetSkill->cooldown;
        }

        for (int i = 0; i < nbUse; ++i)
        {
            if (sheetSkill->staminaCost > 0.f)
            {
                if (m_currentStamina <= 0.f)   // Allow action when we have at least 1 stamina point (allow negative values)
                {
                    break;
                }

                m_currentStamina -= m_attackStaminaCost;
            }

            // Use skill.
            SkillContext context;
            context.skill = sheetSkill;
            context.caster = this;
            context.target = nullptr;
            context.aim = _kCoords;
            context.level = m_level;

            SkillUtility::UseSkill(context);
        }

        sf::Vector2f direction;
        if (_kCoords != GetPosition())
        {
            direction = Normalize(_kCoords - GetPosition());
        }
        else
        {
            direction = sf::Vector2f(1.f, 0.f);
        }

        float fAngleDegrees = ToDegreesf(atan2f(direction.y, direction.x));

        std::string strAnim = "";
        if (fAngleDegrees >= -45.f && fAngleDegrees <= 45.f)
            strAnim = "AttackRight";
        else if (fAngleDegrees <= -135.f || fAngleDegrees >= 135.f)
            strAnim = "AttackLeft";
        else if (fAngleDegrees >= 45.f && fAngleDegrees <= 135.f)
            strAnim = "AttackDown";
        else if (fAngleDegrees <= -45.f && fAngleDegrees >= -135.f)
            strAnim = "AttackUp";

        if (strAnim != "" && !m_sprite->IsAnimationPlaying(strAnim))
            m_sprite->StartAnimation(strAnim);
    }
}

bool CharacterHero::CanUseSkill(DS_Skill* skill) const
{
    if (Character::CanUseSkill(skill))
    {
        // Allow action when we have at least 1 stamina point (allow negative values).
        if (skill->staminaCost > 0.f && m_currentStamina <= 0.f)
        {
            return false;
        }

        if (skill->useAttackSpeed && m_attackCooldown > 0.f)
        {
            return false;
        }
        else if (skill->cooldown > 0.f && m_skillCooldown > 0.f)
        {
            return false;
        }

        return true;
    }

    return false;
}

void CharacterHero::Step(const DeltaTime& dt)
{
    Character::Step(dt);

    // Stamina recovery
    if (m_staminaRecoveryCooldown > 0.f)
    {
        m_staminaRecoveryCooldown -= dt.s();
    }

    if (m_staminaRecoveryCooldown <= 0.f)
    {
        m_currentStamina += dt.s() * m_staminaRecovery;
        m_currentStamina = Min(m_currentStamina, m_maxStamina);
    }

    // Attack cooldown
    if (m_attackCooldown > 0.f)
    {
        m_attackCooldown -= dt.s();
    }

    // Skill cooldown
    if (m_skillCooldown > 0.f)
    {
        m_skillCooldown -= dt.s();
    }
}

void CharacterHero::Update(const DeltaTime& dt)
{
    Character::Update(dt);

    // UI
    if (m_lifeBar)
    {
        m_lifeBar->SetValue(m_currentLife, m_maxLife);
    }
}

void CharacterHero::NotifyOpponentKilled(int value)
{
    m_experience += value;
    m_totalPoints = m_experience / 25;
}

}   //namespace demoproject