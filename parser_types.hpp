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
	std::unordered_map<int, std::shared_ptr<StringTableItem>> items;
	bool userDataFixedSize;
	int userDataSize;
};

// Holds and maintains the string table information for an
// instance of the Parser.
struct StringTables {
  std::unordered_map<int, std::shared_ptr<StringTable>> tables;
  std::unordered_map<std::string, int> nameIndex;
	int nextIndex;
  StringTables() {
    nextIndex = 0;
    tables.reserve(32);
    nameIndex.reserve(32);
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
struct dt;

class Properties {
  public:
    std::unordered_map<std::string, value_type> KV;

    Properties() {
        KV.reserve(256);
    }

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
  std::shared_ptr<PropertySerializer> arraySerializer;
  std::string name;
  /*PropertySerializer() {
    decode = nullptr;
    decodeContainer = nullptr;
  }*/
};

struct PropertySerializerTable {
  std::unordered_map< std::string, std::shared_ptr<PropertySerializer>> serializers;
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
  std::shared_ptr<PropertySerializer> serializer;
  uint32_t build;
};

// Field is always filled, table only for sub-tables
struct dt_property {
  std::shared_ptr<dt_field> field;
  std::shared_ptr<dt> table;
};

// A single datatable
struct dt {
  std::string name;
  int flags;
  int version;
  std::vector<std::shared_ptr<dt_property>> properties;
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
	std::shared_ptr<dt_field> field;
};

// A fieldpath, used to walk through the flattened table hierarchy
struct fieldpath {
  std::shared_ptr<dt> parent;
	std::vector<std::shared_ptr<fieldpath_field>> fields;
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
  std::unordered_map< std::string, std::unordered_map<int, std::shared_ptr<dt>> > serializers;
  std::shared_ptr<PropertySerializerTable> pst;
  uint32_t build;
};

class PacketEntity {
  public:
    int index;
    int classId;
    std::string className;
    std::shared_ptr<Properties> classBaseline;
    std::shared_ptr<Properties> properties;
    int serial;
    std::shared_ptr<dt> flatTbl;

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
  std::shared_ptr<PacketEntity> pe;
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
  private:
    static const std::string demheader;
    std::ifstream stream;
    int length;
    char* buffer;
    int pos;
    /** Position cache for fullpackets */
    std::vector<uint32_t> fpackcache;
  public:
    std::vector<uint32_t> fpackcachetick;
    std::unordered_map<int, std::string> classInfo;
    std::unordered_map<int, std::shared_ptr<Properties>> classBaselines;
    bool hasClassInfo;
    std::unordered_map< std::string, std::unordered_map<int, std::shared_ptr<dt>> > serializers;
    std::unordered_map< int, std::shared_ptr<PacketEntity> > packetEntities;
    std::vector< packetEntityHandler > packetEntityHandlers;
    StringTables stringTables;
    uint32_t GameBuild;
    uint32_t tick;
    uint32_t stopTick;
    int packetEntityFullPackets;
    int classIdSize;
    bool processPacketEntities;
    bool syncTick;

    Parser() {
      pos = 0;
      tick = 0;
      stopTick = 0;
      packetEntityFullPackets = 0;
      hasClassInfo = false;
      processPacketEntities = true;
      syncTick = false;
    };

    void open(std::string path);
    void readHeader();
    inline bool good() const {
      return pos < length;
    }
    inline void read() {
      readMessage(buffer, pos);
    }
    inline void close() {
      delete[] buffer;
    }
    void handle();
    void skipTo(uint32_t tick);
    void seekToFullPacket(int _tick);
    void generateFullPacketCache();

    //uint32_t onCSVCMsg_CreateStringTable(CSVCMsg_CreateStringTable* data);
    //uint32_t onCSVCMsg_ServerInfo(CSVCMsg_ServerInfo* data);
    uint32_t updateInstanceBaseline();
    uint32_t updateInstanceBaselineItem(StringTableItem* item);
    flattened_serializers parseSendTables(CDemoSendTables* sendTables, std::shared_ptr<PropertySerializerTable> pst);
    void readMessageHeader(const char* buffer, int &pos, uint32_t& cmd, uint32_t& _tick, uint32_t& size, bool& compressed);
    void readMessage(const char* buffer, int &pos);
    void parseMessage(int cmd, int tick, int size, const char* buffer);
    //uint32_t parseClassInfo(CDemoClassInfo*);
    uint32_t parseStringTables(const CDemoStringTables*);
    uint32_t parsePendingMessage(pendingMessage* msg);

    void onCDemoPacket(const CDemoPacket* packet);
    void onCDemoFullPacket(const CDemoFullPacket* packet);
    void onCDemoStringTables(const CDemoStringTables* string_table);

    void onCDemoStop(const char* buffer, int size);
    void onCDemoFileHeader(const char* buffer, int size);
    void onCDemoFileInfo(const char* buffer, int size);
    void onCDemoSyncTick(const char* buffer, int size);
    void onCDemoSendTables(const char* buffer, int size);
    void onCDemoClassInfo(const char* buffer, int size);
    void onCDemoStringTables(const char* buffer, int size);
    void onCDemoPacket(const char* buffer, int size);
    void onCDemoSignonPacket(const char* buffer, int size);
    void onCDemoConsoleCmd(const char* buffer, int size);
    void onCDemoCustomData(const char* buffer, int size);
    void onCDemoCustomDataCallbacks(const char* buffer, int size);
    void onCDemoUserCmd(const char* buffer, int size);
    void onCDemoFullPacket(const char* buffer, int size);
    void onCDemoSaveGame(const char* buffer, int size);
    void onCDemoSpawnGroups(const char* buffer, int size);
    void onCNETMsg_NOP(const char* buffer, int size);
    void onCNETMsg_Disconnect(const char* buffer, int size);
    void onCNETMsg_SplitScreenUser(const char* buffer, int size);
    void onCNETMsg_Tick(const char* buffer, int size);
    void onCNETMsg_StringCmd(const char* buffer, int size);
    void onCNETMsg_SetConVar(const char* buffer, int size);
    void onCNETMsg_SignonState(const char* buffer, int size);
    void onCNETMsg_SpawnGroup_Load(const char* buffer, int size);
    void onCNETMsg_SpawnGroup_ManifestUpdate(const char* buffer, int size);
    void onCNETMsg_SpawnGroup_SetCreationTick(const char* buffer, int size);
    void onCNETMsg_SpawnGroup_Unload(const char* buffer, int size);
    void onCNETMsg_SpawnGroup_LoadCompleted(const char* buffer, int size);
    void onCSVCMsg_ServerInfo(const char* buffer, int size);
    void onCSVCMsg_FlattenedSerializer(const char* buffer, int size);
    void onCSVCMsg_ClassInfo(const char* buffer, int size);
    void onCSVCMsg_SetPause(const char* buffer, int size);
    void onCSVCMsg_CreateStringTable(const char* buffer, int size);
    void onCSVCMsg_UpdateStringTable(const char* buffer, int size);
    void onCSVCMsg_VoiceInit(const char* buffer, int size);
    void onCSVCMsg_VoiceData(const char* buffer, int size);
    void onCSVCMsg_Print(const char* buffer, int size);
    void onCSVCMsg_Sounds(const char* buffer, int size);
    void onCSVCMsg_SetView(const char* buffer, int size);
    void onCSVCMsg_ClearAllStringTables(const char* buffer, int size);
    void onCSVCMsg_CmdKeyValues(const char* buffer, int size);
    void onCSVCMsg_BSPDecal(const char* buffer, int size);
    void onCSVCMsg_SplitScreen(const char* buffer, int size);
    void onCSVCMsg_PacketEntities(const char* buffer, int size);
    void onCSVCMsg_Prefetch(const char* buffer, int size);
    void onCSVCMsg_Menu(const char* buffer, int size);
    void onCSVCMsg_GetCvarValue(const char* buffer, int size);
    void onCSVCMsg_StopSound(const char* buffer, int size);
    void onCSVCMsg_PeerList(const char* buffer, int size);
    void onCSVCMsg_PacketReliable(const char* buffer, int size);
    void onCSVCMsg_HLTVStatus(const char* buffer, int size);
    void onCSVCMsg_FullFrameSplit(const char* buffer, int size);
    void onCUserMessageAchievementEvent(const char* buffer, int size);
    void onCUserMessageCloseCaption(const char* buffer, int size);
    void onCUserMessageCloseCaptionDirect(const char* buffer, int size);
    void onCUserMessageCurrentTimescale(const char* buffer, int size);
    void onCUserMessageDesiredTimescale(const char* buffer, int size);
    void onCUserMessageFade(const char* buffer, int size);
    void onCUserMessageGameTitle(const char* buffer, int size);
    void onCUserMessageHintText(const char* buffer, int size);
    void onCUserMessageHudMsg(const char* buffer, int size);
    void onCUserMessageHudText(const char* buffer, int size);
    void onCUserMessageKeyHintText(const char* buffer, int size);
    void onCUserMessageColoredText(const char* buffer, int size);
    void onCUserMessageRequestState(const char* buffer, int size);
    void onCUserMessageResetHUD(const char* buffer, int size);
    void onCUserMessageRumble(const char* buffer, int size);
    void onCUserMessageSayText(const char* buffer, int size);
    void onCUserMessageSayText2(const char* buffer, int size);
    void onCUserMessageSayTextChannel(const char* buffer, int size);
    void onCUserMessageShake(const char* buffer, int size);
    void onCUserMessageShakeDir(const char* buffer, int size);
    void onCUserMessageTextMsg(const char* buffer, int size);
    void onCUserMessageScreenTilt(const char* buffer, int size);
    void onCUserMessageTrain(const char* buffer, int size);
    void onCUserMessageVGUIMenu(const char* buffer, int size);
    void onCUserMessageVoiceMask(const char* buffer, int size);
    void onCUserMessageVoiceSubtitle(const char* buffer, int size);
    void onCUserMessageSendAudio(const char* buffer, int size);
    void onCUserMessageItemPickup(const char* buffer, int size);
    void onCUserMessageAmmoDenied(const char* buffer, int size);
    void onCUserMessageCrosshairAngle(const char* buffer, int size);
    void onCUserMessageShowMenu(const char* buffer, int size);
    void onCUserMessageCreditsMsg(const char* buffer, int size);
    void onCUserMessageCloseCaptionPlaceholder(const char* buffer, int size);
    void onCUserMessageCameraTransition(const char* buffer, int size);
    void onCUserMessageAudioParameter(const char* buffer, int size);
    void onCEntityMessagePlayJingle(const char* buffer, int size);
    void onCEntityMessageScreenOverlay(const char* buffer, int size);
    void onCEntityMessageRemoveAllDecals(const char* buffer, int size);
    void onCEntityMessagePropagateForce(const char* buffer, int size);
    void onCEntityMessageDoSpark(const char* buffer, int size);
    void onCEntityMessageFixAngle(const char* buffer, int size);
    void onCMsgVDebugGameSessionIDEvent(const char* buffer, int size);
    void onCMsgPlaceDecalEvent(const char* buffer, int size);
    void onCMsgClearWorldDecalsEvent(const char* buffer, int size);
    void onCMsgClearEntityDecalsEvent(const char* buffer, int size);
    void onCMsgClearDecalsForSkeletonInstanceEvent(const char* buffer, int size);
    void onCMsgSource1LegacyGameEventList(const char* buffer, int size);
    void onCMsgSource1LegacyListenEvents(const char* buffer, int size);
    void onCMsgSource1LegacyGameEvent(const char* buffer, int size);
    void onCMsgSosStartSoundEvent(const char* buffer, int size);
    void onCMsgSosStopSoundEvent(const char* buffer, int size);
    void onCMsgSosSetSoundEventParams(const char* buffer, int size);
    void onCMsgSosSetLibraryStackFields(const char* buffer, int size);
    void onCMsgSosStopSoundEventHash(const char* buffer, int size);
    void onCDOTAUserMsg_AIDebugLine(const char* buffer, int size);
    void onCDOTAUserMsg_ChatEvent(const char* buffer, int size);
    void onCDOTAUserMsg_CombatHeroPositions(const char* buffer, int size);
    void onCDOTAUserMsg_CombatLogShowDeath(const char* buffer, int size);
    void onCDOTAUserMsg_CreateLinearProjectile(const char* buffer, int size);
    void onCDOTAUserMsg_DestroyLinearProjectile(const char* buffer, int size);
    void onCDOTAUserMsg_DodgeTrackingProjectiles(const char* buffer, int size);
    void onCDOTAUserMsg_GlobalLightColor(const char* buffer, int size);
    void onCDOTAUserMsg_GlobalLightDirection(const char* buffer, int size);
    void onCDOTAUserMsg_InvalidCommand(const char* buffer, int size);
    void onCDOTAUserMsg_LocationPing(const char* buffer, int size);
    void onCDOTAUserMsg_MapLine(const char* buffer, int size);
    void onCDOTAUserMsg_MiniKillCamInfo(const char* buffer, int size);
    void onCDOTAUserMsg_MinimapDebugPoint(const char* buffer, int size);
    void onCDOTAUserMsg_MinimapEvent(const char* buffer, int size);
    void onCDOTAUserMsg_NevermoreRequiem(const char* buffer, int size);
    void onCDOTAUserMsg_OverheadEvent(const char* buffer, int size);
    void onCDOTAUserMsg_SetNextAutobuyItem(const char* buffer, int size);
    void onCDOTAUserMsg_SharedCooldown(const char* buffer, int size);
    void onCDOTAUserMsg_SpectatorPlayerClick(const char* buffer, int size);
    void onCDOTAUserMsg_TutorialTipInfo(const char* buffer, int size);
    void onCDOTAUserMsg_UnitEvent(const char* buffer, int size);
    void onCDOTAUserMsg_ParticleManager(const char* buffer, int size);
    void onCDOTAUserMsg_BotChat(const char* buffer, int size);
    void onCDOTAUserMsg_HudError(const char* buffer, int size);
    void onCDOTAUserMsg_ItemPurchased(const char* buffer, int size);
    void onCDOTAUserMsg_Ping(const char* buffer, int size);
    void onCDOTAUserMsg_ItemFound(const char* buffer, int size);
    void onCDOTAUserMsg_SwapVerify(const char* buffer, int size);
    void onCDOTAUserMsg_WorldLine(const char* buffer, int size);
    void onCDOTAUserMsg_ItemAlert(const char* buffer, int size);
    void onCDOTAUserMsg_HalloweenDrops(const char* buffer, int size);
    void onCDOTAUserMsg_ChatWheel(const char* buffer, int size);
    void onCDOTAUserMsg_ReceivedXmasGift(const char* buffer, int size);
    void onCDOTAUserMsg_UpdateSharedContent(const char* buffer, int size);
    void onCDOTAUserMsg_TutorialRequestExp(const char* buffer, int size);
    void onCDOTAUserMsg_TutorialPingMinimap(const char* buffer, int size);
    void onCDOTAUserMsg_GamerulesStateChanged(const char* buffer, int size);
    void onCDOTAUserMsg_ShowSurvey(const char* buffer, int size);
    void onCDOTAUserMsg_TutorialFade(const char* buffer, int size);
    void onCDOTAUserMsg_AddQuestLogEntry(const char* buffer, int size);
    void onCDOTAUserMsg_SendStatPopup(const char* buffer, int size);
    void onCDOTAUserMsg_TutorialFinish(const char* buffer, int size);
    void onCDOTAUserMsg_SendRoshanPopup(const char* buffer, int size);
    void onCDOTAUserMsg_SendGenericToolTip(const char* buffer, int size);
    void onCDOTAUserMsg_SendFinalGold(const char* buffer, int size);
    void onCDOTAUserMsg_CustomMsg(const char* buffer, int size);
    void onCDOTAUserMsg_CoachHUDPing(const char* buffer, int size);
    void onCDOTAUserMsg_ClientLoadGridNav(const char* buffer, int size);
    void onCDOTAUserMsg_TE_Projectile(const char* buffer, int size);
    void onCDOTAUserMsg_TE_ProjectileLoc(const char* buffer, int size);
    void onCDOTAUserMsg_TE_DotaBloodImpact(const char* buffer, int size);
    void onCDOTAUserMsg_TE_UnitAnimation(const char* buffer, int size);
    void onCDOTAUserMsg_TE_UnitAnimationEnd(const char* buffer, int size);
    void onCDOTAUserMsg_AbilityPing(const char* buffer, int size);
    void onCDOTAUserMsg_ShowGenericPopup(const char* buffer, int size);
    void onCDOTAUserMsg_VoteStart(const char* buffer, int size);
    void onCDOTAUserMsg_VoteUpdate(const char* buffer, int size);
    void onCDOTAUserMsg_VoteEnd(const char* buffer, int size);
    void onCDOTAUserMsg_BoosterState(const char* buffer, int size);
    void onCDOTAUserMsg_WillPurchaseAlert(const char* buffer, int size);
    void onCDOTAUserMsg_TutorialMinimapPosition(const char* buffer, int size);
    void onCDOTAUserMsg_PlayerMMR(const char* buffer, int size);
    void onCDOTAUserMsg_AbilitySteal(const char* buffer, int size);
    void onCDOTAUserMsg_CourierKilledAlert(const char* buffer, int size);
    void onCDOTAUserMsg_EnemyItemAlert(const char* buffer, int size);
    void onCDOTAUserMsg_StatsMatchDetails(const char* buffer, int size);
    void onCDOTAUserMsg_MiniTaunt(const char* buffer, int size);
    void onCDOTAUserMsg_BuyBackStateAlert(const char* buffer, int size);
    void onCDOTAUserMsg_SpeechBubble(const char* buffer, int size);
    void onCDOTAUserMsg_CustomHeaderMessage(const char* buffer, int size);
    void onCDOTAUserMsg_QuickBuyAlert(const char* buffer, int size);
    void onCDOTAUserMsg_PredictionResult(const char* buffer, int size);
    void onCDOTAUserMsg_ModifierAlert(const char* buffer, int size);
    void onCDOTAUserMsg_HPManaAlert(const char* buffer, int size);
    void onCDOTAUserMsg_GlyphAlert(const char* buffer, int size);
    void onCDOTAUserMsg_BeastChat(const char* buffer, int size);
    void onCDOTAUserMsg_SpectatorPlayerUnitOrders(const char* buffer, int size);
    void onCDOTAUserMsg_CustomHudElement_Create(const char* buffer, int size);
    void onCDOTAUserMsg_CustomHudElement_Modify(const char* buffer, int size);
    void onCDOTAUserMsg_CustomHudElement_Destroy(const char* buffer, int size);
    void onCDOTAUserMsg_CompendiumState(const char* buffer, int size);
    void onCDOTAUserMsg_ProjectionAbility(const char* buffer, int size);
    void onCDOTAUserMsg_ProjectionEvent(const char* buffer, int size);
};

typedef void (Parser::*Callback)(const char* buffer, int size);

#endif /* _PARSER_TYPES_HPP_ */
