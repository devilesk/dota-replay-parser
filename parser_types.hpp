#ifndef _PARSER_TYPES_HPP_
#define _PARSER_TYPES_HPP_

#include <string>
#include <map>
#include <unordered_map>
#include "boost/variant.hpp"
#include "bitstream.hpp"

#include "protobufs/ai_activity.pb.h"
#include "protobufs/demo.pb.h"
#include "protobufs/dota_commonmessages.pb.h"
#include "protobufs/dota_modifiers.pb.h"
#include "protobufs/usermessages.pb.h"
#include "protobufs/netmessages.pb.h"
#include "protobufs/networkbasetypes.pb.h"
#include "protobufs/usermessages.pb.h"
#include "protobufs/dota_usermessages.pb.h"
#include "protobufs/gameevents.pb.h"

// Holds and maintains the string table information for an
// instance of the Parser.
struct StringTables {
  std::map<int, struct StringTable> tables;
  std::map<std::string, int> nameIndex;
	int nextIndex;
  StringTables() {
    nextIndex = 0;
  }
};

// Holds and maintains the information for a string table.
struct StringTable {
	int index;
	std::string name;
	std::map<int, struct StringTableItem> items;
	bool userDataFixedSize;
	int userDataSize;
};

struct StringTableItem {
  int index;
  std::string key;
  std::string value;
};

typedef boost::variant<
  bool,
  char,
  float,
  double,
  uint16_t,
  uint32_t,
  uint64_t,
  int32_t,
  int64_t,
  std::string,
  std::vector<float>
> value_type;

struct dt_field;

class Properties {
  public:
    std::map<std::string, value_type> KV;
  
    void merge(Properties* p2);
    bool fetch(const std::string &k, value_type& v);
    bool fetchBool(const std::string &k, bool& v);
    bool fetchInt32(const std::string &k, int32_t& v);
    bool fetchUint32(const std::string &k, uint32_t& v);
    bool fetchUint64(const std::string &k, uint64_t& v);
    bool fetchFloat32(const std::string &k, float& v);
    bool fetchString(const std::string &k, std::string& v);
};

struct PropertySerializer {
  value_type (*decode)(dota::bitstream &stream, dt_field* f);
  value_type (*decodeContainer)(dota::bitstream &stream, dt_field* f);
  bool isArray;
  uint32_t length;
  PropertySerializer* arraySerializer;
  std::string name;
  /*PropertySerializer() {
    decode = nullptr;
    decodeContainer = nullptr;
  }*/
};

struct PropertySerializerTable {
  std::map< std::string, PropertySerializer> serializers;
};

// A datatable field
struct dt_field {
  std::string name;
  std::string encoder;
  std::string type;
  int index;
  int flags;
  bool has_flags;
  int bit_count;
  bool has_bit_count;
  float low_value;
  bool has_low_value;
  float high_value;
  bool has_high_value;
  int version;
  PropertySerializer serializer;
  uint32_t build;
};

// Field is always filled, table only for sub-tables
struct dt_property {
  struct dt_field field;
  struct dt* table;
};

// A single datatable
struct dt {
  std::string name;
  int flags;
  int version;
  std::vector<dt_property> properties;
};

struct HuffmanTree {
	int weight;
	bool isLeaf;
	int value;
	HuffmanTree* left;
	HuffmanTree* right;
};

// A single field to be read
struct fieldpath_field {
	std::string name;
	dt_field* field;
};

// A fieldpath, used to walk through the flattened table hierarchy
struct fieldpath {
  dt* parent;
	std::vector<fieldpath_field> fields;
	std::vector<int> index;
	HuffmanTree* tree;
	bool finished;
};

// Contains the weight and lookup function for a single operation
struct fieldpathOp {
	std::string name;
  void (*fn)(dota::bitstream &stream, fieldpath* fp);
	int weight;
};

// The flattened serializers object
struct flattened_serializers {
  std::map< std::string, std::map<int, dt> > serializers;
  CSVCMsg_FlattenedSerializer* proto;
  PropertySerializerTable pst;
  uint32_t build;
};

class PacketEntity {
  public:
    int index;
    int classId;
    std::string className;
    Properties classBaseline;
    Properties properties;
    int serial;
    dt* flatTbl;
    
    bool fetch(const std::string &k, value_type& v);
    bool fetchBool(const std::string &k, bool& v);
    bool fetchInt32(const std::string &k, int32_t& v);
    bool fetchUint32(const std::string &k, uint32_t& v);
    bool fetchUint64(const std::string &k, uint64_t& v);
    bool fetchFloat32(const std::string &k, float& v);
    bool fetchString(const std::string &k, std::string& v);
};

enum EntityEventType {
  EntityEventType_None,
  EntityEventType_Create,
  EntityEventType_Update,
  EntityEventType_Delete,
  EntityEventType_Leave
};

struct packetEntityUpdate {
  PacketEntity* pe;
  EntityEventType t;
};

struct pendingMessage {
  uint32_t tick;
  int type;
  std::string data;
};

