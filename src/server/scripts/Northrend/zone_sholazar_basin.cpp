/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "CombatAI.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptedEscortAI.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "Vehicle.h"

/*######
## npc_engineer_helice
######*/

enum EngineerHelice
{
    // Spells
    SPELL_DETONATE_1            = 52369,
    SPELL_DETONATE_2            = 52371,
    SPELL_EXPLOSION             = 46419,

    // Yells
    SAY_WP_1                    = 0,
    SAY_WP_2                    = 1,
    SAY_WP_3                    = 2,
    SAY_WP_4                    = 3,
    SAY_WP_5                    = 4,
    SAY_WP_6                    = 5,
    SAY_WP_7                    = 6,

    // Quests
    QUEST_DISASTER              = 12688
};

struct npc_engineer_helice : public EscortAI
{
    npc_engineer_helice(Creature* creature) : EscortAI(creature) {  }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        Player* player = GetPlayerForEscort();

        switch (waypointId)
        {
            case 0:
                Talk(SAY_WP_2);
                break;
            case 1:
                Talk(SAY_WP_3);
                DoCast(SPELL_DETONATE_1);
                break;
            case 7:
                Talk(SAY_WP_5);
                break;
            case 8:
                DoCast(SPELL_DETONATE_2);
                break;
            case 9:
                Talk(SAY_WP_6);
                break;
            case 13:
                if (player)
                {
                    player->GroupEventHappens(QUEST_DISASTER, me);
                    Talk(SAY_WP_7);
                }
                break;
            default:
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (Player* player = GetPlayerForEscort())
                player->FailQuest(QUEST_DISASTER);
        }
    }

    void QuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_DISASTER)
        {
            me->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_PASSIVE);

            Start(false, false, player->GetGUID());
            Talk(SAY_WP_1);
        }
    }
};

class spell_q12688_detonate_1 : public SpellScript
{
    PrepareSpellScript(spell_q12688_detonate_1);

    static constexpr uint32 GO_FLAMES = 182071;

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EXPLOSION });
    }

    void HandleDummyEffect(SpellEffIndex /* effIndex */)
    {
        if (Unit* target = GetHitUnit())
        {
            target->CastSpell(target, SPELL_EXPLOSION);

            static const struct
            {
                Position Pos;
                QuaternionData Quat;
            } Flames[] =
                    {
                            {{ 5921.5337f, 5371.778f,  -96.338776f, 0.f }, { 0.f, 0.f, 0.6883545f,   0.72537446f }},
                            {{ 5919.418f,  5366.918f,  -96.091125f, 0.f }, { 0.f, 0.f, 0.60876083f,  0.7933538f }},
                            {{ 5920.2563f, 5372.4136f, -98.85827f,  0.f }, { 0.f, 0.f, -0.32556725f, 0.94551885f }}
                    };

            for (auto flame : Flames)
                target->SummonGameObject(GO_FLAMES, flame.Pos, flame.Quat, 20s, GO_SUMMON_TIMED_DESPAWN);
        }
    }

    void HandleAfterHit()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->Talk(EngineerHelice::SAY_WP_4);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_q12688_detonate_1::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        AfterHit += SpellHitFn(spell_q12688_detonate_1::HandleAfterHit);
    }
};

class spell_q12688_detonate_2 : public SpellScript
{
    PrepareSpellScript(spell_q12688_detonate_2);

