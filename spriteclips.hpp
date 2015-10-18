#ifndef _SPRITECLIPS_HPP_
#define _SPRITECLIPS_HPP_

#include <unordered_map>
#include <SDL/SDL.h>
#include <string>

enum sprite_t {
  CDOTA_Unit_Hero_Luna, CDOTA_Unit_Hero_Life_Stealer, CDOTA_Unit_Hero_Chen, CDOTA_Unit_Hero_Shadow_Demon, CDOTA_Unit_Hero_Clinkz, CDOTA_Unit_Hero_Earthshaker, CDOTA_Unit_Hero_Slardar, CDOTA_Unit_Hero_Puck, CDOTA_Unit_Hero_AncientApparition, CDOTA_Unit_Hero_Pudge, CDOTA_Unit_Hero_Invoker, CDOTA_Unit_Hero_PhantomAssassin, CDOTA_Unit_Hero_DoomBringer, CDOTA_Unit_Hero_Tiny, CDOTA_Unit_Hero_Riki, CDOTA_Unit_Hero_Bane, CDOTA_Unit_Hero_Obsidian_Destroyer, CDOTA_Unit_Hero_Lina, CDOTA_Unit_Hero_Meepo, CDOTA_Unit_Hero_Skywrath_Mage, CDOTA_Unit_Hero_Mirana, CDOTA_Unit_Hero_Omniknight, CDOTA_Unit_Hero_Alchemist, CDOTA_Unit_Hero_Batrider, CDOTA_Unit_Hero_Spectre, CDOTA_Unit_Hero_SkeletonKing, CDOTA_Unit_Hero_Tidehunter, CDOTA_Unit_Hero_Medusa, CDOTA_Unit_Hero_ChaosKnight, CDOTA_Unit_Hero_Lich, CDOTA_Unit_Hero_WitchDoctor, CDOTA_Unit_Hero_DeathProphet, CDOTA_Unit_Hero_Morphling, CDOTA_Unit_Hero_Rattletrap, CDOTA_Unit_Hero_Viper, CDOTA_Unit_Hero_Juggernaut, CDOTA_Unit_Hero_Dazzle, CDOTA_Unit_Hero_DrowRanger, CDOTA_Unit_Hero_KeeperOfTheLight, CDOTA_Unit_Hero_SandKing, CDOTA_Unit_Hero_Gyrocopter, CDOTA_Unit_Hero_Silencer, CDOTA_Unit_Hero_Broodmother, CDOTA_Unit_Hero_StormSpirit, CDOTA_Unit_Hero_Phoenix, CDOTA_Unit_Hero_Leshrac, CDOTA_Unit_Hero_Wisp, CDOTA_Unit_Hero_Terrorblade, CDOTA_Unit_Hero_LoneDruid, CDOTA_Unit_Hero_VengefulSpirit, CDOTA_Unit_Hero_EmberSpirit, CDOTA_Unit_Hero_Ursa, CDOTA_Unit_Hero_Nyx_Assassin, CDOTA_Unit_Hero_Enchantress, CDOTA_Unit_Hero_Naga_Siren, CDOTA_Unit_Hero_Tusk, CDOTA_Unit_Hero_TrollWarlord, CDOTA_Unit_Hero_TemplarAssassin, CDOTA_Unit_Hero_Razor, CDOTA_Unit_Hero_ShadowShaman, CDOTA_Unit_Hero_Necrolyte, CDOTA_Unit_Hero_Ogre_Magi, CDOTA_Unit_Hero_Sven, CDOTA_Unit_Hero_SpiritBreaker, CDOTA_Unit_Hero_Shredder, CDOTA_Unit_Hero_Pugna, CDOTA_Unit_Hero_Centaur, CDOTA_Unit_Hero_Slark, CDOTA_Unit_Hero_Axe, CDOTA_Unit_Hero_Nevermore, CDOTA_Unit_Hero_Magnataur, CDOTA_Unit_Hero_Windrunner, CDOTA_Unit_Hero_Bristleback, CDOTA_Unit_Hero_Elder_Titan, CDOTA_Unit_Hero_Brewmaster, CDOTA_Unit_Hero_DarkSeer, CDOTA_Unit_Hero_Jakiro, CDOTA_Unit_Hero_Lion, CDOTA_Unit_Hero_Furion, CDOTA_Unit_Hero_Rubick, CDOTA_Unit_Hero_QueenOfPain, CDOTA_Unit_Hero_AntiMage, CDOTA_Unit_Hero_Visage, CDOTA_Unit_Hero_Legion_Commander, CDOTA_Unit_Hero_PhantomLancer, CDOTA_Unit_Hero_Treant, CDOTA_Unit_Hero_Lycan, CDOTA_Unit_Hero_Sniper, CDOTA_Unit_Hero_Disruptor, CDOTA_Unit_Hero_Undying, CDOTA_Unit_Hero_FacelessVoid, CDOTA_Unit_Hero_Tinker, CDOTA_Unit_Hero_Enigma, CDOTA_Unit_Hero_Huskar, CDOTA_Unit_Hero_Zuus, CDOTA_Unit_Hero_Techies, CDOTA_Unit_Hero_Warlock, CDOTA_Unit_Hero_Venomancer, CDOTA_Unit_Hero_CrystalMaiden, CDOTA_Unit_Hero_Beastmaster, CDOTA_Unit_Hero_Bloodseeker, CDOTA_Unit_Hero_Abaddon, CDOTA_Unit_Hero_Kunkka, CDOTA_Unit_Hero_NightStalker, CDOTA_Unit_Hero_Weaver, CDOTA_Unit_Hero_BountyHunter, CDOTA_Unit_Hero_DragonKnight, CDOTA_Unit_Hero_EarthSpirit, CDOTA_Unit_Hero_Oracle, CDOTA_Unit_Hero_Winter_Wyvern, CDOTA_Unit_Courier, CDOTA_BaseNPC_Creep_Lane, CDOTA_BaseNPC_Creep_Siege, CDOTA_NPC_Observer_Ward, CDOTA_NPC_Observer_Ward_TrueSight, CDOTA_BaseNPC_Fort, CDOTA_BaseNPC_Barracks, CDOTA_BaseNPC_Tower, item_demon_edge, item_recipe_refresher, item_emptyitembg, item_shivas_guard, item_recipe_medallion_of_courage, item_quarterstaff, item_belt_of_strength, item_recipe, item_salve, item_aegis, item_recipe_cyclone, item_diffusal_blade_2, item_recipe_black_king_bar, item_stout_shield, item_recipe_mask_of_madness, item_recipe_dagon, item_power_treads_int, item_flask, item_claymore, item_cloak, item_skadi, item_bottle_haste, item_desolator, item_ring_of_regen, item_ring_of_protection, item_bfury, item_ring_of_aquila, item_recipe_basher, item_medallion_of_courage, item_ring_of_basilius_active, item_recipe_wraith_band, item_broadsword, item_travel_boots, item_necronomicon_2, item_recipe_veil_of_discord, item_mithril_hammer, item_recipe_mekansm, item_cyclone, item_blades_of_attack, item_courier_radiant_flying, item_ogre_axe, item_gloves_of_haste, item_gem, item_blade_mail, item_recipe_silver_edge, item_recipe_sphere, item_bottle_doubledamage, item_bottle_medium, item_vitality_booster, item_gloves, item_ward_observer, item_helm_of_the_dominator, item_urn_of_shadows, item_moon_shard, item_recipe_yasha, item_bottle_regeneration, item_monkey_king_bar, item_gauntlets, item_boots, item_recipe_armlet, item_recipe_hand_of_midas, item_recipe_urn_of_shadows, item_ward_dispenser_sentry, item_recipe_necronomicon, item_greater_crit, item_orchid, item_refresher, item_recipe_bloodstone, item_recipe_vladmir, item_point_booster, item_magic_stick, item_reaver, item_enchanted_mango, item_recipe_headdress, item_oblivion_staff, item_recipe_assault, item_tango_single, item_bottle_small, item_eagle, item_manta, item_recipe_heart, item_ancient_janggo, item_satanic, item_helm_of_iron_will, item_talisman_of_evasion, item_power_treads_str, item_power_treads_agi, item_recipe_pipe, item_recipe_lotus_orb, item_recipe_diffusal_blade, item_ward_sentry, item_recipe_greater_crit, item_tranquil_boots, item_wraith_band, item_tpscroll, item_magic_wand, item_glimmer_cape, item_armlet_active, item_buckler, item_blade_mail_axe_pw, item_orb_of_venom, item_hood_of_defiance, item_solar_crest, item_sange, item_cheese, item_maelstrom, item_radiance_inactive, item_power_treads, item_mantle, item_recipe_radiance, item_blink, item_vladmir, item_javelin, item_tango, item_recipe_sange, item_clarity, item_bracer, item_smoke_of_deceit, item_lifesteal, item_invis_sword, item_mask_of_madness, item_quelling_blade, item_recipe_maelstrom, item_octarine_core, item_pers, item_robe, item_soul_booster, item_recipe_mjollnir, item_ultimate_orb, item_relic, item_courier, item_flying_courier, item_mystic_staff, item_necronomicon, item_chainmail, item_heavens_halberd, item_dagon_4, item_mjollnir, item_sange_and_yasha, item_dagon_3, item_dagon_2, item_crimson_guard, item_veil_of_discord, item_cyclone_2, item_bottle_empty, item_necronomicon_3, item_guardian_greaves, item_ethereal_blade, item_recipe_force_staff, item_poor_mans_shield, item_bottle_invisibility, item_vanguard, item_ring_of_basilius, item_basher, item_recipe_soul_ring, item_recipe_crimson_guard, item_slippers, item_travel_boots_2, item_heart, item_radiance, item_tranquil_boots_active, item_recipe_manta, item_rapier, item_ring_of_aquila_active, item_sheepstick, item_soul_ring, item_sphere, item_void_stone, item_platemail, item_shadow_amulet, item_silver_edge, item_bloodstone, item_lotus_orb, item_hyperstone, item_gem_of_true_sight, item_recipe_buckler, item_dagon, item_recipe_bracer, item_arcane_boots, item_blade_of_alacrity, item_ultimate_scepter, item_bottle_bounty, item_force_staff, item_sobi_mask, item_recipe_satanic, item_recipe_lesser_crit, item_band_of_elvenskin, item_rod_of_atos, item_lesser_crit, item_recipe_shivas_guard, item_boots_of_elves, item_recipe_ancient_janggo, item_ghost_scepter, item_butterfly, item_recipe_desolator, item_greater_clarity, item_ring_of_health, item_circlet, item_recipe_guardian_greaves, item_recipe_orchid, item_assault, item_arcane_boots2, item_null_talisman, item_dust, item_abyssal_blade, item_bottle_illusion, item_headdress, item_ghost, item_dagon_5, item_staff_of_wizardry, item_hand_of_midas, item_branches, item_recipe_travel_boots, item_armlet, item_ward_dispenser, item_energy_booster, item_diffusal_blade, item_pipe, item_phase_boots, item_flask2, item_black_king_bar, item_bottle, item_yasha, item_recipe_null_talisman, item_mekansm
};

extern SDL_Rect h_offsets[343];

extern std::unordered_map<std::string, sprite_t> hero_map;
extern std::unordered_map<std::string, sprite_t> item_map;

void set_heroclips1();
void set_heroclips2();
void set_heroclips3();
void set_heroclips4();
void set_heroclips5();

void set_hero_map1();
void set_hero_map2();
void set_hero_map3();
void set_hero_map4();
void set_hero_map5();

void set_itemclips1();
void set_itemclips2();
void set_itemclips3();
void set_itemclips4();
void set_itemclips5();
void set_itemclips6();
void set_itemclips7();
void set_itemclips8();
void set_itemclips9();

void set_item_map1();
void set_item_map2();
void set_item_map3();
void set_item_map4();
void set_item_map5();
void set_item_map6();
void set_item_map7();
void set_item_map8();
void set_item_map9();

void init_sprites();

#endif /* _SPRITECLIPS_HPP_ */