// A function that can handle a game event.
typedef void (*packetEntityHandler)(PacketEntity* , EntityEventType);

class Parser {
  public:
    std::map<int, std::string> classInfo;
    std::map<int, struct Properties> classBaselines;
    bool hasClassInfo;
    std::map< std::string, std::map<int, dt> > serializers;
    std::map< int, PacketEntity* > packetEntities;
    std::vector< packetEntityHandler > packetEntityHandlers;
    StringTables stringTables;
    uint32_t GameBuild;
    uint32_t tick;
    int packetEntityFullPackets;
    int classIdSize;
    bool processPacketEntities;
    
    Parser() {
      tick = 0;
      packetEntityFullPackets = 0;
      hasClassInfo = false;
      processPacketEntities = true;
    };
    //uint32_t onCSVCMsg_CreateStringTable(CSVCMsg_CreateStringTable* data);
    //uint32_t onCSVCMsg_ServerInfo(CSVCMsg_ServerInfo* data);
    uint32_t updateInstanceBaseline();
    uint32_t updateInstanceBaselineItem(StringTableItem item);
    flattened_serializers parseSendTables(CDemoSendTables* sendTables, PropertySerializerTable pst);
    uint32_t readMessage(std::ifstream &stream);
    uint32_t parseMessage(int cmd, int tick, int size, char* buffer);
    uint32_t parseClassInfo(CDemoClassInfo*);
    uint32_t parseStringTables(const CDemoStringTables*);
    uint32_t parsePendingMessage(pendingMessage* msg);
    