    static constexpr uint32 GO_FLAMES = 182071;

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EXPLOSION });
    }

    void HandleDummyEffect(SpellEffIndex /* effIndex */)
    {
        if (Unit* target = GetHitUnit())
        {
            target->CastSpell(target, SPELL_EXPLOSION);

            static const struct
            {
                Position Pos;
                QuaternionData Quat;
            } Flames[] =
                    {
                            {{ 5888.6816f, 5377.422f,  -92.617165f, 0.f }, { 0.f, 0.f, 0.9612608f,   0.2756405f }},
                            {{ 5886.481f,  5379.052f,  -92.97995f,  0.f }, { 0.f, 0.f, 0.41469288f,  0.90996146f }},
                            {{ 5887.204f,  5381.1753f, -93.50007f,  0.f }, { 0.f, 0.f, 0.061048508f, 0.9981348f }}
                    };

            for (auto flame : Flames)
                target->SummonGameObject(GO_FLAMES, flame.Pos, flame.Quat, 20s, GO_SUMMON_TIMED_DESPAWN);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_q12688_detonate_2::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

/*#####
## npc_jungle_punch_target
#####*/

#define SAY_OFFER     "Care to try Grimbooze Thunderbrew's new jungle punch?"

enum JunglePunch
{
    SPELL_OFFER                         = 51962,
    QUEST_TASTE_TEST                    = 12645,

    SAY_HEMET_HADRIUS_TAMARA_1          = 0,
    SAY_HEMET_HADRIUS_TAMARA_2          = 1,
    SAY_HEMET_HADRIUS_TAMARA_3          = 2,

    SAY_HEMET_4                         = 3, // unused
    SAY_HEMET_5                         = 4  // unused
};

enum NesingwaryChildrensWeek
{
    SPELL_ORPHAN_OUT                    = 58818,

    QUEST_THE_MIGHTY_HEMET_NESINGWARY   = 13957,

    ORPHAN_WOLVAR                       = 33532,

    TEXT_NESINGWARY_1                   = 5,

    TEXT_WOLVAR_ORPHAN_6                = 6,
    TEXT_WOLVAR_ORPHAN_7                = 7,
    TEXT_WOLVAR_ORPHAN_8                = 8,
    TEXT_WOLVAR_ORPHAN_9                = 9
};

class npc_jungle_punch_target : public CreatureScript
{
public:
    npc_jungle_punch_target() : CreatureScript("npc_jungle_punch_target") { }

    struct npc_jungle_punch_targetAI : public ScriptedAI
    {
        npc_jungle_punch_targetAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            sayTimer = 3500;
            sayStep = 0;
            timer = 0;
            phase = 0;
            playerGUID.Clear();
            orphanGUID.Clear();
        }

        void Reset() override
        {
            Initialize();
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!phase && who && who->GetDistance2d(me) < 10.0f)
                if (Player* player = who->ToPlayer())
                    if (player->GetQuestStatus(QUEST_THE_MIGHTY_HEMET_NESINGWARY) == QUEST_STATUS_INCOMPLETE)
                    {
                        playerGUID = player->GetGUID();
                        if (Aura* orphanOut = player->GetAura(SPELL_ORPHAN_OUT))
                            if (orphanOut->GetCaster() && orphanOut->GetCaster()->GetEntry() == ORPHAN_WOLVAR)
                            {
                                orphanGUID = orphanOut->GetCaster()->GetGUID();
                                phase = 1;
                            }
                    }
        }

        void proceedCwEvent(const uint32 diff)
        {
            if (timer <= diff)
            {
                Player* player = ObjectAccessor::GetPlayer(*me, playerGUID);
                Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGUID);

                if (!orphan || !player)
                {
                    Reset();
                    return;
                }

                switch (phase)
                {
                    case 1:
                        orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + std::cos(me->GetOrientation()) * 5, me->GetPositionY() + std::sin(me->GetOrientation()) * 5, me->GetPositionZ());
                        orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_6);
                        timer = 5000;
                        break;
                    case 2:
                        orphan->SetFacingToObject(me);
                        orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_7);
                        timer = 5000;
                        break;
                    case 3:
                        Talk(TEXT_NESINGWARY_1);
                        timer = 5000;
                        break;
                    case 4:
                        orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_8);
                        timer = 5000;
                        break;
                    case 5:
                        orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_9);
                        timer = 5000;
                        break;
                    case 6:
                        orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                        player->GroupEventHappens(QUEST_THE_MIGHTY_HEMET_NESINGWARY, me);
                        Reset();
                        return;
                }
                ++phase;
            }
            else
                timer -= diff;
        }

        void UpdateAI(uint32 diff) override
        {
            if (phase)
                proceedCwEvent(diff);

            if (!sayStep)
                return;

            if (sayTimer < diff)
            {
                Talk(SAY_HEMET_HADRIUS_TAMARA_1 + sayStep - 1);
                sayTimer = 3000;
                sayStep++;

                if (sayStep > 3) // end
                    sayStep = 0;
            }
            else
                sayTimer -= diff;
        }

        void SpellHit(WorldObject* caster, SpellInfo const* spellInfo) override
        {
            if (spellInfo->Id != SPELL_OFFER)
                return;

            Player* player = caster->ToPlayer();
            if (!player)
                return;

            Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_TASTE_TEST);
            if (!quest)
                return;

            QuestStatusMap::const_iterator itr = player->getQuestStatusMap().find(QUEST_TASTE_TEST);
            if (itr->second.Status != QUEST_STATUS_INCOMPLETE)
                return;

            for (uint8 i = 0; i < 3; ++i)
            {
                if (uint32(quest->RequiredNpcOrGo[i]) != me->GetEntry())
                    continue;

                if (itr->second.CreatureOrGOCount[i] != 0)
                    continue;

                player->KilledMonsterCredit(me->GetEntry());
                player->Say(SAY_OFFER, LANG_UNIVERSAL);
                sayStep = 1;
                break;
            }
        }

        private:
            uint16 sayTimer;
            uint8 sayStep;
            uint32 timer;
            int8 phase;
            ObjectGuid playerGUID;
            ObjectGuid orphanGUID;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_jungle_punch_targetAI(creature);
    }
};

