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

struct StringTableItem {
  int index;
  std::string key;
  std::string value;
};

// Holds and maintains the information for a string table.
struct StringTable {
	int index;
	std::string name;
	std::unordered_map<int, struct StringTableItem> items;
	bool userDataFixedSize;
	int userDataSize;
};

// Holds and maintains the string table information for an
// instance of the Parser.
struct StringTables {
  std::unordered_map<int, struct StringTable> tables;
  std::unordered_map<std::string, int> nameIndex;
	int nextIndex;
  StringTables() {
    nextIndex = 0;
  }
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
    std::unordered_map<std::string, value_type> KV;
  
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
  std::unordered_map< std::string, PropertySerializer> serializers;
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
  std::unordered_map< std::string, std::unordered_map<int, dt> > serializers;
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
    std::unordered_map<int, std::string> classInfo;
    std::unordered_map<int, struct Properties> classBaselines;
    bool hasClassInfo;
    std::unordered_map< std::string, std::unordered_map<int, dt> > serializers;
    std::unordered_map< int, PacketEntity* > packetEntities;
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
    void readMessage(const char* buffer, int &pos);
    void parseMessage(int cmd, int tick, int size, const char* buffer);
    //uint32_t parseClassInfo(CDemoClassInfo*);
    uint32_t parseStringTables(const CDemoStringTables*);
    uint32_t parsePendingMessage(pendingMessage* msg);
    
    void onCDemoPacket(const CDemoPacket* packet);
    void onCDemoFullPacket(const CDemoFullPacket* packet);

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