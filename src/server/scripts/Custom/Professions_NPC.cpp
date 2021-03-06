#include "ScriptPCH.h"
 
class Professions_NPC : public CreatureScript
{
public:
    Professions_NPC () : CreatureScript("Professions_NPC") {}
 
    bool OnGossipHello(Player *pPlayer, Creature* _creature)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Alchemy", GOSSIP_SENDER_MAIN, SKILL_ALCHEMY);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blacksmithing", GOSSIP_SENDER_MAIN, SKILL_BLACKSMITHING);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Leatherworking", GOSSIP_SENDER_MAIN, SKILL_LEATHERWORKING);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tailoring", GOSSIP_SENDER_MAIN, SKILL_TAILORING);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Engineering", GOSSIP_SENDER_MAIN, SKILL_ENGINEERING);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Enchanting", GOSSIP_SENDER_MAIN, SKILL_ENCHANTING);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Jewelcrafting", GOSSIP_SENDER_MAIN, SKILL_JEWELCRAFTING);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Inscription", GOSSIP_SENDER_MAIN, SKILL_INSCRIPTION);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Cooking", GOSSIP_SENDER_MAIN, SKILL_COOKING);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "First Aid", GOSSIP_SENDER_MAIN, SKILL_FIRST_AID);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Herbalism", GOSSIP_SENDER_MAIN, SKILL_HERBALISM);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Skinning", GOSSIP_SENDER_MAIN, SKILL_SKINNING);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mining", GOSSIP_SENDER_MAIN, SKILL_MINING);
 
        pPlayer->PlayerTalkClass->SendGossipMenu(907, _creature->GetGUID());
        return true;
    }
 
    bool OnGossipSelect(Player* pPlayer, Creature* _creature, uint32 uiSender, uint32 SKILL)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
 
        if (uiSender == GOSSIP_SENDER_MAIN)
        {
            if(pPlayer->HasSkill(SKILL))
                pPlayer->GetSession()->SendNotification("You already have that skill");
            else
                CompleteLearnProfession(pPlayer, (SkillType)SKILL);
        }
        OnGossipHello(pPlayer, _creature);
        return true;
    }
 
    bool IsSecondarySkill(SkillType skill) const
    {
        return skill == SKILL_COOKING || skill == SKILL_FIRST_AID;
    }
 
    void CompleteLearnProfession(Player *pPlayer, SkillType skill)
    {
        if (PlayerAlreadyHasTwoProfessions(pPlayer) && !IsSecondarySkill(skill))
            pPlayer->GetSession()->SendNotification("You already know two professions!");
        else
        {
            if (!LearnAllRecipesInProfession(pPlayer, skill))
                pPlayer->GetSession()->SendNotification("Internal error occured!");
        }
    }
 
    bool LearnAllRecipesInProfession(Player* pPlayer, SkillType skill)
    {
      //  const char* skill_name;
      //  SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
      //  skill_name = SkillInfo->name[ChatHandler(pPlayer).GetSessionDbcLocale()];
 //
      //  if (!SkillInfo)
      //  {
      //      sLog->outError(LOG_FILTER_PLAYER_SKILLS, "Profession NPC: received non-valid skill ID (LearnAllRecipesInProfession)");
      //      return false;
      //  }      
 //
      //  LearnSkillRecipesHelper(pPlayer, SkillInfo->id);
 //
      //  pPlayer->SetSkill(SkillInfo->id, pPlayer->GetSkillStep(SkillInfo->id), 450, 450);
      //  ChatHandler(pPlayer).PSendSysMessage(LANG_COMMAND_LEARN_ALL_RECIPES, skill_name);
 //
//ERROR:Professions_NPC.cpp: In member function ‘bool Professions_NPC::LearnAllRecipesInProfession(Player*, SkillType)’:
//ERROR:Professions_NPC.cpp:63:57: error: no matching function for call to ‘ChatHandler::ChatHandler(Player*&)’
        return true;
    }
 
    bool PlayerAlreadyHasTwoProfessions(Player *pPlayer)
    {
        uint32 skillCount = 0;
 
        if (pPlayer->HasSkill(SKILL_MINING))
            skillCount++;
        if (pPlayer->HasSkill(SKILL_SKINNING))
            skillCount++;
        if (pPlayer->HasSkill(SKILL_HERBALISM))
            skillCount++;
 
        if (skillCount >= 2)
            return true;
 
        for (uint32 i = 1; i < sSkillLineStore.GetNumRows(); ++i)
        {
            SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(i);
            if (!SkillInfo)
                continue;
 
            if (SkillInfo->categoryId == SKILL_CATEGORY_SECONDARY)
                continue;
 
            if ((SkillInfo->categoryId != SKILL_CATEGORY_PROFESSION) || !SkillInfo->canLink)
                continue;
 
            const uint32 skillID = SkillInfo->id;
            if (pPlayer->HasSkill(skillID))
                skillCount++;
 
            if (skillCount >= 2)
                return true;
        }
        return false;
    }
 
    void LearnSkillRecipesHelper(Player *player, uint32 skill_id)
    {
        uint32 classmask = player->getClassMask();
 
        for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
        {
            SkillLineAbilityEntry const *skillLine = sSkillLineAbilityStore.LookupEntry(j);
            if (!skillLine)
                continue;
 
            // wrong skill
            if (skillLine->skillId != skill_id)
                continue;
 
            // not high rank
            if (skillLine->forward_spellid)
                continue;
 
            // skip racial skills
            if (skillLine->racemask != 0)
                continue;
 
            // skip wrong class skills
            if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
                continue;
 
            SpellInfo const * spellInfo = sSpellMgr->GetSpellInfo(skillLine->spellId);
            if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player, false))
                continue;
 
            player->learnSpell(skillLine->spellId, false);
        }
    }
};
 
void AddSC_Professions_NPC()
{
    new Professions_NPC();
}