/*######
## Quest The Lifewarden's Wrath
######*/

enum MiscLifewarden
{
    NPC_PRESENCE                 = 28563, // Freya's Presence
    NPC_SABOTEUR                 = 28538, // Cultist Saboteur
    NPC_SERVANT                  = 28320, // Servant of Freya

    WHISPER_ACTIVATE             = 0,

    SPELL_FREYA_DUMMY            = 51318,
    SPELL_LIFEFORCE              = 51395,
    SPELL_FREYA_DUMMY_TRIGGER    = 51335,
    SPELL_LASHER_EMERGE          = 48195,
    SPELL_WILD_GROWTH            = 52948,
};

class spell_q12620_the_lifewarden_wrath : public SpellScriptLoader
{
public:
    spell_q12620_the_lifewarden_wrath() : SpellScriptLoader("spell_q12620_the_lifewarden_wrath") { }

    class spell_q12620_the_lifewarden_wrath_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12620_the_lifewarden_wrath_SpellScript);

        void HandleSendEvent(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            if (Unit* caster = GetCaster())
            {
                if (Creature* presence = caster->FindNearestCreature(NPC_PRESENCE, 50.0f))
                {
                    presence->AI()->Talk(WHISPER_ACTIVATE, caster);
                    presence->CastSpell(presence, SPELL_FREYA_DUMMY, true); // will target plants
                    // Freya Dummy could be scripted with the following code

                    // Revive plants
                    std::list<Creature*> servants;
                    GetCaster()->GetCreatureListWithEntryInGrid(servants, NPC_SERVANT, 200.0f);
                    for (std::list<Creature*>::iterator itr = servants.begin(); itr != servants.end(); ++itr)
                    {
                        // Couldn't find a spell that does this
                        if ((*itr)->isDead())
                            (*itr)->Respawn(true);

                        (*itr)->CastSpell(*itr, SPELL_FREYA_DUMMY_TRIGGER, true);
                        (*itr)->CastSpell(*itr, SPELL_LASHER_EMERGE, false);
                        (*itr)->CastSpell(*itr, SPELL_WILD_GROWTH, false);

                        if (Unit* target = (*itr)->SelectNearestTarget(150.0f))
                            (*itr)->AI()->AttackStart(target);
                    }

                    // Kill nearby enemies
                    std::list<Creature*> saboteurs;
                    caster->GetCreatureListWithEntryInGrid(saboteurs, NPC_SABOTEUR, 200.0f);
                    for (std::list<Creature*>::iterator itr = saboteurs.begin(); itr != saboteurs.end(); ++itr)
                        if ((*itr)->IsAlive())
                            // Lifeforce has a cast duration, it should be cast at all saboteurs one by one
                            presence->CastSpell((*itr), SPELL_LIFEFORCE, false);
                }
            }
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_q12620_the_lifewarden_wrath_SpellScript::HandleSendEvent, EFFECT_0, SPELL_EFFECT_SEND_EVENT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_q12620_the_lifewarden_wrath_SpellScript();
    }
};