    void onCDemoPacket(const CDemoPacket* packet, int tick);
    void onCDemoFullPacket(const CDemoFullPacket* packet, int tick);
/*
    void onCDemoStop(CDemoStop* data);
    void onCDemoFileHeader(CDemoFileHeader* data);
    void onCDemoFileInfo(CDemoFileInfo* data);
    void onCDemoSyncTick(CDemoSyncTick* data);
    void onCDemoSendTables(CDemoSendTables* data);
    void onCDemoClassInfo(CDemoClassInfo* data);
    void onCDemoStringTables(CDemoStringTables* data);
    void onCDemoPacket(CDemoPacket* data);
    void onCDemoSignonPacket(CDemoPacket* data);
    void onCDemoConsoleCmd(CDemoConsoleCmd* data);
    void onCDemoCustomData(CDemoCustomData* data);
    void onCDemoCustomDataCallbacks(CDemoCustomDataCallbacks* data);
    void onCDemoUserCmd(CDemoUserCmd* data);
    void onCDemoFullPacket(CDemoFullPacket* data);
    void onCDemoSaveGame(CDemoSaveGame* data);
    void onCDemoSpawnGroups(CDemoSpawnGroups* data);
    void onCNETMsg_NOP(CNETMsg_NOP* data);
    void onCNETMsg_Disconnect(CNETMsg_Disconnect* data);
    void onCNETMsg_SplitScreenUser(CNETMsg_SplitScreenUser* data);
    void onCNETMsg_Tick(CNETMsg_Tick* data);
    void onCNETMsg_StringCmd(CNETMsg_StringCmd* data);
    void onCNETMsg_SetConVar(CNETMsg_SetConVar* data);
    void onCNETMsg_SignonState(CNETMsg_SignonState* data);
    void onCNETMsg_SpawnGroup_Load(CNETMsg_SpawnGroup_Load* data);
    void onCNETMsg_SpawnGroup_ManifestUpdate(CNETMsg_SpawnGroup_ManifestUpdate* data);
    void onCNETMsg_SpawnGroup_SetCreationTick(CNETMsg_SpawnGroup_SetCreationTick* data);
    void onCNETMsg_SpawnGroup_Unload(CNETMsg_SpawnGroup_Unload* data);
    void onCNETMsg_SpawnGroup_LoadCompleted(CNETMsg_SpawnGroup_LoadCompleted* data);
    void onCSVCMsg_ServerInfo(CSVCMsg_ServerInfo* data);
    void onCSVCMsg_FlattenedSerializer(CSVCMsg_FlattenedSerializer* data);
    void onCSVCMsg_ClassInfo(CSVCMsg_ClassInfo* data);
    void onCSVCMsg_SetPause(CSVCMsg_SetPause* data);
    void onCSVCMsg_CreateStringTable(CSVCMsg_CreateStringTable* data);
    void onCSVCMsg_UpdateStringTable(CSVCMsg_UpdateStringTable* data);
    void onCSVCMsg_VoiceInit(CSVCMsg_VoiceInit* data);
    void onCSVCMsg_VoiceData(CSVCMsg_VoiceData* data);
    void onCSVCMsg_Print(CSVCMsg_Print* data);
    void onCSVCMsg_Sounds(CSVCMsg_Sounds* data);
    void onCSVCMsg_SetView(CSVCMsg_SetView* data);
    void onCSVCMsg_ClearAllStringTables(CSVCMsg_ClearAllStringTables* data);
    void onCSVCMsg_CmdKeyValues(CSVCMsg_CmdKeyValues* data);
    void onCSVCMsg_BSPDecal(CSVCMsg_BSPDecal* data);
    void onCSVCMsg_SplitScreen(CSVCMsg_SplitScreen* data);
    void onCSVCMsg_PacketEntities(CSVCMsg_PacketEntities* data);
    void onCSVCMsg_Prefetch(CSVCMsg_Prefetch* data);
    void onCSVCMsg_Menu(CSVCMsg_Menu* data);
    void onCSVCMsg_GetCvarValue(CSVCMsg_GetCvarValue* data);
    void onCSVCMsg_StopSound(CSVCMsg_StopSound* data);
    void onCSVCMsg_PeerList(CSVCMsg_PeerList* data);
    void onCSVCMsg_PacketReliable(CSVCMsg_PacketReliable* data);
    void onCSVCMsg_HLTVStatus(CSVCMsg_HLTVStatus* data);
    void onCSVCMsg_FullFrameSplit(CSVCMsg_FullFrameSplit* data);
    void onCUserMessageAchievementEvent(CUserMessageAchievementEvent* data);
    void onCUserMessageCloseCaption(CUserMessageCloseCaption* data);
    void onCUserMessageCloseCaptionDirect(CUserMessageCloseCaptionDirect* data);
    void onCUserMessageCurrentTimescale(CUserMessageCurrentTimescale* data);
    void onCUserMessageDesiredTimescale(CUserMessageDesiredTimescale* data);
    void onCUserMessageFade(CUserMessageFade* data);
    void onCUserMessageGameTitle(CUserMessageGameTitle* data);
    void onCUserMessageHintText(CUserMessageHintText* data);
    void onCUserMessageHudMsg(CUserMessageHudMsg* data);
    void onCUserMessageHudText(CUserMessageHudText* data);
    void onCUserMessageKeyHintText(CUserMessageKeyHintText* data);
    void onCUserMessageColoredText(CUserMessageColoredText* data);
    void onCUserMessageRequestState(CUserMessageRequestState* data);
    void onCUserMessageResetHUD(CUserMessageResetHUD* data);
    void onCUserMessageRumble(CUserMessageRumble* data);
    void onCUserMessageSayText(CUserMessageSayText* data);
    void onCUserMessageSayText2(CUserMessageSayText2* data);
    void onCUserMessageSayTextChannel(CUserMessageSayTextChannel* data);
    void onCUserMessageShake(CUserMessageShake* data);
    void onCUserMessageShakeDir(CUserMessageShakeDir* data);
    void onCUserMessageTextMsg(CUserMessageTextMsg* data);
    void onCUserMessageScreenTilt(CUserMessageScreenTilt* data);
    void onCUserMessageTrain(CUserMessageTrain* data);
    void onCUserMessageVGUIMenu(CUserMessageVGUIMenu* data);
    void onCUserMessageVoiceMask(CUserMessageVoiceMask* data);
    void onCUserMessageVoiceSubtitle(CUserMessageVoiceSubtitle* data);
    void onCUserMessageSendAudio(CUserMessageSendAudio* data);
    void onCUserMessageItemPickup(CUserMessageItemPickup* data);
    void onCUserMessageAmmoDenied(CUserMessageAmmoDenied* data);
    void onCUserMessageCrosshairAngle(CUserMessageCrosshairAngle* data);
    void onCUserMessageShowMenu(CUserMessageShowMenu* data);
    void onCUserMessageCreditsMsg(CUserMessageCreditsMsg* data);
    void onCUserMessageCloseCaptionPlaceholder(CUserMessageCloseCaptionPlaceholder* data);
    void onCUserMessageCameraTransition(CUserMessageCameraTransition* data);
    void onCUserMessageAudioParameter(CUserMessageAudioParameter* data);
    void onCEntityMessagePlayJingle(CEntityMessagePlayJingle* data);
    void onCEntityMessageScreenOverlay(CEntityMessageScreenOverlay* data);
    void onCEntityMessageRemoveAllDecals(CEntityMessageRemoveAllDecals* data);
    void onCEntityMessagePropagateForce(CEntityMessagePropagateForce* data);
    void onCEntityMessageDoSpark(CEntityMessageDoSpark* data);
    void onCEntityMessageFixAngle(CEntityMessageFixAngle* data);
    void onCMsgVDebugGameSessionIDEvent(CMsgVDebugGameSessionIDEvent* data);
    void onCMsgPlaceDecalEvent(CMsgPlaceDecalEvent* data);
    void onCMsgClearWorldDecalsEvent(CMsgClearWorldDecalsEvent* data);
    void onCMsgClearEntityDecalsEvent(CMsgClearEntityDecalsEvent* data);
    void onCMsgClearDecalsForSkeletonInstanceEvent(CMsgClearDecalsForSkeletonInstanceEvent* data);
    void onCMsgSource1LegacyGameEventList(CMsgSource1LegacyGameEventList* data);
    void onCMsgSource1LegacyListenEvents(CMsgSource1LegacyListenEvents* data);
    void onCMsgSource1LegacyGameEvent(CMsgSource1LegacyGameEvent* data);
    void onCMsgSosStartSoundEvent(CMsgSosStartSoundEvent* data);
    void onCMsgSosStopSoundEvent(CMsgSosStopSoundEvent* data);
    void onCMsgSosSetSoundEventParams(CMsgSosSetSoundEventParams* data);
    void onCMsgSosSetLibraryStackFields(CMsgSosSetLibraryStackFields* data);
    void onCMsgSosStopSoundEventHash(CMsgSosStopSoundEventHash* data);
    void onCDOTAUserMsg_AIDebugLine(CDOTAUserMsg_AIDebugLine* data);
    void onCDOTAUserMsg_ChatEvent(CDOTAUserMsg_ChatEvent* data);
    void onCDOTAUserMsg_CombatHeroPositions(CDOTAUserMsg_CombatHeroPositions* data);
    void onCDOTAUserMsg_CombatLogShowDeath(CDOTAUserMsg_CombatLogShowDeath* data);
    void onCDOTAUserMsg_CreateLinearProjectile(CDOTAUserMsg_CreateLinearProjectile* data);
    void onCDOTAUserMsg_DestroyLinearProjectile(CDOTAUserMsg_DestroyLinearProjectile* data);
    void onCDOTAUserMsg_DodgeTrackingProjectiles(CDOTAUserMsg_DodgeTrackingProjectiles* data);
    void onCDOTAUserMsg_GlobalLightColor(CDOTAUserMsg_GlobalLightColor* data);
    void onCDOTAUserMsg_GlobalLightDirection(CDOTAUserMsg_GlobalLightDirection* data);
    void onCDOTAUserMsg_InvalidCommand(CDOTAUserMsg_InvalidCommand* data);
    void onCDOTAUserMsg_LocationPing(CDOTAUserMsg_LocationPing* data);
    void onCDOTAUserMsg_MapLine(CDOTAUserMsg_MapLine* data);
    void onCDOTAUserMsg_MiniKillCamInfo(CDOTAUserMsg_MiniKillCamInfo* data);
    void onCDOTAUserMsg_MinimapDebugPoint(CDOTAUserMsg_MinimapDebugPoint* data);
    void onCDOTAUserMsg_MinimapEvent(CDOTAUserMsg_MinimapEvent* data);
    void onCDOTAUserMsg_NevermoreRequiem(CDOTAUserMsg_NevermoreRequiem* data);
    void onCDOTAUserMsg_OverheadEvent(CDOTAUserMsg_OverheadEvent* data);
    void onCDOTAUserMsg_SetNextAutobuyItem(CDOTAUserMsg_SetNextAutobuyItem* data);
    void onCDOTAUserMsg_SharedCooldown(CDOTAUserMsg_SharedCooldown* data);
    void onCDOTAUserMsg_SpectatorPlayerClick(CDOTAUserMsg_SpectatorPlayerClick* data);
    void onCDOTAUserMsg_TutorialTipInfo(CDOTAUserMsg_TutorialTipInfo* data);
    void onCDOTAUserMsg_UnitEvent(CDOTAUserMsg_UnitEvent* data);
    void onCDOTAUserMsg_ParticleManager(CDOTAUserMsg_ParticleManager* data);
    void onCDOTAUserMsg_BotChat(CDOTAUserMsg_BotChat* data);
    void onCDOTAUserMsg_HudError(CDOTAUserMsg_HudError* data);
    void onCDOTAUserMsg_ItemPurchased(CDOTAUserMsg_ItemPurchased* data);
    void onCDOTAUserMsg_Ping(CDOTAUserMsg_Ping* data);
    void onCDOTAUserMsg_ItemFound(CDOTAUserMsg_ItemFound* data);
    void onCDOTAUserMsg_SwapVerify(CDOTAUserMsg_SwapVerify* data);
    void onCDOTAUserMsg_WorldLine(CDOTAUserMsg_WorldLine* data);
    void onCDOTAUserMsg_ItemAlert(CDOTAUserMsg_ItemAlert* data);
    void onCDOTAUserMsg_HalloweenDrops(CDOTAUserMsg_HalloweenDrops* data);
    void onCDOTAUserMsg_ChatWheel(CDOTAUserMsg_ChatWheel* data);
    void onCDOTAUserMsg_ReceivedXmasGift(CDOTAUserMsg_ReceivedXmasGift* data);
    void onCDOTAUserMsg_UpdateSharedContent(CDOTAUserMsg_UpdateSharedContent* data);
    void onCDOTAUserMsg_TutorialRequestExp(CDOTAUserMsg_TutorialRequestExp* data);
    void onCDOTAUserMsg_TutorialPingMinimap(CDOTAUserMsg_TutorialPingMinimap* data);
    void onCDOTAUserMsg_GamerulesStateChanged(CDOTAUserMsg_GamerulesStateChanged* data);
    void onCDOTAUserMsg_ShowSurvey(CDOTAUserMsg_ShowSurvey* data);
    void onCDOTAUserMsg_TutorialFade(CDOTAUserMsg_TutorialFade* data);
    void onCDOTAUserMsg_AddQuestLogEntry(CDOTAUserMsg_AddQuestLogEntry* data);
    void onCDOTAUserMsg_SendStatPopup(CDOTAUserMsg_SendStatPopup* data);
    void onCDOTAUserMsg_TutorialFinish(CDOTAUserMsg_TutorialFinish* data);
    void onCDOTAUserMsg_SendRoshanPopup(CDOTAUserMsg_SendRoshanPopup* data);
    void onCDOTAUserMsg_SendGenericToolTip(CDOTAUserMsg_SendGenericToolTip* data);
    void onCDOTAUserMsg_SendFinalGold(CDOTAUserMsg_SendFinalGold* data);
    void onCDOTAUserMsg_CustomMsg(CDOTAUserMsg_CustomMsg* data);
    void onCDOTAUserMsg_CoachHUDPing(CDOTAUserMsg_CoachHUDPing* data);
    void onCDOTAUserMsg_ClientLoadGridNav(CDOTAUserMsg_ClientLoadGridNav* data);
    void onCDOTAUserMsg_TE_Projectile(CDOTAUserMsg_TE_Projectile* data);
    void onCDOTAUserMsg_TE_ProjectileLoc(CDOTAUserMsg_TE_ProjectileLoc* data);
    void onCDOTAUserMsg_TE_DotaBloodImpact(CDOTAUserMsg_TE_DotaBloodImpact* data);
    void onCDOTAUserMsg_TE_UnitAnimation(CDOTAUserMsg_TE_UnitAnimation* data);
    void onCDOTAUserMsg_TE_UnitAnimationEnd(CDOTAUserMsg_TE_UnitAnimationEnd* data);
    void onCDOTAUserMsg_AbilityPing(CDOTAUserMsg_AbilityPing* data);
    void onCDOTAUserMsg_ShowGenericPopup(CDOTAUserMsg_ShowGenericPopup* data);
    void onCDOTAUserMsg_VoteStart(CDOTAUserMsg_VoteStart* data);
    void onCDOTAUserMsg_VoteUpdate(CDOTAUserMsg_VoteUpdate* data);
    void onCDOTAUserMsg_VoteEnd(CDOTAUserMsg_VoteEnd* data);
    void onCDOTAUserMsg_BoosterState(CDOTAUserMsg_BoosterState* data);
    void onCDOTAUserMsg_WillPurchaseAlert(CDOTAUserMsg_WillPurchaseAlert* data);
    void onCDOTAUserMsg_TutorialMinimapPosition(CDOTAUserMsg_TutorialMinimapPosition* data);
    void onCDOTAUserMsg_PlayerMMR(CDOTAUserMsg_PlayerMMR* data);
    void onCDOTAUserMsg_AbilitySteal(CDOTAUserMsg_AbilitySteal* data);
    void onCDOTAUserMsg_CourierKilledAlert(CDOTAUserMsg_CourierKilledAlert* data);
    void onCDOTAUserMsg_EnemyItemAlert(CDOTAUserMsg_EnemyItemAlert* data);
    void onCDOTAUserMsg_StatsMatchDetails(CDOTAUserMsg_StatsMatchDetails* data);
    void onCDOTAUserMsg_MiniTaunt(CDOTAUserMsg_MiniTaunt* data);
    void onCDOTAUserMsg_BuyBackStateAlert(CDOTAUserMsg_BuyBackStateAlert* data);
    void onCDOTAUserMsg_SpeechBubble(CDOTAUserMsg_SpeechBubble* data);
    void onCDOTAUserMsg_CustomHeaderMessage(CDOTAUserMsg_CustomHeaderMessage* data);
    void onCDOTAUserMsg_QuickBuyAlert(CDOTAUserMsg_QuickBuyAlert* data);
    void onCDOTAUserMsg_PredictionResult(CDOTAUserMsg_PredictionResult* data);
    void onCDOTAUserMsg_ModifierAlert(CDOTAUserMsg_ModifierAlert* data);
    void onCDOTAUserMsg_HPManaAlert(CDOTAUserMsg_HPManaAlert* data);
    void onCDOTAUserMsg_GlyphAlert(CDOTAUserMsg_GlyphAlert* data);
    void onCDOTAUserMsg_BeastChat(CDOTAUserMsg_BeastChat* data);
    void onCDOTAUserMsg_SpectatorPlayerUnitOrders(CDOTAUserMsg_SpectatorPlayerUnitOrders* data);
    void onCDOTAUserMsg_CustomHudElement_Create(CDOTAUserMsg_CustomHudElement_Create* data);
    void onCDOTAUserMsg_CustomHudElement_Modify(CDOTAUserMsg_CustomHudElement_Modify* data);
    void onCDOTAUserMsg_CustomHudElement_Destroy(CDOTAUserMsg_CustomHudElement_Destroy* data);
    void onCDOTAUserMsg_CompendiumState(CDOTAUserMsg_CompendiumState* data);
    void onCDOTAUserMsg_ProjectionAbility(CDOTAUserMsg_ProjectionAbility* data);
    void onCDOTAUserMsg_ProjectionEvent(CDOTAUserMsg_ProjectionEvent* data);
*/
    void onCDemoStop(const std::string &raw_data);
    void onCDemoFileHeader(const std::string &raw_data);
    void onCDemoFileInfo(const std::string &raw_data);
    void onCDemoSyncTick(const std::string &raw_data);
    void onCDemoSendTables(const std::string &raw_data);
    void onCDemoClassInfo(const std::string &raw_data);
    void onCDemoStringTables(const std::string &raw_data);
    void onCDemoPacket(const std::string &raw_data);
    void onCDemoSignonPacket(const std::string &raw_data);
    void onCDemoConsoleCmd(const std::string &raw_data);
    void onCDemoCustomData(const std::string &raw_data);
    void onCDemoCustomDataCallbacks(const std::string &raw_data);
    void onCDemoUserCmd(const std::string &raw_data);
    void onCDemoFullPacket(const std::string &raw_data);
    void onCDemoSaveGame(const std::string &raw_data);
    void onCDemoSpawnGroups(const std::string &raw_data);
    void onCNETMsg_NOP(const std::string &raw_data);
    void onCNETMsg_Disconnect(const std::string &raw_data);
    void onCNETMsg_SplitScreenUser(const std::string &raw_data);
    void onCNETMsg_Tick(const std::string &raw_data);
    void onCNETMsg_StringCmd(const std::string &raw_data);
    void onCNETMsg_SetConVar(const std::string &raw_data);
    void onCNETMsg_SignonState(const std::string &raw_data);
    void onCNETMsg_SpawnGroup_Load(const std::string &raw_data);
    void onCNETMsg_SpawnGroup_ManifestUpdate(const std::string &raw_data);
    void onCNETMsg_SpawnGroup_SetCreationTick(const std::string &raw_data);
    void onCNETMsg_SpawnGroup_Unload(const std::string &raw_data);
    void onCNETMsg_SpawnGroup_LoadCompleted(const std::string &raw_data);
    void onCSVCMsg_ServerInfo(const std::string &raw_data);
    void onCSVCMsg_FlattenedSerializer(const std::string &raw_data);
    void onCSVCMsg_ClassInfo(const std::string &raw_data);
    void onCSVCMsg_SetPause(const std::string &raw_data);
    void onCSVCMsg_CreateStringTable(const std::string &raw_data);
    void onCSVCMsg_UpdateStringTable(const std::string &raw_data);
    void onCSVCMsg_VoiceInit(const std::string &raw_data);
    void onCSVCMsg_VoiceData(const std::string &raw_data);
    void onCSVCMsg_Print(const std::string &raw_data);
    void onCSVCMsg_Sounds(const std::string &raw_data);
    void onCSVCMsg_SetView(const std::string &raw_data);
    void onCSVCMsg_ClearAllStringTables(const std::string &raw_data);
    void onCSVCMsg_CmdKeyValues(const std::string &raw_data);
    void onCSVCMsg_BSPDecal(const std::string &raw_data);
    void onCSVCMsg_SplitScreen(const std::string &raw_data);
    void onCSVCMsg_PacketEntities(const std::string &raw_data);
    void onCSVCMsg_Prefetch(const std::string &raw_data);
    void onCSVCMsg_Menu(const std::string &raw_data);
    void onCSVCMsg_GetCvarValue(const std::string &raw_data);
    void onCSVCMsg_StopSound(const std::string &raw_data);
    void onCSVCMsg_PeerList(const std::string &raw_data);
    void onCSVCMsg_PacketReliable(const std::string &raw_data);
    void onCSVCMsg_HLTVStatus(const std::string &raw_data);
    void onCSVCMsg_FullFrameSplit(const std::string &raw_data);
    void onCUserMessageAchievementEvent(const std::string &raw_data);
    void onCUserMessageCloseCaption(const std::string &raw_data);
    void onCUserMessageCloseCaptionDirect(const std::string &raw_data);
    void onCUserMessageCurrentTimescale(const std::string &raw_data);
    void onCUserMessageDesiredTimescale(const std::string &raw_data);
    void onCUserMessageFade(const std::string &raw_data);
    void onCUserMessageGameTitle(const std::string &raw_data);
    void onCUserMessageHintText(const std::string &raw_data);
    void onCUserMessageHudMsg(const std::string &raw_data);
    void onCUserMessageHudText(const std::string &raw_data);
    void onCUserMessageKeyHintText(const std::string &raw_data);
    void onCUserMessageColoredText(const std::string &raw_data);
    void onCUserMessageRequestState(const std::string &raw_data);
    void onCUserMessageResetHUD(const std::string &raw_data);
    void onCUserMessageRumble(const std::string &raw_data);
    void onCUserMessageSayText(const std::string &raw_data);
    void onCUserMessageSayText2(const std::string &raw_data);
    void onCUserMessageSayTextChannel(const std::string &raw_data);
    void onCUserMessageShake(const std::string &raw_data);
    void onCUserMessageShakeDir(const std::string &raw_data);
    void onCUserMessageTextMsg(const std::string &raw_data);
    void onCUserMessageScreenTilt(const std::string &raw_data);
    void onCUserMessageTrain(const std::string &raw_data);
    void onCUserMessageVGUIMenu(const std::string &raw_data);
    void onCUserMessageVoiceMask(const std::string &raw_data);
    void onCUserMessageVoiceSubtitle(const std::string &raw_data);
    void onCUserMessageSendAudio(const std::string &raw_data);
    void onCUserMessageItemPickup(const std::string &raw_data);
    void onCUserMessageAmmoDenied(const std::string &raw_data);
    void onCUserMessageCrosshairAngle(const std::string &raw_data);
    void onCUserMessageShowMenu(const std::string &raw_data);
    void onCUserMessageCreditsMsg(const std::string &raw_data);
    void onCUserMessageCloseCaptionPlaceholder(const std::string &raw_data);
    void onCUserMessageCameraTransition(const std::string &raw_data);
    void onCUserMessageAudioParameter(const std::string &raw_data);
    void onCEntityMessagePlayJingle(const std::string &raw_data);
    void onCEntityMessageScreenOverlay(const std::string &raw_data);
    void onCEntityMessageRemoveAllDecals(const std::string &raw_data);
    void onCEntityMessagePropagateForce(const std::string &raw_data);
    void onCEntityMessageDoSpark(const std::string &raw_data);
    void onCEntityMessageFixAngle(const std::string &raw_data);
    void onCMsgVDebugGameSessionIDEvent(const std::string &raw_data);
    void onCMsgPlaceDecalEvent(const std::string &raw_data);
    void onCMsgClearWorldDecalsEvent(const std::string &raw_data);
    void onCMsgClearEntityDecalsEvent(const std::string &raw_data);
    void onCMsgClearDecalsForSkeletonInstanceEvent(const std::string &raw_data);
    void onCMsgSource1LegacyGameEventList(const std::string &raw_data);
    void onCMsgSource1LegacyListenEvents(const std::string &raw_data);
    void onCMsgSource1LegacyGameEvent(const std::string &raw_data);
    void onCMsgSosStartSoundEvent(const std::string &raw_data);
    void onCMsgSosStopSoundEvent(const std::string &raw_data);
    void onCMsgSosSetSoundEventParams(const std::string &raw_data);
    void onCMsgSosSetLibraryStackFields(const std::string &raw_data);
    void onCMsgSosStopSoundEventHash(const std::string &raw_data);
    void onCDOTAUserMsg_AIDebugLine(const std::string &raw_data);
    void onCDOTAUserMsg_ChatEvent(const std::string &raw_data);
    void onCDOTAUserMsg_CombatHeroPositions(const std::string &raw_data);
    void onCDOTAUserMsg_CombatLogShowDeath(const std::string &raw_data);
    void onCDOTAUserMsg_CreateLinearProjectile(const std::string &raw_data);
    void onCDOTAUserMsg_DestroyLinearProjectile(const std::string &raw_data);
    void onCDOTAUserMsg_DodgeTrackingProjectiles(const std::string &raw_data);
    void onCDOTAUserMsg_GlobalLightColor(const std::string &raw_data);
    void onCDOTAUserMsg_GlobalLightDirection(const std::string &raw_data);
    void onCDOTAUserMsg_InvalidCommand(const std::string &raw_data);
    void onCDOTAUserMsg_LocationPing(const std::string &raw_data);
    void onCDOTAUserMsg_MapLine(const std::string &raw_data);
    void onCDOTAUserMsg_MiniKillCamInfo(const std::string &raw_data);
    void onCDOTAUserMsg_MinimapDebugPoint(const std::string &raw_data);
    void onCDOTAUserMsg_MinimapEvent(const std::string &raw_data);
    void onCDOTAUserMsg_NevermoreRequiem(const std::string &raw_data);
    void onCDOTAUserMsg_OverheadEvent(const std::string &raw_data);
    void onCDOTAUserMsg_SetNextAutobuyItem(const std::string &raw_data);
    void onCDOTAUserMsg_SharedCooldown(const std::string &raw_data);
    void onCDOTAUserMsg_SpectatorPlayerClick(const std::string &raw_data);
    void onCDOTAUserMsg_TutorialTipInfo(const std::string &raw_data);
    void onCDOTAUserMsg_UnitEvent(const std::string &raw_data);
    void onCDOTAUserMsg_ParticleManager(const std::string &raw_data);
    void onCDOTAUserMsg_BotChat(const std::string &raw_data);
    void onCDOTAUserMsg_HudError(const std::string &raw_data);
    void onCDOTAUserMsg_ItemPurchased(const std::string &raw_data);
    void onCDOTAUserMsg_Ping(const std::string &raw_data);
    void onCDOTAUserMsg_ItemFound(const std::string &raw_data);
    void onCDOTAUserMsg_SwapVerify(const std::string &raw_data);
    void onCDOTAUserMsg_WorldLine(const std::string &raw_data);
    void onCDOTAUserMsg_ItemAlert(const std::string &raw_data);
    void onCDOTAUserMsg_HalloweenDrops(const std::string &raw_data);
    void onCDOTAUserMsg_ChatWheel(const std::string &raw_data);
    void onCDOTAUserMsg_ReceivedXmasGift(const std::string &raw_data);
    void onCDOTAUserMsg_UpdateSharedContent(const std::string &raw_data);
    void onCDOTAUserMsg_TutorialRequestExp(const std::string &raw_data);
    void onCDOTAUserMsg_TutorialPingMinimap(const std::string &raw_data);
    void onCDOTAUserMsg_GamerulesStateChanged(const std::string &raw_data);
    void onCDOTAUserMsg_ShowSurvey(const std::string &raw_data);
    void onCDOTAUserMsg_TutorialFade(const std::string &raw_data);
    void onCDOTAUserMsg_AddQuestLogEntry(const std::string &raw_data);
    void onCDOTAUserMsg_SendStatPopup(const std::string &raw_data);
    void onCDOTAUserMsg_TutorialFinish(const std::string &raw_data);
    void onCDOTAUserMsg_SendRoshanPopup(const std::string &raw_data);
    void onCDOTAUserMsg_SendGenericToolTip(const std::string &raw_data);
    void onCDOTAUserMsg_SendFinalGold(const std::string &raw_data);
    void onCDOTAUserMsg_CustomMsg(const std::string &raw_data);
    void onCDOTAUserMsg_CoachHUDPing(const std::string &raw_data);
    void onCDOTAUserMsg_ClientLoadGridNav(const std::string &raw_data);
    void onCDOTAUserMsg_TE_Projectile(const std::string &raw_data);
    void onCDOTAUserMsg_TE_ProjectileLoc(const std::string &raw_data);
    void onCDOTAUserMsg_TE_DotaBloodImpact(const std::string &raw_data);
    void onCDOTAUserMsg_TE_UnitAnimation(const std::string &raw_data);
    void onCDOTAUserMsg_TE_UnitAnimationEnd(const std::string &raw_data);
    void onCDOTAUserMsg_AbilityPing(const std::string &raw_data);
    void onCDOTAUserMsg_ShowGenericPopup(const std::string &raw_data);
    void onCDOTAUserMsg_VoteStart(const std::string &raw_data);
    void onCDOTAUserMsg_VoteUpdate(const std::string &raw_data);
    void onCDOTAUserMsg_VoteEnd(const std::string &raw_data);
    void onCDOTAUserMsg_BoosterState(const std::string &raw_data);
    void onCDOTAUserMsg_WillPurchaseAlert(const std::string &raw_data);
    void onCDOTAUserMsg_TutorialMinimapPosition(const std::string &raw_data);
    void onCDOTAUserMsg_PlayerMMR(const std::string &raw_data);
    void onCDOTAUserMsg_AbilitySteal(const std::string &raw_data);
    void onCDOTAUserMsg_CourierKilledAlert(const std::string &raw_data);
    void onCDOTAUserMsg_EnemyItemAlert(const std::string &raw_data);
    void onCDOTAUserMsg_StatsMatchDetails(const std::string &raw_data);
    void onCDOTAUserMsg_MiniTaunt(const std::string &raw_data);
    void onCDOTAUserMsg_BuyBackStateAlert(const std::string &raw_data);
    void onCDOTAUserMsg_SpeechBubble(const std::string &raw_data);
    void onCDOTAUserMsg_CustomHeaderMessage(const std::string &raw_data);
    void onCDOTAUserMsg_QuickBuyAlert(const std::string &raw_data);
    void onCDOTAUserMsg_PredictionResult(const std::string &raw_data);
    void onCDOTAUserMsg_ModifierAlert(const std::string &raw_data);
    void onCDOTAUserMsg_HPManaAlert(const std::string &raw_data);
    void onCDOTAUserMsg_GlyphAlert(const std::string &raw_data);
    void onCDOTAUserMsg_BeastChat(const std::string &raw_data);
    void onCDOTAUserMsg_SpectatorPlayerUnitOrders(const std::string &raw_data);
    void onCDOTAUserMsg_CustomHudElement_Create(const std::string &raw_data);
    void onCDOTAUserMsg_CustomHudElement_Modify(const std::string &raw_data);
    void onCDOTAUserMsg_CustomHudElement_Destroy(const std::string &raw_data);
    void onCDOTAUserMsg_CompendiumState(const std::string &raw_data);
    void onCDOTAUserMsg_ProjectionAbility(const std::string &raw_data);
    void onCDOTAUserMsg_ProjectionEvent(const std::string &raw_data);
};

typedef void (Parser::*Callback)(const std::string &raw_data);

#endif /* _PARSER_TYPES_HPP_ */