/*######
## Quest Kick, What Kick? (12589)
######*/

enum KickWhatKick
{
    NPC_LUCKY_WILHELM         = 28054,
    NPC_APPLE                 = 28053,
    NPC_DROSTAN               = 28328,
    NPC_CRUNCHY               = 28346,
    NPC_THICKBIRD             = 28093,

    SPELL_HIT_APPLE           = 51331,
    SPELL_MISS_APPLE          = 51332,
    SPELL_MISS_BIRD_APPLE     = 51366,
    SPELL_APPLE_FALL          = 51371,
    SPELL_BIRD_FALL           = 51369,

    EVENT_MISS                = 0,
    EVENT_HIT                 = 1,
    EVENT_MISS_BIRD           = 2,

    SAY_WILHELM_MISS          = 0,
    SAY_WILHELM_HIT           = 1,
    SAY_DROSTAN_REPLY_MISS    = 0,
};

class spell_q12589_shoot_rjr : public SpellScriptLoader
{
public:
    spell_q12589_shoot_rjr() : SpellScriptLoader("spell_q12589_shoot_rjr") { }

    class spell_q12589_shoot_rjr_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12589_shoot_rjr_SpellScript);

        SpellCastResult CheckCast()
        {
            if (Unit* target = GetExplTargetUnit())
                if (target->GetEntry() == NPC_LUCKY_WILHELM)
                    return SPELL_CAST_OK;

            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_TARGET_WILHELM);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            uint32 roll = urand(1, 100);

            uint8 ev;
            if (roll <= 50)
                ev = EVENT_MISS;
            else if (roll <= 83)
                ev = EVENT_HIT;
            else
                ev = EVENT_MISS_BIRD;

            Unit* shooter = GetCaster();
            Creature* wilhelm = GetHitUnit()->ToCreature();
            Creature* apple = shooter->FindNearestCreature(NPC_APPLE, 30);
            Creature* drostan = shooter->FindNearestCreature(NPC_DROSTAN, 30);

            if (!wilhelm || !apple || !drostan)
                return;

            switch (ev)
            {
                case EVENT_MISS_BIRD:
                {
                    Creature* crunchy = shooter->FindNearestCreature(NPC_CRUNCHY, 30);
                    Creature* bird = shooter->FindNearestCreature(NPC_THICKBIRD, 30);

                    if (bird && crunchy)
                    {
                        shooter->CastSpell(bird, SPELL_MISS_BIRD_APPLE);
                        bird->CastSpell(bird, SPELL_BIRD_FALL);
                        wilhelm->AI()->Talk(SAY_WILHELM_MISS);
                        drostan->AI()->Talk(SAY_DROSTAN_REPLY_MISS);

                        bird->KillSelf();
                        crunchy->GetMotionMaster()->MovePoint(0, bird->GetPositionX(), bird->GetPositionY(),
                            bird->GetMap()->GetWaterOrGroundLevel(bird->GetPhaseMask(), bird->GetPositionX(), bird->GetPositionY(), bird->GetPositionZ()));
                        /// @todo Make crunchy perform emote eat when he reaches the bird
                        break;
                    }
                    [[fallthrough]];
                }
                case EVENT_MISS:
                {
                    shooter->CastSpell(wilhelm, SPELL_MISS_APPLE);
                    wilhelm->AI()->Talk(SAY_WILHELM_MISS);
                    drostan->AI()->Talk(SAY_DROSTAN_REPLY_MISS);
                    break;
                }
                case EVENT_HIT:
                {
                    shooter->CastSpell(apple, SPELL_HIT_APPLE);
                    apple->CastSpell(apple, SPELL_APPLE_FALL);
                    wilhelm->AI()->Talk(SAY_WILHELM_HIT);
                    if (Player* player = shooter->ToPlayer())
                        player->KilledMonsterCredit(NPC_APPLE);
                    break;
                }
            }
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_q12589_shoot_rjr_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_q12589_shoot_rjr_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_q12589_shoot_rjr_SpellScript();
    }
};

/*######
## Quest: Song of Wind and Water ID: 12726
######*/
/*This quest precisly needs core script since battle vehicles are not well integrated with SAI,
may be easily converted to SAI when they get.*/
enum SongOfWindAndWater
{
    // Spells
    SPELL_DEVOUR_WIND     = 52862,
    SPELL_DEVOUR_WATER    = 52864,
    // NPCs
    NPC_HAIPHOON_WATER    = 28999,
    NPC_HAIPHOON_AIR      = 28985
};

class npc_haiphoon : public CreatureScript
{
public:
    npc_haiphoon() : CreatureScript("npc_haiphoon") { }

    struct npc_haiphoonAI : public VehicleAI
    {
        npc_haiphoonAI(Creature* creature) : VehicleAI(creature) { }

        void SpellHitTarget(WorldObject* target, SpellInfo const* spellInfo) override
        {
            if (target == me)
                return;

            if (spellInfo->Id == SPELL_DEVOUR_WIND && me->GetCharmerOrOwnerPlayerOrPlayerItself())
            {
                me->UpdateEntry(NPC_HAIPHOON_AIR);
            }
            else if (spellInfo->Id == SPELL_DEVOUR_WATER && me->GetCharmerOrOwnerPlayerOrPlayerItself())
            {
                me->UpdateEntry(NPC_HAIPHOON_WATER);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_haiphoonAI(creature);
    }
};

/*######
## Quest Dreadsaber Mastery: Stalking the Prey (12550)
######*/

enum ShangoTracks
{
    SPELL_CORRECT_TRACKS   = 52160,
    SPELL_INCORRECT_TRACKS = 52163,
    SAY_CORRECT_TRACKS     = 28634,
    SAY_INCORRECT_TRACKS   = 28635
};

class spell_shango_tracks : public SpellScriptLoader
{
public:
   spell_shango_tracks() : SpellScriptLoader("spell_shango_tracks") { }

    class spell_shango_tracks_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_shango_tracks_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (Unit* target = GetHitUnit())
            {
                switch (GetSpellInfo()->Id)
                {
                    case SPELL_CORRECT_TRACKS:
                        target->Say(SAY_CORRECT_TRACKS, target);
                        break;
                    case SPELL_INCORRECT_TRACKS:
                        target->Say(SAY_INCORRECT_TRACKS, target);
                        break;
                    default:
                        break;
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_shango_tracks_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_shango_tracks_SpellScript();
    }
};

enum ReturnedSevenfold
{
    SPELL_FREYAS_WARD           = 51845,
    SPELL_SEVENFOLD_RETRIBUTION = 51856,
    SPELL_DEATHBOLT             = 51855
};

class spell_q12611_deathbolt : public SpellScript
{
    PrepareSpellScript(spell_q12611_deathbolt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_FREYAS_WARD,
                SPELL_SEVENFOLD_RETRIBUTION,
                SPELL_DEATHBOLT
            });
    }

    void HandleScriptEffect(SpellEffIndex /* effIndex */)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (target->HasAura(SPELL_FREYAS_WARD))
            target->CastSpell(caster, SPELL_SEVENFOLD_RETRIBUTION, true);
        else
            caster->CastSpell(target, SPELL_DEATHBOLT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_q12611_deathbolt::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_sholazar_basin()
{
    RegisterCreatureAI(npc_engineer_helice);
    RegisterSpellScript(spell_q12688_detonate_1);
    RegisterSpellScript(spell_q12688_detonate_2);
    new npc_jungle_punch_target();
    new spell_q12620_the_lifewarden_wrath();
    new spell_q12589_shoot_rjr();
    new npc_haiphoon();
    new spell_shango_tracks();
    RegisterSpellScript(spell_q12611_deathbolt);
}
