#pragma once

//#include "pthread.h"
#include "cocos2d.h"
#include "BaseNetMsg.h"
//#include "ConvertEndianUtil.h"
#include "thread"
#include "UILoadingIndicator.h"
#include "ProtocolDefine.h"



class Buffer;
typedef std::function<void(struct ProtobufCMessage*, int)> ResponseCallBack;
typedef std::function<void(char*msg)> chatMsgCallBack;
typedef std::function<void(int result)> googleLoginCallBack;
typedef std::pair<void*, ResponseCallBack> ResponseCallBackPair;

#define CLIENT_TIME_OUT 30

#define EXP_NUM_TO_LEVEL(N) (ProtocolThread::GetInstance()->get_exp_level(N))
#define LEVEL_TO_EXP_NUM(N) (ProtocolThread::GetInstance()->get_level_exp(N))
#define FAME_NUM_TO_LEVEL(N) (ProtocolThread::GetInstance()->get_fame_level(N))
#define LEVEL_TO_FAME_NUM(N) (ProtocolThread::GetInstance()->get_level_fame(N))

enum {
	PROTO_TYPE_NETERROR = -1,
	PROTO_TYPE_EmailLogin = 1,
	PROTO_TYPE_Activity = 2,
	PROTO_TYPE_Task = 3,
	PROTO_TYPE_SysMessage = 4,
	PROTO_TYPE_City = 5,
	PROTO_TYPE_LoginFirstData = 6,
	PROTO_TYPE_EmailLoginResult = 7,
	PROTO_TYPE_RegisterUser = 8,

	PROTO_TYPE_ProtoSample				= 9,
	PROTO_TYPE_ProtoSampleResult		= 10,

	//PROTO_TYPE_SailCityFinish = 11,
	PROTO_TYPE_RegisterUserResult = 12,
	PROTO_TYPE_CreateCharacter = 13,
	PROTO_TYPE_CreateCharacterFailed = 14,
	PROTO_TYPE_GetFleetCargo = 15,
	PROTO_TYPE_GetFleetCargoResult = 16,
	PROTO_TYPE_GetGoodsForShip = 17,
	PROTO_TYPE_GetGoodsForShipResult = 18,
	PROTO_TYPE_ExchangeGoodsForShips = 19,
	PROTO_TYPE_ExchangeGoodsForShipsResult = 20,
	PROTO_TYPE_CheckMailBox = 21,
	PROTO_TYPE_CheckMailBoxResult = 22,
	PROTO_TYPE_GetMailList = 23,
	PROTO_TYPE_GetMailListResult = 24,
	PROTO_TYPE_SendMail = 25,
	PROTO_TYPE_SendMailResult = 26,
	PROTO_TYPE_MarkMailStatus = 27,
	PROTO_TYPE_MarkMailStatusResult = 28,

	PROTO_TYPE_GetItemsBeingSold = 29,
	PROTO_TYPE_GetItemsBeingSoldResult = 30,
	PROTO_TYPE_GetLastPrice = 31,
	PROTO_TYPE_GetLastPriceResult = 32,
	PROTO_TYPE_GetOrderList = 33,
	PROTO_TYPE_GetOrderListResult = 34,
	PROTO_TYPE_BuyItem = 35,
	PROTO_TYPE_BuyItemResult = 36,
	PROTO_TYPE_SellItem = 37,
	PROTO_TYPE_SellItemResult = 38,
	PROTO_TYPE_CreateOrder = 39,
	PROTO_TYPE_CreateOrderResult = 40,
	PROTO_TYPE_SearchTradeItems = 41,
	PROTO_TYPE_SearchTradeItemsResult = 42,
	PROTO_TYPE_GetItemsToSell = 43,
	PROTO_TYPE_GetItemsToSellResult = 44,
	PROTO_TYPE_GetPriceData = 45,
	PROTO_TYPE_GetPriceDataResult = 46,
	PROTO_TYPE_GetPersonalItem = 47,
	PROTO_TYPE_GetPersonalItemResult = 48,
	PROTO_TYPE_CancelOrder = 49,
	PROTO_TYPE_CancelOrderResult = 50,
	PROTO_TYPE_WarehouseOpration = 51,
	PROTO_TYPE_WarehouseOprationResult = 52,
	PROTO_TYPE_GetWarehouseItems = 53,
	PROTO_TYPE_GetWarehouseItemsResult = 54,
	PROTO_TYPE_BarConsume = 55,
	PROTO_TYPE_BarConsumeResult = 56,
	PROTO_TYPE_GetBarInfo = 57,
	PROTO_TYPE_GetBarInfoResult = 58,
	PROTO_TYPE_GetCrew = 59,
	PROTO_TYPE_GetCrewResult = 60,
	PROTO_TYPE_TreatBargirl = 61,
	PROTO_TYPE_TreatBargirlResult = 62,
	PROTO_TYPE_GiveBargirlGift = 63,
	PROTO_TYPE_GiveBargirlGiftResult = 64,
	PROTO_TYPE_GetAvailCrewNum = 65,
	PROTO_TYPE_GetAvailCrewNumResult = 66,
	PROTO_TYPE_GetTaskList = 67,
	PROTO_TYPE_GetTaskListResult = 68,
	PROTO_TYPE_GetIntelligence = 69,
	PROTO_TYPE_GetIntelligenceResult = 70,
	PROTO_TYPE_GetCaptain = 71,
	PROTO_TYPE_GetCaptainResult = 72,
	PROTO_TYPE_HandleTask = 73,
	PROTO_TYPE_HandleTaskResult = 74,
	PROTO_TYPE_GetCurrentCityData = 75,
	PROTO_TYPE_GetCurrentCityDataResult = 76,
	PROTO_TYPE_GetShipList = 77,
	PROTO_TYPE_GetShipListResult = 78,
	PROTO_TYPE_RepairShip = 79,
	PROTO_TYPE_RepairShipResult = 80,
	PROTO_TYPE_RepairAllShips = 81,
	PROTO_TYPE_RepairAllShipsResult = 82,
	PROTO_TYPE_FleetFormation = 83,
	PROTO_TYPE_FleetFormationResult = 84,
	PROTO_TYPE_ParkShip = 85,
	PROTO_TYPE_ParkShipResult = 86,
	PROTO_TYPE_GetDrawings = 87,
	PROTO_TYPE_GetDrawingsResult = 88,
	PROTO_TYPE_BuildShip = 89,
	PROTO_TYPE_BuildShipResult = 90,
	PROTO_TYPE_GetBuildingShips = 91,
	PROTO_TYPE_GetBuildingShipsResult = 92,
	PROTO_TYPE_FinishBuilding = 93,
	PROTO_TYPE_FinishBuildingResult = 94,
	PROTO_TYPE_GetBuildingEquipment = 95,
	PROTO_TYPE_GetBuildingEquipmentResult = 96,
	PROTO_TYPE_CancelBuild = 97,
	PROTO_TYPE_CancelBuildResult = 98,
	PROTO_TYPE_GetFleetAndDockShips = 99,
	PROTO_TYPE_GetFleetAndDockShipsResult = 100,
	PROTO_TYPE_GetReinforceShipList = 101,
	PROTO_TYPE_GetReinforceShipListResult = 102,
	PROTO_TYPE_ReinforceShip = 103,
	PROTO_TYPE_ReinforceShipResult = 104,
	PROTO_TYPE_RerollShip = 105,
	PROTO_TYPE_RerollShipResult = 106,
	PROTO_TYPE_GetReinforceEquipList = 107,
	PROTO_TYPE_GetReinforceEquipListResult = 108,
	PROTO_TYPE_ReinforceEquip = 109,
	PROTO_TYPE_ReinforceEquipResult = 110,
	PROTO_TYPE_RerollEquip = 111,
	PROTO_TYPE_RerollEquipResult = 112,
	PROTO_TYPE_EquipShip = 113,
	PROTO_TYPE_EquipShipResult = 114,
	PROTO_TYPE_GetEquipShipInfo = 115,
	PROTO_TYPE_GetEquipShipInfoResult = 116,
	PROTO_TYPE_GetEquipableItems = 117,
	PROTO_TYPE_GetEquipableItemsResult = 118,
	PROTO_TYPE_GetVisitedCities = 119,
	PROTO_TYPE_GetVisitedCitiesResult = 120,
	PROTO_TYPE_AddSupply = 121,
	PROTO_TYPE_AddSupplyResult = 122,
	PROTO_TYPE_GetNextMapBlock = 123,
	PROTO_TYPE_GetNextMapBlockResult = 124,
	PROTO_TYPE_LeaveCity = 125,
	PROTO_TYPE_LeaveCityResult = 126,
	PROTO_TYPE_ReachCity = 127,
	PROTO_TYPE_ReachCityResult = 128,
	PROTO_TYPE_TriggerSeaEvent = 129,
	PROTO_TYPE_TriggerSeaEventResult = 130,
	PROTO_TYPE_FinishSeaEvent = 131,
	PROTO_TYPE_FinishSeaEventResult = 132,
	PROTO_TYPE_GetPalaceInfo = 133,
	PROTO_TYPE_GetPalaceInfoResult = 134,
	PROTO_TYPE_InvestCity = 135,
	PROTO_TYPE_InvestCityResult = 136,
	PROTO_TYPE_GetRankList = 137,
	PROTO_TYPE_GetRankListResult = 138,
	PROTO_TYPE_GetVoteList = 139,
	PROTO_TYPE_GetVoteListResult = 140,
	PROTO_TYPE_VoteMayor = 141,
	PROTO_TYPE_VoteMayorResult = 142,
	PROTO_TYPE_ChangeDevDir = 143,
	PROTO_TYPE_ChangeDevDirResult = 144,
	PROTO_TYPE_GetLabourUnion = 145,
	PROTO_TYPE_GetLabourUnionResult = 146,
	PROTO_TYPE_CreateLabourUnion = 147,
	PROTO_TYPE_CreateLabourUnionResult = 148,
	PROTO_TYPE_DeleteLabourUnion = 149,
	PROTO_TYPE_DeleteLabourUnionResult = 150,
	PROTO_TYPE_InviteLabour = 151,
	PROTO_TYPE_InviteLabourResult = 152,
	PROTO_TYPE_ApplyLabourUnion = 153,
	PROTO_TYPE_ApplyLabourUnionResult = 154,
	PROTO_TYPE_QuitLabourUnion = 155,
	PROTO_TYPE_QuitLabourUnionResult = 156,
	PROTO_TYPE_KickOutMember = 157,
	PROTO_TYPE_KickOutMemberResult = 158,
	PROTO_TYPE_AdjustPrivilege = 159,
	PROTO_TYPE_AdjustPrivilegeResult = 160,
	PROTO_TYPE_DepositMoney = 161,
	PROTO_TYPE_DepositMoneyResult = 162,
	PROTO_TYPE_WithdrawMoney = 163,
	PROTO_TYPE_WithdrawMoneyResult = 164,
	PROTO_TYPE_DepositItem = 165,
	PROTO_TYPE_DepositItemResult = 166,
	PROTO_TYPE_WithdrawItem = 167,
	PROTO_TYPE_WithdrawItemResult = 168,
	PROTO_TYPE_ExpandBank = 169,
	PROTO_TYPE_ExpandBankResult = 170,
	PROTO_TYPE_GetBankInfo = 171,
	PROTO_TYPE_GetBankInfoResult = 172,
	PROTO_TYPE_GetBankLog = 173,
	PROTO_TYPE_GetBankLogResult = 174,
	PROTO_TYPE_SetMainTaskStatus = 175,
	PROTO_TYPE_SetMainTaskStatusResult = 176,
	PROTO_TYPE_EngageInFight = 177,
	PROTO_TYPE_EngageInFightResult = 178,
	PROTO_TYPE_EndFight = 179,
	PROTO_TYPE_EndFightResult = 180,
	PROTO_TYPE_StartAutoFight = 181,
	PROTO_TYPE_StartAutoFightResult = 182,
	PROTO_TYPE_GetSkillsDetails = 183,
	PROTO_TYPE_GetSkillsDetailsResult = 184,
	PROTO_TYPE_AddSkillPoint = 185,
	PROTO_TYPE_AddSkillPointResult = 186,
	PROTO_TYPE_ResetSkillPoints = 187,
	PROTO_TYPE_ResetSkillPointsResult = 188,
	PROTO_TYPE_NotifyBattleLog = 189,
	PROTO_TYPE_NotifyBattleLogResult = 190,
	PROTO_TYPE_GetPersonalInfo = 191,
	PROTO_TYPE_GetPersonalInfoResult = 192,
	PROTO_TYPE_SetBioInfo = 193,
	PROTO_TYPE_SetBioInfoResult = 194,
	PROTO_TYPE_CancelFight = 195,
	PROTO_TYPE_CancelFightResult = 196,
	PROTO_TYPE_GetGuildList = 197,
	PROTO_TYPE_GetGuildListResult = 198,
	PROTO_TYPE_GetMyGuildDetails = 199,
	PROTO_TYPE_GetMyGuildDetailsResult = 200,
	PROTO_TYPE_GetApplyInfo = 201,
	PROTO_TYPE_GetApplyInfoResult = 202,
	PROTO_TYPE_CreateNewGuild = 203,
	PROTO_TYPE_CreateNewGuildResult = 204,
	PROTO_TYPE_JoinGuildRequest = 205,
	PROTO_TYPE_JoinGuildRequestResult = 206,
	PROTO_TYPE_GetGuildDetails = 207,
	PROTO_TYPE_GetGuildDetailsResult = 208,
	PROTO_TYPE_ChangeGuildIntro = 209,
	PROTO_TYPE_ChangeGuildIntroResult = 210,
	PROTO_TYPE_ExpandGuildCapacity = 211,
	PROTO_TYPE_ExpandGuildCapacityResult = 212,
	PROTO_TYPE_GetGuildMemberPermission = 213,
	PROTO_TYPE_GetGuildMemberPermissionResult = 214,
	PROTO_TYPE_SetGuildMemberPermission = 215,
	PROTO_TYPE_SetGuildMemberPermissionResult = 216,
	PROTO_TYPE_GetHiredCaptains = 217,
	PROTO_TYPE_GetHiredCaptainsResult = 218,
	PROTO_TYPE_DealWithGuildJoin = 219,
	PROTO_TYPE_DealWithGuildJoinResult = 220,
	PROTO_TYPE_ChangeGuildAnnouncement = 221,
	PROTO_TYPE_ChangeGuildAnnouncementResult = 222,
	PROTO_TYPE_RemoveGuildMember = 223,
	PROTO_TYPE_RemoveGuildMemberResult = 224,
	PROTO_TYPE_ExitFromGuild = 225,
	PROTO_TYPE_ExitFromGuildResult = 226,
	PROTO_TYPE_DismissGuild = 227,
	PROTO_TYPE_DismissGuildResult = 228,
	PROTO_TYPE_GetSearchGuildList = 229,
	PROTO_TYPE_GetSearchGuildListResult = 230,
	PROTO_TYPE_RefreshCaptainList = 231,
	PROTO_TYPE_RefreshCaptainListResult = 232,
	PROTO_TYPE_GetFriendsList = 233,
	PROTO_TYPE_GetFriendsListResult = 234,
	PROTO_TYPE_GetBlackList = 235,
	PROTO_TYPE_GetBlackListResult = 236,
	PROTO_TYPE_FriendsOperation = 237,
	PROTO_TYPE_FriendsOperationResult = 238,
	PROTO_TYPE_GetFriendsReqList = 239,
	PROTO_TYPE_GetFriendsReqListResult = 240,
	PROTO_TYPE_FireCaptain = 241,
	PROTO_TYPE_FireCaptainResult = 242,
	PROTO_TYPE_DestoryItems = 243,
	PROTO_TYPE_DestoryItemsResult = 244,
	PROTO_TYPE_SearchUserByName = 245,
	PROTO_TYPE_SearchUserByNameResult = 246,
	PROTO_TYPE_TakeAllAttachment = 247,
	PROTO_TYPE_TakeAllAttachmentResult = 248,
	PROTO_TYPE_GetItemsDetailInfo = 249,
	PROTO_TYPE_GetItemsDetailInfoResult = 250,
	PROTO_TYPE_GetDevGoodsInfo = 251,
	PROTO_TYPE_GetDevGoodsInfoResult = 252,
	PROTO_TYPE_ProvideDevGoods = 253,
	PROTO_TYPE_ProvideDevGoodsResult = 254,
	PROTO_TYPE_GetDevGoodsCount = 255,
	PROTO_TYPE_GetDevGoodsCountResult = 256,
	PROTO_TYPE_GetFriendValue = 257,
	PROTO_TYPE_GetFriendValueResult = 258,
	PROTO_TYPE_GetUserInfoById = 259,
	PROTO_TYPE_GetUserInfoByIdResult = 260,
	PROTO_TYPE_TestCreateNewGuild = 261,
	PROTO_TYPE_TestCreateNewGuildResult = 262,
	PROTO_TYPE_InviteUserToGuild = 263,
	PROTO_TYPE_InviteUserToGuildResult = 264,
	PROTO_TYPE_GetInvitationList = 265,
	PROTO_TYPE_GetInvitationListResult = 266,
	PROTO_TYPE_DealWithInvitation = 267,
	PROTO_TYPE_DealWithInvitationResult = 268,
	PROTO_TYPE_GetVTicketMarketItems = 269,
	PROTO_TYPE_GetVTicketMarketItemsResult = 270,
	PROTO_TYPE_BuyVTicketMarketItem = 271,
	PROTO_TYPE_BuyVTicketMarketItemResult = 272,
	PROTO_TYPE_GetCurrentInvestData = 273,
	PROTO_TYPE_GetCurrentInvestDataResult = 274,
	PROTO_TYPE_GetUserTasks = 275,
	PROTO_TYPE_GetUserTasksResult = 276,
	PROTO_TYPE_UseSudanMask = 277,
	PROTO_TYPE_UseSudanMaskResult = 278,
	PROTO_TYPE_EquipHero = 279,
	PROTO_TYPE_EquipHeroResult = 280,
	PROTO_TYPE_GetHeroEquip = 281,
	PROTO_TYPE_GetHeroEquipResult = 282,
	PROTO_TYPE_BuyCityLicense = 283,
	PROTO_TYPE_BuyCityLicenseResult = 284,
	PROTO_TYPE_DeleteUser = 285,
	PROTO_TYPE_DeleteUserResult = 286,
	PROTO_TYPE_ChangePassword = 287,
	PROTO_TYPE_ChangePasswordResult = 288,
	PROTO_TYPE_SelectHeroPositiveSkill = 289,
	PROTO_TYPE_SelectHeroPositiveSkillResult = 290,
	PROTO_TYPE_GetHeroPositiveSkillList = 291,
	PROTO_TYPE_GetHeroPositiveSkillListResult = 292,
	PROTO_TYPE_GetSelectedPositiveSkills = 293,
	PROTO_TYPE_GetSelectedPositiveSkillsResult = 294,
	PROTO_TYPE_GetSailInfo = 295,
	PROTO_TYPE_GetSailInfoResult = 296,
	PROTO_TYPE_InsertUserName = 297,
	PROTO_TYPE_InsertUserNameResult = 298,
	PROTO_TYPE_GetLeaderboard = 299,
	PROTO_TYPE_GetLeaderboardResult = 300,
	PROTO_TYPE_GetCityStatus = 301,
	PROTO_TYPE_GetCityStatusResult = 302,
	PROTO_TYPE_GetDiscoveredCities = 303,
	PROTO_TYPE_GetDiscoveredCitiesResult = 304,
	PROTO_TYPE_AddDiscoveredCity = 305,
	PROTO_TYPE_AddDiscoveredCityResult = 306,
	PROTO_TYPE_ForceFightStatus = 307,
	PROTO_TYPE_ForceFightStatusResult = 308,
	PROTO_TYPE_GetCityProduces = 309,
	PROTO_TYPE_GetCityProducesResult = 310,
	PROTO_TYPE_GetCityDemands = 311,
	PROTO_TYPE_GetCityDemandsResult = 312,
	PROTO_TYPE_GetMainTask = 313,
	PROTO_TYPE_GetMainTaskResult = 314,
	PROTO_TYPE_CompleteMainTask = 315,
	PROTO_TYPE_CompleteMainTaskResult = 316,
	PROTO_TYPE_GetMainTaskReward = 317,
	PROTO_TYPE_GetMainTaskRewardResult = 318,
	PROTO_TYPE_GetEventTaskList = 319,
	PROTO_TYPE_GetEventTaskListResult = 320,
	PROTO_TYPE_HandleEventTask = 321,
	PROTO_TYPE_HandleEventTaskResult = 322,
	PROTO_TYPE_GetNpcScore = 323,
	PROTO_TYPE_GetNpcScoreResult = 324,
	PROTO_TYPE_GetServerInfo = 325,
	PROTO_TYPE_GetServerInfoResult = 326,
	PROTO_TYPE_PingServer = 327,
	PROTO_TYPE_PingServerResult = 328,
	PROTO_TYPE_ExpandPackageSize = 329,
	PROTO_TYPE_ExpandPackageSizeResult = 330,
	PROTO_TYPE_GetPretaskItems = 331,
	PROTO_TYPE_GetPretaskItemsResult = 332,
	PROTO_TYPE_GetOnsaleItems = 333,
	PROTO_TYPE_GetOnsaleItemsResult = 334,
	PROTO_TYPE_BuyOnsaleItems = 335,
	PROTO_TYPE_BuyOnsaleItemsResult = 336,
	PROTO_TYPE_SendCustomServiceInfo = 337,
	PROTO_TYPE_SendCustomServiceInfoResult = 338,
	PROTO_TYPE_GetIABItem = 339,
	PROTO_TYPE_GetIABItemResult = 340,
	PROTO_TYPE_ChangeEmail = 341,
	PROTO_TYPE_ChangeEmailResult = 342,
	PROTO_TYPE_TokenLogin = 343,
	PROTO_TYPE_EngageInFightForTask = 344,
	PROTO_TYPE_EngageInFightForTaskResult = 345,
	PROTO_TYPE_AuthenticateAccount = 346,
	PROTO_TYPE_AuthenticateAccountResult = 347,
	PROTO_TYPE_Salvage = 348,
	PROTO_TYPE_SalvageResult = 349,
	PROTO_TYPE_GetEmailVerifiedReward = 350,
	PROTO_TYPE_GetEmailVerifiedRewardResult = 351,
	PROTO_TYPE_StartSalvage = 352,
	PROTO_TYPE_StartSalvageResult = 353,
	PROTO_TYPE_GetEmailVerifiedInfo = 354,
	PROTO_TYPE_GetEmailVerifiedInfoResult = 355,
	PROTO_TYPE_GetSalvageCondition = 356,
	PROTO_TYPE_GetSalvageConditionResult = 357,
	PROTO_TYPE_GetActivitiesProps = 358,
	PROTO_TYPE_GetActivitiesPropsResult = 359,
	PROTO_TYPE_GetActivitiesGiftInfo = 360,
	PROTO_TYPE_GetActivitiesGiftInfoResult = 361,
	PROTO_TYPE_GetActivitiesPrayInfo = 362,
	PROTO_TYPE_GetActivitiesPrayInfoResult = 363,
	PROTO_TYPE_GetPray = 364,
	PROTO_TYPE_GetPrayResult = 365,
	PROTO_TYPE_SendActivitiesGift = 366,
	PROTO_TYPE_SendActivitiesGiftResult = 367,
	PROTO_TYPE_GetPirateAttackInfo = 368,
	PROTO_TYPE_GetPirateAttackInfoResult = 369,
	PROTO_TYPE_GetAttackPirateInfo = 370,
	PROTO_TYPE_GetAttackPirateInfoResult = 371,
	PROTO_TYPE_GetVTicketMarketActivities = 372,
	PROTO_TYPE_GetVTicketMarketActivitiesResult = 373,
	PROTO_TYPE_BuyVTicketMarketActivities = 374,
	PROTO_TYPE_BuyVTicketMarketActivitiesResult = 375,
	PROTO_TYPE_FinalMyExploitScore = 376,
	PROTO_TYPE_FinalMyExploitScoreResult = 377,
	PROTO_TYPE_FindLootPlayer = 378,
	PROTO_TYPE_FindLootPlayerResult = 379,
	PROTO_TYPE_LootPlayer = 380,
	PROTO_TYPE_LootPlayerResult = 381,
	PROTO_TYPE_GetLootPlayerLog = 382,
	PROTO_TYPE_GetLootPlayerLogResult = 383,
	PROTO_TYPE_StartRevengeOnLootPlayer = 384,
	PROTO_TYPE_GetAttackPirateRankInfo = 385,
	PROTO_TYPE_GetAttackPirateRankInfoResult = 386,
	PROTO_TYPE_GetAttackPirateBossInfo = 387,
	PROTO_TYPE_GetAttackPirateBossInfoResult = 388,
	PROTO_TYPE_BuyInsurance = 389,
	PROTO_TYPE_BuyInsuranceResult = 390,
	PROTO_TYPE_CompleteDialogs = 391,
	PROTO_TYPE_CompleteDialogsResult = 392,
	PROTO_TYPE_ObtainLottoMessage = 393,
	PROTO_TYPE_ObtainLottoMessageResult = 394,
	PROTO_TYPE_GetInviteBonus = 395,
	PROTO_TYPE_GetInviteBonusResult = 396,
	PROTO_TYPE_GetCompanies = 397,
	PROTO_TYPE_GetCompaniesResult = 398,
	PROTO_TYPE_EquipCompany = 399,
	PROTO_TYPE_EquipCompanyResult = 400,
	PROTO_TYPE_UnlockCompany = 401,
	PROTO_TYPE_UnlockCompanyResult = 402,
	PROTO_TYPE_GetCompanionsStatus = 403,
	PROTO_TYPE_GetCompanionsStatusResult = 404,
	PROTO_TYPE_UnlockShipCabin = 405,
	PROTO_TYPE_UnlockShipCabinResult = 406,
	PROTO_TYPE_GetShipCompanions = 407,
	PROTO_TYPE_GetShipCompanionsResult = 408,
	PROTO_TYPE_SetShipCompanions = 409,
	PROTO_TYPE_SetShipCompanionsResult = 410,
	PROTO_TYPE_SaveFailedFight = 411,
	PROTO_TYPE_SaveFailedFightResult = 412,
	PROTO_TYPE_EndFailedFightByVTicket = 413,
	PROTO_TYPE_EndFailedFightByVTicketResult = 414,
	PROTO_TYPE_CompleteCompanionTask = 415,
	PROTO_TYPE_CompleteCompanionTaskResult = 416,
	PROTO_TYPE_DeductCoins = 417,
	PROTO_TYPE_DeductCoinsResult = 418,
	PROTO_TYPE_UpdateDataVersion = 419,
	PROTO_TYPE_UpdateDataVersionResult = 420,
	PROTO_TYPE_CalFastHireCrew = 421,
	PROTO_TYPE_CalFastHireCrewResult = 422,
	PROTO_TYPE_CompanionRebirth = 423,
	PROTO_TYPE_CompanionRebirthResult = 424,
	PROTO_TYPE_RepairEquipment = 425,
	PROTO_TYPE_RepairEquipmentResult = 426,
	PROTO_TYPE_GetRepairEquipmentNeed = 427,
	PROTO_TYPE_GetRepairEquipmentNeedResult = 428,
	PROTO_TYPE_GetRecentOrder = 429,
	PROTO_TYPE_GetRecentOrderResult = 430,
	PROTO_TYPE_GetDropIncreaseRateInfo = 431,
	PROTO_TYPE_GetDropIncreaseRateInfoResult = 432,
	PROTO_TYPE_GetDailyActivitiesReward = 433,
	PROTO_TYPE_GetDailyActivitiesRewardResult = 434,
	PROTO_TYPE_AddProficiency = 435,
	PROTO_TYPE_AddProficiencyResult = 436,
	PROTO_TYPE_GetProficiencyValues = 437,
	PROTO_TYPE_GetProficiencyValuesResult = 438,

		PROTO_TYPE_RepairCityDefenseInWar = 439,
		PROTO_TYPE_RepairCityDefenseInWarResult = 440,
		PROTO_TYPE_EnhanceAttack = 441,
		PROTO_TYPE_EnhanceAttackResult = 442,
		PROTO_TYPE_GetCityPrepareStatus = 443,
		PROTO_TYPE_GetCityPrepareStatusResult = 444,
		PROTO_TYPE_StartAttackCity = 445,
		PROTO_TYPE_StartAttackCityResult = 446,
		PROTO_TYPE_GetEnemyNation = 447,
		PROTO_TYPE_GetEnemyNationResult = 448,
		PROTO_TYPE_AddCityRepairPool = 449,
		PROTO_TYPE_AddCityRepairPoolResult = 450,
		PROTO_TYPE_AddCityAttackPool = 451,
		PROTO_TYPE_AddCityAttackPoolResult = 452,
		PROTO_TYPE_GetPackageInfo = 453,
		PROTO_TYPE_GetPackageInfoResult = 454,
		PROTO_TYPE_GetProficiencyBook = 455,
		PROTO_TYPE_GetProficiencyBookResult = 456,
		PROTO_TYPE_ApplyStateWar = 457,
		PROTO_TYPE_ApplyStateWarResult = 458,
		PROTO_TYPE_GetContributionRank = 459,
		PROTO_TYPE_GetContributionRankResult = 460,
		PROTO_TYPE_AddSupplyStation = 461,
		PROTO_TYPE_AddSupplyStationResult = 462,
		PROTO_TYPE_HireArmy = 463,
		PROTO_TYPE_HireArmyResult = 464,
		PROTO_TYPE_GetNationWarEntrance = 465,
		PROTO_TYPE_GetNationWarEntranceResult = 466,
		PROTO_TYPE_EndAttackCity = 467,
		PROTO_TYPE_EndAttackCityResult = 468,
		PROTO_TYPE_EnterDepotRepair = 469,
		PROTO_TYPE_EnterDepotRepairResult = 470,
		PROTO_TYPE_GetNationWarCityLost = 471,
		PROTO_TYPE_GetNationWarCityLostResult = 472,
		PROTO_TYPE_NationWarBattleTurnResult = 473,
		PROTO_TYPE_ReachDepot = 474,
		PROTO_TYPE_ReachDepotResult = 475,
		PROTO_TYPE_ReduceInNationWar = 476,
		PROTO_TYPE_ReduceInNationWarResult = 477,
		PROTO_TYPE_GetMyNationWar = 478,
		PROTO_TYPE_GetMyNationWarResult = 479,
		PROTO_TYPE_FuncForTest = 480,
		PROTO_TYPE_FuncForTestResult = 481,
		PROTO_TYPE_CheckDeleteCharactersNum = 482,
		PROTO_TYPE_CheckDeleteCharactersNumResult = 483,
		PROTO_TYPE_GetPlayerEquipShipInfo = 484,
		PROTO_TYPE_GetPlayerEquipShipInfoResult = 485,
		PROTO_TYPE_GetPlayerShipList = 486,
		PROTO_TYPE_GetPlayerShipListResult = 487,
		PROTO_TYPE_FireCrew = 488,
		PROTO_TYPE_FireCrewResult = 489,
		PROTO_TYPE_GetForceCity = 490,
		PROTO_TYPE_GetForceCityResult = 491,
		PROTO_TYPE_SaveItemsToWarehouse = 492,
		PROTO_TYPE_SaveItemsToWarehouseResult = 493,
		PROTO_TYPE_ExpandWarehouse = 494,
		PROTO_TYPE_ExpandWarehouseResult = 495,
		PROTO_TYPE_GetChangeLog = 496,
		PROTO_TYPE_GetChangeLogResult = 497,
};

enum inbox_mail_attachment_type {
	INBOX_ATTACH_TYPE_ITEM = 0, INBOX_ATTACH_TYPE_GOODS = 1,
};

class UserSession;

class ProtocolThread {
public:
	~ProtocolThread(void);
	static ProtocolThread* GetInstance();
	static int get_exp_level(long exp);
	static int get_fame_level(long fame);
	static int64_t get_level_exp(int level);
	static int64_t get_level_fame(int level);

	void processSocketMessage(void * = NULL);
	int getSeaAreaIndex(cocos2d::Point cord);
	//海域区分
	int getSeaIndex(int seaIndex = 0);
	void joinGuildChannel(int guildid);
	void joinCityChannel(int cityId);
	void joinSeaChannel(int seaIdx);
	bool sendChat(int channelType, const char*msg); //1,world,2,country,3 city or sea area,4 guild
	void listChannelUsers(int channelType); //1,world,2,country,3 city or sea area,4 guild
	void getPrivateUserInfo(char*name);
	bool sendPrivateChat(const char* toName, const char*msg);
	void sendChatMessage(const char*p);
	void connectToChatServer();
	bool isConnectingChatServer();
	bool isChatServerConnected();
	void loginChatServer();
	void processChatMessage();
	void broadcastConnectionFailed();
	void broadcastConnectionSucceeded();
	void broadcastConnectionDisconnected();
	void connectionStatusChanged(bool hasNetwork);
	void sleep(int tesec);
	void registerMessageCallback(const ResponseCallBack &callback, void*_this);
	void registerChatMessageCallback(const chatMsgCallBack &callback);
	void registerGoogleLoginCallback(const googleLoginCallBack &callback);
	void unregisterChatMessageCallback();

	void unregisterMessageCallback(void*_this);

	void addBufferToSendingQueue(Buffer*buffer);
	void sendQueuedBuffer();
	void clearQueueMsg();
	void connectServer();
	void reset();
	void reconnectServer();
	bool getConnectionStatus();
	void savePassword(const char*password);
	void saveEmail(const char*email);

	void savePendingPassword(char*password) {
		pendingPassword = password;
	}
	void savePendigEmail(char*email) {
		pendingEmail = email;
	}
	void notifyGamePaused(bool value) {
		gamePaused = value;
	}
	std::string getSavedPassword();
	std::string getSavedEmail();
	std::string getDefaultUserName();
	void init();
	void setServerUrl(std::string& url,int port, std::string chatUrl,int chatPort);
//***************************************************************************API START*************************************************************************************
	//***************************************************************************API START*************************************************************************************
	/*
	 * userEmailLogin : 用户登陆接口. email : 用户登陆帐号, passwd : 用户登陆密码, networkType : 玩家使用网络类型
	 * EmailLoginResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	lastCityId : 最近一次登陆城市Id
	 * 	shipx : 船只位置x坐标
	 * 	shipy : 船只位置y坐标
	 * 	needCreateCharacter : 帐号是否有创建角色,值为1没有创建角色
	 * 	dailyRewardData : 每日登陆奖励信息
	 * 	totalContinuousDays : 连续登陆天数
	 * 	mainTaskStatus : 玩家新手教程完成标志,值为10代表已经完成
	 * 	serverVersion : 服务器版本号
	 *		guildId : 玩家所在工会Id
	 *		heroName : 角色名称
	 *		exp : 玩家经验值
	 *		cid ： 玩家Id
	 *		gold : V票数量
	 *		coin : 银币数量
	 *		prestige : 玩家声望值
	 *		packageSize : 初始可以存放物品的最大重量(包括装备和道具)
	 *		bankSize : 初始银行存放物品格子数量
	 *		nation : 所属国家
	 *		gender : 性别,值为1男性,值为2女性
	 *		iconIdx : 头像id
	 */
	void userEmailLogin(const char*email, const char*passwd, UILoadingIndicator*loading = 0);
	/*
	 * registerUser : 注册用户借口. email : 注册帐号, networkType : 玩家使用网络类型
	 * RegisterUserResult : 返回结果
	 * 	reason : 0成功,非0失败
	 * 	serverVersion : 服务器版本号
	 */
	void registerUser(const char*email,const char*password, UILoadingIndicator*loading = 0);
	/*
	 * createCharacter : 创建玩家人物接口. nation : 国家
	 * CreateCharacterFailed : 返回结果
	 * 	reason : 0成功,非0失败
	 * 	int companionType  //1,战斗女，2 辅助女，3 战斗男，4，辅助男
	 */
	void createCharacter(int companionType,int nation, UILoadingIndicator*loading = 0);
	/*
	 * insertUserName : 插入人物基本信息到数据库,为玩家没有完成注册而下线使用. name : 玩家姓名, gender : 性别,值为1男性,值为2女性, iconIdx : 头像id
	 * InsertUserNameResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void insertUserName(const char*name, int gender, int iconIdx, UILoadingIndicator*loading = 0);
	/*
	 * getFleetCargo : 获取船上货物信息接口
	 * GetFleetCargoResult : 返回结果
	 *		failed : 0成功,非0失败
	 *		ships : ShipDefine类型,玩家船只信息
	 *		shipGoods : ShipGoods类型, 玩家货物信息
	 */
	void getFleetCargo(UILoadingIndicator*loading = 0);
	/*
	 * getGoodsForShip, exchangeGoodsForShips : 废弃的接口
	 */
	void getGoodsForShip(int shipId, UILoadingIndicator*loading = 0);
	void exchangeGoodsForShips(int firstShipId, int secendShipId, int*firstShipItemIds, int*firstShipItemAmounts, int*secondShipItemIds, int*secondShipItemAmounts, int firstCount, int sencondCount,
			UILoadingIndicator*loading = 0);
	/*
	 * getMailList : 获取邮件内容接口
	 * GetMailListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	inbox : MailDefine类型,非系统邮件
	 * 	outbox : 没有使用
	 * 	system : MailDefine类型,系统邮件
	 * 	coceral : 没有使用
	 * 	dustbin : 没有使用
	 */
	void getMailList(UILoadingIndicator*loading = 0);
	/*
	 * checkMailBox : 查询是否有新邮件的接口
	 * CheckMailBoxResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	newSkillPoints : 可用技能点数量
	 * 	newFriendsReqs : 好友申请信件数量
	 * 	newGuildReqs : 加入工会请求信件数量
	 * 	taskCompleted : 接受的任务完成或者过期
	 * 	newMailCount : 邮件数量
	 */
	void checkMailBox();
	/*sendMail : 发送邮件接口. receiverName : 接受者姓名, title : 邮件题目, body : 邮件内容, attachid : 附件id, attachamount : 附件数量, attachtype : 附件类型, goldnum : V票数(现在不能使用), coinnum : 银币数量
	 * SendMailResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	mail : 成功返回MailDefine类型, 暂时只有id有效, 失败返回0
	 * 	userCoins : 成功返回银币数量, 失败返回0
	 * 	userGolds : 成功返回V票数量, 失败返回0
	 * 	fame : 成功返回声望大小, 失败返回0
	 * 	exp : 成功返回经验值, 失败返回0
	 */
	void sendMail(char *receiverName, char *title, char *body, int32_t attachid = 0, int32_t attachamount = 0, int32_t attachtype = INBOX_ATTACH_TYPE_ITEM, int32_t goldnum = 0, int64_t coinnum = 0,
			UILoadingIndicator*loading = 0);
	/*
	 * markMailStatus : 邮件操作后的标记接口. mailIds : 邮件id, action : 操作方式(1,mark as read. 2, move to recycle bin. 3 ,delete permanently. 4,restore from recycle bin. 5, fetch attachment.)
	 * MarkMailStatusResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	action : 操作方式(markMailStatus->action)
	 * 	userCoins : 成功返回银币数量, 失败返回0
	 * 	userGolds : 成功返回V票数量, 失败返回0
	 * 	fame : 成功返回声望大小, 失败返回0
	 * 	exp : 成功返回经验值, 失败返回0
	 */
	void markMailStatus(int* mailIds, int n_mails, int action, UILoadingIndicator*loading = 0);
	/*
	 * moveMailToDustbin, deleteMail, restoreMailFromDustbin, markMailAsRead, fetchAttachmentFromMail : 废弃的接口
	 */
	void moveMailToDustbin(int* mailIds, int n_mails, UILoadingIndicator*loading = 0);
	void deleteMail(int* mailIds, int n_mails, UILoadingIndicator*loading = 0);
	void restoreMailFromDustbin(int* mailIds, int n_mails, UILoadingIndicator*loading = 0);
	void markMailAsRead(int* mailIds, int n_mails, UILoadingIndicator*loading = 0);
	void fetchAttachmentFromMail(int* mailIds, int n_mails, UILoadingIndicator*loading = 0);
	/*
	 * getItemsBeingSold : 获取市场中可以买的物品. itemType : 物品类型
	 * GetItemsBeingSoldResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	itemType : 物品类型
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	items : BeingSoldItemsDefine类型, 物品信息
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void getItemsBeingSold(int itemType, UILoadingIndicator*loading = 0);
	/*
	 * getItemsToSell : 创建订单或向银行存储物品时调用, 获取物品信息. itemType : 物品类型
	 * GetItemsToSellResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	itemType : 物品类型
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	items : ToSellItemsDefine类型, 物品信息
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void getItemsToSell(int itemType, int forTrade = 0,UILoadingIndicator*loading = 0);
	/*
	 * getPersonalItems : 获取可以卖出的物品. itemType : 物品类型
	 * GetPersonalItemResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	itemType : 物品类型
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	goods : HatchItemsDefine类型, 货物信息
	 * 	ships : ShipDefine类型, 船只信息
	 * 	equipments : EquipmentDefine类型, 装备信息
	 * 	curPackageSize : 当前存放物品的重量(包括装备和道具)
	 * 	maxPackageSize : 可以存放物品的最大重量(包括装备和道具)
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	curGoodsWeight : 当前存放货物的重量
	 * 	maxGoodsWeight : 可以存放货物的最大重量
	 * 	drawings : DrawingItemsDefine类型, 图纸信息
	 * 	specials : SpecialItemsDefine类型, V票信息
	 */
	void getPersonalItems(int itemType, int isTrade, UILoadingIndicator*loading = 0);
	/*
	 * getLastPrice : 废弃的接口
	 */
	void getLastPrice(int itemType, int*ids, int n_ids, UILoadingIndicator*loading = 0);
	/*
	 * getOrderList : 获取订单信息接口, isSell : 订单类型(值为1是卖订单, 值为0是买订单)
	 * GetOrderListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	maxOrderNum : 最多可以创建订单的数量(买或卖)
	 * 	fee : 创建订单的花费
	 * 	tax : 创建订单的税率
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	orders : OrderDefine类型, 订单信息
	 * 	skills : SkillDefine类型, 订单相关技能信息(减少税收)
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	currentOrderNum2 : 买订单数量(isSell值为1)或者卖订单数量(isSell值为0)
	 */
	void getOrderList(int isSell, UILoadingIndicator*loading = 0);
	/*
	 * buyItem : 交易所购买物品. itemType : 物品类型, itemId : 物品iid, averagePrice : 平均价格, shipIds : 船id数组, counts : 购买数量,
	 * 											 shipNum : 船数量, optionalNum : 一般都为0, characterItemId : 一般都为0
	 * BuyItemResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	itemType : 物品类型
	 * 	itemId : 物品iid
	 * 	count : 购买数量
	 * 	totalPrice : 总价格
	 * 	userCoins : 银币数量
	 * 	userGolds : V票数量
	 * 	characterItemId : 一般都为0
	 * 	optionalNum : 一般都为0
	 * 	resAveragePrice : 平均价格
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	taxAmount : 花费税的价格
	 */
	void buyItem(int itemType, int itemId, float averagePrice, int* shipIds, int* counts, int shipNum, int optionalNum, int characterItemId, UILoadingIndicator*loading = 0);
	/*
	 * sellItem : 交易所卖出物品. itemType : 物品类型, itemId : 物品iid, count : 卖出数量, optionalNum : 非装备类型值为0, 装备类型值为强化数(一般都为0), characterItemId
	 * 											 characterItemId : 非装类型备值为0, 装备类型值为装备的id(一般都为0), averagePrice : 平均价格, npcOnly : 只卖给npc
	 *	SellItemResult : 返回结果
	 *		failed : 0成功,非0失败
	 * 	itemType : 物品类型
	 * 	itemId : 物品iid
	 * 	count : 已经卖出数量
	 * 	totalPrice : 卖出总价格
	 * 	userCoins : 银币数量
	 * 	userGolds : V票数量
	 * 	earnings : 利润
	 * 	characterItemId : 一般为0
	 * 	optioinalNum : 一般为0
	 * 	resAveragePrice : 平均价格
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 *		reqCount : 希望卖出物品的数量
	 *		newLevel : 玩家等级
	 *		taxAmount : 花费税的价格
	 *		expAdded : 增加的经验值
	 *		fameAdded : 增加的声望值
	 *		captains : CaptainInfo类型, 船长信息
	 *		new_fame_level : 玩家声望值
	 */
	void sellItem(int itemType, int itemId, int count, int optionalNum, int characterItemId, float averagePrice, int npcOnly = 0, UILoadingIndicator*loading = 0);
	/*
	 * createOrder : 交易所创建订单. itemType : 物品类型, itemId : 物品iid, count : 订单数量, maxPrice : 订单价格, expDays : 挂单天数, optionalNum : 非装备类型值为0, 装备类型值为强化数
	 * 	                                                characterItemId : 非装类型备值为0, 装备类型值为装备的id, currency : 废弃 , isbid : 订单类型, 值为1创建买订单,值为0创建卖订单
	 * CreateOrderResult :返回结果
	 *		failed : 0成功,非0失败
	 *		orderId : 订单id(一般是一个redis hash key)
	 */
	void createOrder(int itemType, int itemId, int count, int maxPrice, int expDays, int optionalNum, int characterItemId, int currency = 0, int isbid = 0, UILoadingIndicator*loading = 0);
	/*
	 * searchTradeItems : 废弃
	 */
	void searchTradeItems(int*types, int*ids, int n, UILoadingIndicator*loading = 0);
	/*
	 * getPriceData : 交易所中获取物品买或卖的价格信息. itemType : 物品类型, itemId : 物品iid, characterItemId : 非装类型备值为0, 装备类型值为装备的id
	 * 																					  count : 数量, isSell : 交易类型(值为1卖, 值为0买)
	 * GetPriceDataResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	itemType : 物品类型
	 * 	itemId : 物品iid
	 * 	count : 可交易数量
	 * 	tax : 税率
	 */
	void getPriceData(int itemType, int itemId, int characterItemId, int count, int isSell = 0, UILoadingIndicator*loading = 0);
	/*
	 * cancelOrder : 交易所取消订单. itemType : 物品类型, itemId : 物品iid, orderId : 订单id
	 * CancelOrderResult : 返回信息
	 * 	failed : 0成功,非0失败
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void cancelOrder(int itemType, int itemId, char*orderId, UILoadingIndicator*loading = 0);
	/*
	 * getWarehouseItems : 获取仓库中的物品
	 * GetWarehouseItemsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	curCityGoods : CityGoods类型, 仓库中当前城市的物品信息
	 * 	otherGoods : CityGoods类型, 仓库中其他城市的物品信息
	 */
	void getWarehouseItems(UILoadingIndicator*loading = 0);
	// 	void fetchItemFromWarehouse(int itemId,int itemType,LoadingLayer*loading=0);
	// 	void deleteItemFromWarehouse(int itemId,int itemType,LoadingLayer*loading=0);
	/*
	 * fetchAllFromWarehouse, fetchItemFromWarehouse, deleteItemFromWarehouse : 废弃的接口
	 */
	void fetchAllFromWarehouse(UILoadingIndicator*loading = 0);
	void fetchItemFromWarehouse(int id, UILoadingIndicator*loading = 0);
	void deleteItemFromWarehouse(int id, long count = 0, UILoadingIndicator*loading = 0);
	/*
	 * getBarInfo : 获取酒吧信息.
	 * GetBarInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	foodMenu : BarMenuDefine数组, 食物菜单
	 * 	drinkMenu : BarMenuDefine数组, 饮品菜单
	 * 	barCaptain : BarCaptainDefine数组, 可雇佣船长信息
	 * 	barTask : TaskDefine类型, 正在执行的任务
	 * 	gold : v票数量
	 * 	coin : 银币数量
	 * 	treated : 曾经在酒吧消费过的标志
	 * 	hasTask : 当前是否有正在执行的任务
	 * 	myCaptainNum : BarCaptainDefine数组, 已经使用的船长信息
	 * 	maxCaptainNum : 最多可雇佣船长数
	 * 	refreshInterval : 船长刷新时间间隔
	 * 	lastRefreshTime : 可雇佣船长上次刷新的时间
	 * 	employed : 没有使用
	 * 	myCaptain : BarCaptainDefine数组
	 *		fame : 声望值
	 *		exp : 经验值
	 *		barGirlPrestige : 酒吧女郎友好度值
	 *		taskIds : 可接受任务的id
	 *		taskRefreshInterval : 任务刷新时间间隔
	 *		taskNextRefreshTime : 任务下次刷新的时间
	 *		bargirlDrinkCost : 请酒吧女郎喝酒花费银币数
	 *		treatForGetCrewCost : 宴请招募花费银币数
	 *		refreshCaptainCost : 刷新可招募船长花费v票数量
	 *		refreshTaskCost : 刷新任务花费v票数
	 *		hireCrewFee : 解雇水手花费银币(可能没有使用)
	 *		barGirlPrestigeRequired : 获取情报最低友好度
	 *		drinkTimes : 邀请酒吧女郎喝酒的次数
	 *		totalCrewNum : 当前水手总数
	 *		friendValue : 自己所在国家与所在城市所属国家的友好度
	 *		usedSudanMask : 是否使用sudan道具进入
	 *		coreTaskIds : side_task_numbers标的id
	 *		skill_mission_terminator_level : 玩家任务达人技能等级(增加任务获取经验2X%)
	 *		skill_late_submission : 玩家延时提交技能等级(提高任务时长2X%)
	 *		npcIndexStr : 可接受任务的战斗npc id
	 *		targetCityStr : 可接受任务的货物运送城市id
	 *		captain_skill_late_submission : 船长任务达人技能等级(增加任务获取经验2X%)
	 *		captain_mission_terminator_level : 船长延时提交技能等级(提高任务时长2X%)
	 */
	void getBarInfo(int, UILoadingIndicator*loading = 0);
	//1:food   2:dirnk  3:banquet
	/*
	 * barConsume : 酒吧消费操作. id : 物品id, actioncode : 消费类型(1为吃, 2为喝, 3为宴请招募水手)
	 * BarConsumeResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	actionCode : 消费类型
	 * 	cost : 消费金额
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void barConsume(int id, int actioncode, UILoadingIndicator*loading = 0);
	/*
	 * getCrew : 招募水手. count : 招募数量 fasthire:是否快速招募
	 * GetCrewResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	crewNum : 招募水手的数量
	 * 	shipCrew : ShipCrewDefine类型, 每艘船上水手的信息
	 * 	cost : 消费金额
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void getCrew(int count, UILoadingIndicator*loading = 0,int fasthire = 0);
	/*
	 * getAvailCrewNum : 获取可以招募水手
	 * GetAvailCrewNumResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	crewPrice : 水手单价
	 * 	availCrewNum : 可以招募水手数
	 * 	currentCrewNum : 当前水手数
	 * 	maxCrewNum : 最大水手数
	 * 	totalRequired : 所需水手最低数量
	 * 	totalSupply : 当前补给品数量
	 * 	oneCrewSupply : 每个水手每天消耗的补给品
	 */
	void getAvailCrewNum(UILoadingIndicator*loading = 0);
	/*
	 * treatBargirl : 和酒吧女郎聊天, 	请她喝一杯
	 * TreatBargirlResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 消费金额
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	prestige : 声望大小
	 * 	exp : 经验值
	 */
	void treatBargirl(UILoadingIndicator*loading = 0);
	/*
	 * giveBargirlGift : 赠送礼物给酒吧女郎. itemId : 物品iid, itemType : 物品类型
	 * GiveBargirlGiftResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	prestige : 与酒吧女郎的友好度
	 * 	itemId : 物品iid
	 * 	itemType : 物品类型
	 */
	void giveBargirlGift(int itemId, int itemType, UILoadingIndicator*loading = 0);
	/*
	 * getTaskList : 获取酒馆任务信息. refresh : 刷新可以接的任务的列表(1为刷新任务列表)
	 * GetTaskListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	actionCode : 没有使用
	 * 	curruntTask : TaskDefine类型, 当前执行的任务
	 * 	cost : 没有使用
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	refreshInterval : 任务刷新时间的间隔
	 * 	nextRefreshTime : 下次刷新任务的时间
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	taskIds : 可接任务id字符串
	 * 	coreTaskIds : 对应taskIds的任务奖励id字符串
	 * 	skill_mission_terminator_level : 玩家任务达人技能等级, 增加任务获取经验2X%
	 * 	skill_late_submission : 玩家延时提交技能等级, 提高任务时长2X
	 * 	npcIndexStr : 对应taskIds的npc id字符串(战斗任务为npc序号, 非战斗任务为goods_amount序号)
	 * 	targetCityStr : 对应taskIds的目标城市的id(非战斗任务为0)
	 * 	captain_skill_late_submission : 船长延时提交技能等级, 提高任务时长2X
	 */
	void getTaskList(int refresh = 0, UILoadingIndicator*loading = 0);
	/*
	 * getIntelligence : 从酒吧女郎那里获取情报
	 * GetIntelligenceResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	json : json字符串, 情报信息
	 */
	void getIntelligence(UILoadingIndicator*loading = 0);
	/*
	 * getCaptain : 雇佣船长. index : 选择雇佣的序号
	 * GetCaptainResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	barCaptain : BarCaptainDefine类型, 雇佣的船长信息
	 * 	cost : 船长每日消耗银币数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	captains : BarCaptainDefine类型, 可雇佣穿上列表
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	averageCost : 买入时的平均价格
	 * 	weight : 一件物品的重量
	 * 	curPackageSize : 当前背包大小
	 * 	maxPackageSize : 背包最大大小
	 *		prices : PriceDataDefine数组, 定义物品价格信息(每组定义价格不等, 购买时要进行计算)
	 *		shipInfos : ShipSizeInfo类型, 船货物装载信息
	 *		skills : SkillDefine : 玩家技能信息(税务协定技能等级, 税收减免X%)
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	npcOnlyPrices : PriceDataDefine数组, 作为流行品交易的价格信息(每组定义价格不等, 购买时要进行计算)
	 * 	skill_expert_sellor : 玩家售卖专家技能等级(增加流行品出售的价格X%)
	 * 	captain_skill_expert_sellor : 船长售卖专家技能等级(增加流行品出售的价格X%)
	 */
	void getCaptain(int index, UILoadingIndicator*loading = 0);

	//// 0:get task, 1:submit task,2 get reward
	/*
	 * handleTask : 对任务进行的操作. id : 任务id, actionCode : 操作方式(0:get task, 1:submit task,2 get reward,3 give up task)
	 * HandleTaskResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	actionCode : 操作方式
	 * 	task : TaskDefine类型, 当前正在执行的任务
	 * 	coin : 银币数量
	 * 	exp : 经验值
	 * 	fame : 声望大小
	 * 	gold : V票数量
	 * 	taskId : 废弃
	 * 	rewardItems : TaskRewardItems类型, 完成任务获取的物品奖励
	 * 	rewardCoin : 完成任务获取的银币奖励
	 * 	rewardGold : 完成任务获取的V票奖励
	 * 	rewardFame : 完成任务获取的声望奖励
	 * 	forceId : 影响友好度势力的id
	 * 	forceAffect : 影响友好度的大小
	 * 	newLevel : 玩家等级
	 * 	captains : CaptainInfo数组, 船长信息
	 * 	new_fame_level : 声望等级
	 * 	taskIds : 废弃
	 */
	void handleTask(int id, int actionCode, UILoadingIndicator*loading = 0);
	/*
	 * getCurrentCityData : 返回城市获取城市相关信息
	 * GetCurrentCityDataResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	data : LoginFirstData类型, 个人登陆的一些相关信息
	 * 	wareHouseDailyCost : 没有使用
	 * 	dockDailyCost : 没有使用
	 * 	captainsDailyCost : 没有使用
	 * 	friendValue : 所在国家和当前城市国家的友好度
	 * 	completeTaskId : 完成任务id
	 * 	completeCoreTaskId : 完成任务的side_task_numbers id
	 * 	newMailCount : 新的邮件数
	 * 	isNewLanding : 是否是登陆的第一个城市
	 * 	popularGoodsIds : 流行品的id
	 * 	sudanmasknum : sudan道具的数量
	 * 	sudanmaskId : sudan道具的在character_item的id
	 * 	hasSudanMask : 是否有苏丹伪装道具
	 * 	hasLisence : 是否有城市许可证
	 * 	reason : 回到城市的原因(2为战斗失败)
	 * 	skillPoints : 可用技能点数
	 * 	wantedForceIds : 被势力通缉的势力id
	 * 	taskIndex : 战斗npc id或运送货物id
	 * 	mainTaskInfo : 主线任务信息
	 * 	onsale_remain_time : 礼包倒计时
	 */
	void getCurrentCityData(UILoadingIndicator*loading = 0);
	/*
	 * GetShipList : 获取舰队中船只信息.
	 * GetShipListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	ship : ShipDefine类型, 舰队中船只信息
	 */
	void getShipList(UILoadingIndicator*loading = 0);
	/*
	 * repairShip : 维修舰队中的船只, 使用V票, 修复船只的最大耐久. shipId : 船只id(0为所有船只)
	 * RepairShipResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 消费的V票数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void repairShipMaxHP(int shipId, UILoadingIndicator*loading = 0);
	/*
	 * repairAllShips : 修复所有船只耐久到当前最大耐久
	 * type = 0,修舰队里得
	 * type = 1，修shipyard里
	 * type = 2 修shipyard + fleet
	 * RepairAllShipsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 消费的银币数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void repairAllShips(int type,UILoadingIndicator*loading = 0);
	/*
	 * fleetFormation : 保存对船坞中舰队的修改. shipIds : 船只id, n : 船只数, captain : 对应船长数据, nCaptain : 船长数, deletedShipIds : 废弃, nDelShips : 废弃,
	 * 																	   renameShipNames : 重命名船只id, nRenameShips : 重命名船只数
	 * FleetFormationResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	dropSailorNum : 废弃
	 */
	void fleetFormation(int*shipIds, int n, FleetCaptain** captain, int nCaptain, int*deletedShipIds, int nDelShips, int *renameShipIds, char**renameShipNames, int nRenameShips, int n_catchshipid, int *catchshipid, UILoadingIndicator*loading = 0);
	/*
	 * parkShip : 已经废弃
	 */
	void parkShip(int shipId, int isUnpark, UILoadingIndicator*loading = 0);
	/*
	 * getDrawings : 获取装备或船只制造的图纸. type : 图纸类型, 值为0制造船只图纸, 值为1制造装备图纸, subType : 一般为0
	 * GetDrawingsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	drawingtype : 图纸类型, 值为0制造船只图纸, 值为1制造装备图纸
	 * 	subType : 一般为0
	 * 	shipdrawing : ShipdrawingDefine类型, 船只图纸信息
	 * 	equipdrawing : EquipdrawingDefine类型, 装备图纸信息
	 * 	currentCityId : 当前城市ID
	 * 	currentCityManufacture : 当前城市制造数
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void getDrawings(int type, int subType, UILoadingIndicator*loading = 0);
	/*
	 * buildShip : 制造船只. drawingId : 图纸id
	 * BuildShipResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 花费的银币数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	useGold :废弃
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	consumedItemId1 : 消耗材料1id
	 * 	consumedCount1 : 消耗材料1数量
	 * 	consumedItemId2 : 消耗材料2id
	 * 	consumedCount2 : 消耗材料2数量
	 */
	void buildShip(int drawingId, UILoadingIndicator*loading = 0);
	/*
	 * getBuildingShips : 获取正在制造的船只或装备.
	 * GetBuildingShipsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	ship : BuildingShipDefine数字, 正在制造的船只信息
	 * 	skills : 玩家技能,SkillDefine类型(制造专家, 提高X%的制造速度. 造船工艺, 增加X个船只制造栏位)
	 */
	void getBuildingShips(UILoadingIndicator*loading = 0);
	/*
	 * finishBuilding : 完成制造. shipManufactureId : 正在制造的船只或装备shipManafactureId
	 * GetBuildingShipsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 花费的V票数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void finishBuilding(int shipManufactureId, UILoadingIndicator*loading = 0);
	/*
	 * getBuildingEquipment : 获取正在制造的装备信息. type : 类型, subType : 子类型
	 * GetBuildingEquipmentResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	equipment : BuildingEquipmentDefine数组, 正在制造的装备的信息
	 * 	drawingType : 传过来的type
	 * 	subType : 传过来的subType
	 * 	skills : 玩家技能,SkillDefine类型(制造专家, 提高X%的制造速度)
	 */
	void getBuildingEquipment(int type, int subType, UILoadingIndicator*loading = 0);
	/*
	 * cancelBuild : 取消制造装备或船只. shipManufactureId : 正在制造的船只或装备shipManafactureId
	 * CancelBuildResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	refundCoins : 返还的银币
	 * 	refundGolds : 返还的V票
	 */
	void cancelBuild(int shipManufactureId, UILoadingIndicator*loading = 0);
	/*
	 * getFleetAndDockShips : 获取舰队和船坞中船只船长装备的信息.
	 * GetFleetAndDockShipsResult : 返回信息
	 * 	failed : 0成功,非0失败
	 * 	fleetShips : ShipDefine类型, 舰队中船只信息
	 * 	dockShips : ShipDefine类型, 船坞中船只信息
	 * 	captains : CaptainDefine类型, 船长信息
	 * 	equipDetails : EquipDetailInfo : 装备信息
	 * 	morale : 士气
	 */
	void getFleetAndDockShips(UILoadingIndicator*loading = 0);
	/*
	 * getReinforceShipList : 获取船只强化列表.
	 * GetReinforceShipListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	ship : ReinforceShipDefine类型, 已经强化或可以强化的船只的信息
	 * 	item : ItemDefine类型, 船只改造方案信息
	 * 	reinforceShipCostGold : 强化所需V票数
	 * 	rerollShipCostGold : 重新强化所需V票数
	 * 	reinforceShipCostCoin : 强化所需银币数
	 */
	void getReinforceShipList(UILoadingIndicator*loading = 0);
	/*
	 * reinforceShip : 强化船只. shipid : 船只id, optional1 : 船只改造方案id, type : 强化位置, 位置1或位置2, optionNum : 废弃
	 * ReinforceShipResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 花费银币数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	ItemType : 废弃
	 * 	ItemId : 废弃
	 * 	shipInfo : ReinforceShipDefine类型,强化的船只信息
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void reinforceShip(int shipid, int optional1, int type, int optionNum, UILoadingIndicator*loading = 0);
	/*
	 * rerollShip : 重置船只强化数值. shipid : 船id, optional1 : 强化槽1的改造方案id, optional2 : 强化槽2的改造方案id, optionNum : 废弃
	 * RerollShipResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 花费Ｖ票数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	shipInfo : ReinforceShipDefine类型,强化的船只信息
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void rerollShip(int shipid, int optional1, int optional2, int optionNum, UILoadingIndicator*loading = 0);
	/*
	 * getReinforceEquipList : 获取装备强化信息
	 * GetReinforceEquipListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	equipmentOnShip : ReinforceEquipDefine类型, 船上装备信息
	 * 	equipmentsInBag : ReinforceEquipDefine类型, 背包装备信息
	 * 	reinforceEquipCostGold : 强化需要Ｖ票数量
	 * 	rerollEquipCostGold : 重置强化需要Ｖ票数量
	 * 	reinforceEquipCostCoin : 强化需要银币数量
	 * 	item : ItemDefine类型, 装备改造方案信息
	 */
	void getReinforceEquipList(UILoadingIndicator*loading = 0);
	/*
	 * reinforceEquip : 强化装备. equipmentId : 装备id, optionId : 装备iid, optionNum : 大于0的值, shipId : 船的id,
	 * 						                     position : 装备类型(1000x,cannon; 2000x,armor;3000x,spinnaker;4000x,bow_gun 5000x,tail gun;6000x bow icon)
	 * ReinforceEquipResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 花费银币数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	equipInfo : ReinforceEquipDefine类型, 强化装备信息
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void reinforceEquip(int equipmentId, int optionId, int optionNum, int shipId, int position, UILoadingIndicator*loading = 0);
	/*
	 * rerollEquip : 重置装备强化数值. equipmentId : 装备id, optionId : 装备iid, optionNum : 废弃, shipId : 船id, position : 废弃
	 * RerollEquipResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 花费Ｖ票数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	equipInfo : ReinforceEquipDefine类型, 强化装备信息
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void rerollEquip(int equipmentId, int optionId, int optionNum, int shipId, int position, UILoadingIndicator*loading = 0);
	/*
	 * equipShip : 装备船只接口. shipId : 船只id, 其他参数 : 不同位置的装备id
	 * EquipShipResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void equipShip(int shipId, int bowIconId, int bowGunId, int aftGunId, char*spinnakerIds, char*armorIds, char*cannonIds, UILoadingIndicator*loading = 0);
	/*
	 * getEquipShipInfo : 获取船只装备信息. shipid : 船只id
	 * GetEquipShipInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	shipInfo : EquipShipInfo类型, 船只信息
	 * 	shipList : 数组, 船坞中船之id
	 * 	bowIconId : bowIconId
	 * 	bowGunId : bowGunId
	 * 	aftGunId : aftGunId
	 * 	spinnakerIds : 数组, spinnakerIds
	 * 	armorIds : 数组, armorIds
	 * 	cannonIds : 数组, cannonIds
	 * 	equipDetails : EquipDetailInfo数组, 穿上装备信息
	 * 	maxBowIconNum : 最大BowIcon个数
	 * 	maxBowGunNum : 最大BowGu个数
	 * 	maxAftNum : 最大aftGun个数
	 * 	maxRigNum : 最大spinnaker个数
	 * 	maxArmorNum : 最大armor个数
	 * 	maxCannonNum : 最大cannon个数
	 * 	sidList : 数组, 船坞中船之sid
	 */
	void getEquipShipInfo(int shipid, UILoadingIndicator*loading = 0);
	/*
	 *	getEquipableItems : 获取装备信息. type : 船装备子类型(bowIconId=1 bowGunId=2 aftGunId=3 spinnaker=4 rigged=5 armor=6 cannonIds=7), isForHero : 值为1获取人装备
	 *	GetEquipableItemsResult : 返回结果
	 *		failed : 0成功,非0失败
	 *		items : EquipableItem类型, 装备基本信息
	 *		equipDetails : EquipDetailInfo类型, 装备详细信息
	 *		isHeroEquip : 值为1获取人装备
	 *		type : 船装备子类型
	 */
	void getEquipableItems(int type, int isForHero, UILoadingIndicator*loading = 0);
	/*
	 * getVisitedCities : 获取已经登陆过的城市
	 * GetVisitedCitiesResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	city : CityDefine数组, 登陆过的城市信息
	 * 	currentSupply : 当前供给
	 * 	maxSupply : 最大供给
	 * 	currentCrewNum : 当前税收数量
	 * 	maxCrewNum : 最大水手数量
	 * 	enduranceDays : 可航行天数
	 * 	costPerSupply : 补给的平均消费
	 * 	costPerCrew : 船员的平均消费
	 * 	sailorsIsEnough : 水手是否满员
	 * 	shipCrew : ShipCrewDefine数组
	 * 	coins : 银币数量
	 * 	gold : 金币数量
	 * 	fame : 声望值
	 * 	exp : 经验值
	 * 	wareHouseDailyCost : 仓库每日花费
	 * 	dockDailyCost : 船坞每日花费
	 * 	captainsDailyCost : 船长每日花费
	 * 	dailyCost : 每日总花费
	 * 	wantedForceIds : 被通缉的国家id
	 * 	shipSpeed : 船速
	 * 	currentPackageSize : 当前背包货物重量
	 * 	maxPackageSize : 背包能够承受的货物重量
	 * 	inCity : 是否在城市中
	 */
	void getVisitedCities(UILoadingIndicator*loading = 0);
	/*
	 * addSupply : 出海添加补给. amountToAdd : 添加补给数量
	 * AddSupplyResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cost : 花费银币
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	enduranceDays : 可以在海上航行的天数
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	currentSupply : 当前补给数量
	 * 	addedSupply : 增加的补给数量
	 */
	void addSupply(int amountToAdd, UILoadingIndicator*loading = 0);
	/*
	 * getNextMapBlock : 获取海上坐标信息. currentX : 海上横坐标, currentY : 海上纵坐标, sailDays : 航行天数
	 * GetNextMapBlockResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	blockInfo : SeaBlockInfo类型, 海上相关信息
	 * 	days : 没有使用
	 * 	totalSailors : 水手总数
	 * 	coin : 银币数量
	 * 	gold : v票数量
	 * 	fame : 声望值
	 * 	exp : 经验值
	 * 	eventChanged : 海上时间是否刷新
	 * 	currentSupply : 当前补给品数量
	 * 	maxSupply : 最大补给品数量
	 * 	maxSailorNum : 最大水手数量
	 * 	newLevel : 新的等级
	 * 	captains : CaptainInfo数组类型
	 * 	brokenEquips : OutOfDurableItemDefine数组类型, 损坏装备的信息
	 */
	bool getNextMapBlock(ShipRoutine*shipRoutine, int sailDays, UILoadingIndicator*loading = 0);
	/*
	 * leaveCity : 离开当前城市
	 * LeaveCityResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cityX : 城市x坐标
	 * 	cityY : 城市y坐标
	 * 	blockInfo : SeaBlockInfo类型, 海上相关信息
	 * 	shipId : 速度最快的船id
	 * 	speed : 舰队航行速度
	 * 	shipX : 城市出港x坐标
	 * 	shipY : 城市出港y坐标
	 * 	enduranceDays : 可以航行的天数
	 * 	sailorCount : 水手数量
	 * 	visitedCityIds : 已经登陆过的城市id
	 * 	wareHouseDailyCost : 仓库每日花费
	 * 	dockDailyCost : 船坞每日花费
	 * 	captainsDailyCost : 船长每日花费
	 * 	dailyCost : 每日花费总额
	 * 	maxTotalSupply : 最大补给品数量
	 * 	maxSailorCount : 最大水手数
	 * 	exp : 玩家经验值
	 * 	fame : 玩家声望值
	 * 	expPerday : 海上每日获得经验值
	 * 	discoverdCityIds : 已经发现的城市的id
	 * 	requiredSailorCount : 需要的最小税收数
	 * 	wantedForceIds : 被通缉的国家id
	 * 	shipNum : 船只数量
	 * 	fightNpcId : 上一个未结束战斗的npcid
	 * 	caption_skill_good_cook : 船长好厨师技能等级(减少补给品消耗速度X%)
	 * 	skill_ocean_exploration : 玩家海洋探索技能等级(提高X%的大地图航行速度)
	 * 	skill_good_cook : 玩家烹饪技巧技能等级(减少补给品消耗速度3X%)
	 */
	void leaveCity(UILoadingIndicator*loading = 0);
	/*
	 * reachCity : 登陆城市. currentX : 当强横坐标, currentY : 当前纵坐标
	 * ReachCityResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	currentCityId : 登陆城市的id
	 * 	mainTaskStatus : 主线任务状态
	 * 	sailingDays : 航行的天数
	 * 	fleetShips : ShipDefine数组
	 * 	wareHouseDailyCost : 仓库每日消费
	 * 	dockDailyCost : 船坞每日消费
	 * 	captainsDailyCost : 船长每日消费
	 * 	dailyCost : 每日消费总额
	 * 	exp : 经验值
	 * 	fame : 声望值
	 * 	coins : 银币数量
	 * 	gold : v票数量
	 * 	expPerday : 航行每天获取经验
	 * 	addedExp : 航行获取的经验值
	 * 	newLevel : 新的等级
	 * 	captains : CaptainInfo数组, 船长信息
	 */
	void reachCity(int currentX, int currentY, UILoadingIndicator*loading = 0);

	//propid set to 0 if not using prop
	/*
	 * triggerSeaEvent : 触发海上事件. eventId : 时间id, propId : 没有使用
	 * TriggerSeaEventResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	seaInfo : SeaBlockInfo类型, 海上相关信息
	 * 	coin : 银币数量
	 * 	gold : v票数量
	 * 	loot : SeaEventLootDefine类型, 掉落的漂浮物
	 * 	loss : SeaEventLossDefine类型, 遇到灾难损失信息
	 * 	fame : 声望值
	 * 	exp : 经验值
	 * 	eventId : 没有使用
	 * 	eventChanged : 遇到漂浮物会将这个值设置为1
	 * 	lootCoins : 失去银币数量
	 * 	type : 时间类型
	 * 	useProp : 是否使用物品(防止鼠疫等)
	 * 	usedPropId : 使用物品iid(防止鼠疫等)
	 * 	usedPropRemainCount : 消耗物品剩余数量(防止鼠疫等)
	 * 	brokenEquips : OutOfDurableItemDefine类型, 损坏装备信息
	 */
	void triggerSeaEvent(int eventId, int propId, UILoadingIndicator*loading = 0);
	/*
	 * finishSeaEvent : 没有做任何事情
	 * FinishSeaEventResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void finishSeaEvent(UILoadingIndicator*loading = 0);
	/*
	 * getPalaceInfo : 获取皇宫信息.
	 * GetPalaceInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	development : 1王宫, 2总督府, 3官邸
	 * 	population : 人口
	 * 	trade : 贸易等级
	 * 	manufacture : 制造等级
	 * 	myInvestment : 我的投资
	 * 	totalInvestment : 废弃
	 * 	isMayor : 当前玩家是否是市长标志(值为1代表是市长)
	 * 	oldDevDirection : 旧城市发展方向(1人口, 2交易, 3制造)
	 * 	newDevDirection : 新城市发展方向
	 * 	nationIdx : 当前城市的国籍
	 * 	canSelectDevDirection : 是否可以选择城市发张方向
	 * 	mayorName : 市长姓名
	 * 	mayorLevel : 市长等级
	 * 	mayorFame : 市长声望
	 * 	mayorIcon : 市长头像图表
	 * 	mayorNation : 市长所属国家
	 * 	nextCycleTime : 下一次市长选举的时间
	 * 	isFirstday : 是否是第一天当选市长
	 * 	lastSessionTopInvest : 上周市长投资额
	 * 	licensePrice : 购买商业许可证需要的银币数量
	 * 	licenseRequiredFameLv : 购买商业许可证需要的等级
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 */
	void getPalaceInfo(UILoadingIndicator*loading = 0);
	/*
	 * investCity : 向投资城市银币. investNum : 投资银币数量
	 * InvestCityResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	add_fame : 增加声望数
	 * 	new_fame_level : 声望等级
	 * 	skill_public_speaking : 玩家技能演说技巧等级(提高投资声望X%)
	 */
	void investCity(int64_t investNum, UILoadingIndicator*loading = 0);
	/*
	 * getRankList : 获取历史投资排行
	 * GetRankListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	rankInfo : RankInfoDefine数组, 排名信息
	 * 	myRank : 当前玩家排名
	 * 	myInvest : 当前玩家投资银币数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	myGuildId : 当前玩家工会id
	 * 	isAdmin : 是工会管理员或者创建者标志(1工会管理员或者创建者)
	 */
	void getRankList(UILoadingIndicator*loading = 0);
	/*
	 * getVoteList : 获取本周选举投票排行
	 * GetVoteListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	rankinfo : RankInfoDefine数组, 排名信息
	 * 	mayorName : 当前市长名字
	 */
	void getVoteList(UILoadingIndicator*loading = 0);
	/*
	 *	voteMayor : 选举投票操作. id : 候选人id
	 *	VoteMayorResult : 返回结果
	 *		failed : 0成功,非0失败
	 */
	void voteMayor(int id, UILoadingIndicator*loading = 0);
	/*
	 * changeDevDir : 改变城市发展方向. newDevDirection : 新的城市发展方向(1人口, 2交易, 3制造)
	 * ChangeDevDirResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	newDevDirection : 新的城市发展方向
	 */
	void changeDevDir(int newDevDirection, UILoadingIndicator*loading = 0);

	/*
	 * depositMoney : 向银行存钱操作. money : 存储银币的数量, isPersonal : 是否是个人存储类型(值大于0个人, 值为0工会)
	 * DepositMoneyResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void depositMoney(int64_t money, int isPersonal, UILoadingIndicator*loading = 0);
	/*
	 * withdrawMoney : 从银行取钱操作. money : 取出银币的数量, isPersonal : 是否是个人取出类型(值大于0个人, 值为0工会)
	 * WithdrawMoneyResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void withdrawMoney(int64_t money, int isPersonal, UILoadingIndicator*loading = 0);
	/*
	 * depositItem : 向银行存储物品. id : 物品id, iid : 物品的iid, type : 物品类型, num : 物品数量, isPersonal : 是否是个人存储类型(值大于0个人, 值为0工会)
	 * DepositItemResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	id : 返回存储id
	 * 	type : 物品类型
	 */
	void depositItem(int id, int iid, int type, int num, int isPersonal, UILoadingIndicator*loading = 0);
	/*
	 * withdrawItem : 从银行存储物品. id : 物品id, iid : 物品的iid, type : 物品类型, num : 物品数量, isPersonal : 是否是个人取出类型(值大于0个人, 值为0工会)
	 * WithdrawItemResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	id : 返回物品id
	 * 	type : 物品类型
	 * 	num : 取出数量
	 */
	void withdrawItem(int id, int type, int num, int isPersonal, UILoadingIndicator*loading = 0);
	/*
	 * expandBank : 扩展个人或工会银行保险箱一个格子. isPersonal : 是否是个人保险箱(值大于0个人, 值为0工会)
	 * ExpandBankResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	bankCapacity : 保险箱格子容量
	 * 	cost : 花费银币数量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 */
	void expandBank(int isPersonal, UILoadingIndicator*loading = 0);
	/*
	 * getBankInfo : 获取个人或者工会银行信息. isPersonal : 值大于0个人, 值为0工会
	 * GetBankInfoResult : 返回结果
	 *		failed : 0成功,非0失败
	 * 	coin : 银币数量
	 * 	bankId : bankId
	 * 	maxItemNum : 银行保险箱格子数量
	 * 	bankItem : BankItemDefine数组, 银行保险箱存放的物品
	 * 	isPersonal : 值大于0个人, 值为0工会
	 * 	fame : 声望大小
	 * 	exp : 经验值
	 * 	gold : V票数量
	 * 	guildId : 玩家所属工会id
	 * 	deposit_item(工会银行专属) : 玩家是否可以向工会银行存入物品(1可以, 0不可以)
	 * 	withdraw_item(工会银行专属) : 玩家是否可以从工会银行取出物品(1可以, 0不可以)
	 * 	deposit_cash(工会银行专属) : 玩家是否可以向工会银行存入银币(1可以, 0不可以)
	 * 	withdraw_cash(工会银行专属) : 玩家是否可以从工会银行存入取出(1可以, 0不可以)
	 * 	isAdmin(工会银行专属) : 值为1工会创建者或工会管理员
	 */
	void getBankInfo(int isPersonal, UILoadingIndicator*loading = 0);
	/*
	 * getBankLog : 获取银行存取记录. isPersonal : 值大于0个人, 值为0工会
	 * GetBankLogResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	log : log信息(json字符串)
	 */
	void getBankLog(int isPersonal, UILoadingIndicator*loading = 0);
	/*
	 * setMainTaskStatus : 设置玩家新手教程的完成程度. stage : 完成程度值
	 * SetMainTaskStatusResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	status : 完成程度值
	 */
	void setMainTaskStatus(char*shipName,int stage, UILoadingIndicator*loading = 0);
	//when eventid == -1,use last unfinished fight npc id.
	/*
	 * engageInFight : 投入战斗, 在战斗前获取相关信息. eventId : 海上事件id, forceFight : 是否可以取消战斗, eventVersion : 没有使用
	 * EngageInFightResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	forceFight : 是否可以取消战斗
	 * 	props : PropsInfo类型
	 * 	myships : FightShipInfo类型, 玩家舰队船只信息
	 * 	enemyships : FightShipInfo类型, 对手舰队船只信息
	 * 	myinfo : FightSummary类型, 玩家舰队概要信息(包括战斗力等)
	 * 	enemyinfo : enemyinfo类型, 对手舰队概要信息(包括战斗力等)
	 * 	victoryPercent : 胜率
	 * 	key : %d_in_battle(user_id)
	 * 	filename : %d_%s.log(user_id, uuid)
	 * 	morale : 玩家舰队士气
	 * 	npcId : 海上npc id
	 * 	npcMorale : 海上npc士气
	 * 	npcForceId : 海上npc所属势力id
	 * 	positiveSkills : SkillDefine数组
	 * 	eventId : 事件id

	fightType //0 normal 1 activities
	  fightLevel  // 1 simple 2 normal 3 hard

	  SkillDefine里新增 skilltype 0 npc ,1 captain,2 player
	  */
	void engageInFight(int eventId, int forceFight, int64_t eventVersion,int fightType,int fightLevel, UILoadingIndicator*loading = 0);
	void notifyBattleLog(int fightType, UILoadingIndicator*loading = 0);
	/*
	 * cancelFight : 取消战斗. type : 0 免费，1 停战道具，2 v票,
	 * eventId : 废弃
	 * CancelFightResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void cancelFight(int type,EndFight*lastFightInfo = nullptr,UILoadingIndicator*loading = 0);
	/* win = 1 ,fail = 2, ===3, use prop to end 4 */
	/*
	 * endFight : 结束战斗. reason : 结束原因, nShip : 船只数量, props : 使用道具信息, nProp : 道具数量, morale : 士气
	 *                                    hpArray : 船只耐久, sailNumArray : 船只水手数 , shipIdArry : 船只id
	 * EndFightResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	reason : 返回原因(2为战败, 1为胜利, 3平手, 4使用道具,5 失败，但是使用v票挽救舰队)
	 * 	lootCoins : 获得的银币数
	 * 	lootItems : LootDefine类型
	 * 	lootGoods : LootDefine类型
	 * 	supply : 获取的补给品数量
	 * 	addedSailDays : 增加的航行天数
	 * 	lootProps : 获取的物品
	 * 	lootFame : 增加的声望
	 * 	coins : 银币数量
	 * 	seaInfo : SeaBlockInfo类型, 海上相关信息
	 * 	lostCoinPercent : 失去银币的百分比
	 * 	lostShips : SunkShipDetails类型, 沉没的船只
	 * 	lostGoods : LostGoodsDefine类型, 失去的货物
	 * 	shipX : 船只位置x坐标
	 * 	shipY : 船只位置y坐标
	 * 	enduranceDays : 没有使用
	 * 	sailorCount : 水手数
	 * 	golds : v票数
	 * 	lootExp : 增加的经验值
	 * 	TotalExp : 经验值
	 * 	TotalFame : 声望值
	 * 	ExpLv : 等级
	 * 	FameLv : 声望等级
	 * 	newLevel : 新等级
	 * 	captains : CaptainInfo数组
	 * 	new_fame_level : 新声望等级
	 * 	isAutoFight : 是否是自动战斗
	 * 	survivedShips : SurvivedShipdetails数组
	 * 	totalSupply : 当前补给品数量
	 * 	brokenEquips : OutOfDurableItemDefine数组
	 * 	npcId : 战斗的npc id
	 * 	fightType  0 normal 1 for task 2 for pirate attack
	 */
	void endFight(int reason, int nShip, int*hpArray, int*sailNumArray, int* shipIdArry, PropsInfo**props, int nProp, int morale, int fightType, int remainSec, int attack_hurt, int attack_sailors, int n_catchshipids, int* catchshipids, int n_catchgoodsshipids, int *catchgoodsshipids, int usesec,BattleLog*log, UILoadingIndicator*loading = 0);
	/*
	 * startAutoFight : 开始自动战斗
	 *	StartAutoFightResult : 返回结果(调用endfight, 在endfight返回)
	 *		failed : 失败返回1
	 */
	void startAutoFight(UILoadingIndicator*loading = 0);
	void getSkillsDetails(UILoadingIndicator*loading = 0);
	/*
	 * addSkillPoint : 增加玩家技能等级. skillId : 技能id, points : 增加的点数(一般为1)
	 * AddSkillPointResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	skillId : 技能id
	 * 	skillLv : 技能等级
	 * 	remainSkillPoints : 剩余的技能点数
	 */
	void addSkillPoint(int skillId, int points, UILoadingIndicator*loading = 0);
	/*
	 * remainSkillPoints : 重置玩家技能点数, 技能点被返还
	 * ResetSkillPointsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	totalPoints : 全部技能点数
	 */
	void resetSkillPoints(UILoadingIndicator*loading = 0);
	/*
	 * getPersonalInfo : 获取个人信息接口.
	 * GetPersonalInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	heroName : 玩家名字
	 * 	iconId : 头像图表id
	 * 	level : 玩家等级
	 * 	exp : 经验值
	 * 	fame : 声望值
	 * 	fameLevel : 声望等级
	 * 	bio : 玩家签名
	 * 	guildName : 工会名字
	 * 	nationId : 所属国家id
	 * 	regTime : 玩家在线时长(小时)
	 * 	dailyCost : 每日开支
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	bankCoins : 银行存款
	 * 	nation : FriendValue类型, 所在国家和其他国家的友好度
	 * 	personal : FriendValue类型, 所在工会和其他国家的友好度
	 *		gender : 性别(1男性, 2女性)
	 *		guildId : 所在工会id
	 *		wareHouseDailyCost : 仓库每日花费
	 *		dockDailyCost : 船坞每日花费
	 *		captainsDailyCost : 雇佣的船长每日花费
	 *		guildIcon : 工会图标id
	 *		skillPoints : 剩余技能点数
	 */
	void getPersonalInfo(UILoadingIndicator*loading = 0);
	/*
	 * setBioInfo : 设置个人签名. info : 签名内容
	 * SetBioInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void setBioInfo(char*info, UILoadingIndicator*loading = 0);
	/*
	 * getGuildList : 获取所有工会列表
	 * GetGuildListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	guildid : 玩家当前所在工会id
	 * 	guilds : GuildInfoDefine数组, 工会列表信息
	 * 	memberStatus : 玩家在当前工会的身份(0 normal, 1 admin 2 creator)
	 * 	applicationNum : 待处理申请加入工会数
	 * 	invitationNum : 待处理邀请加入工会数
	 */
	void getGuildList(UILoadingIndicator*loading = 0);
	/*
	 * getMyGuildDetails : 获取玩家当前所在工会信息
	 * GetMyGuildDetailsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	id : 工会id
	 * 	maxMemberNum : 工会成员最大容量
	 * 	name : 工会名称
	 *		members : GuildPersonalInfoDefine类型, 工会成员信息
	 *		intro : 工会简介(目前编辑公告会改变)
	 *		nation : 国家id
	 *		currentMemNum : 当前工会成员数
	 *		creatorId : 工会创建者id
	 *		creatorName : 创建者姓名
	 *		relations : GuildRelationDefine数组, 工会对国家的友好度
	 *		myStatus : 玩家在工会的身份(0 normal, 1 admin 2 creator)
	 *		announcement : 工会公告
	 *		guildIcon : 工会图标id
	 * 	applicationNum : 待处理申请加入工会数
	 * 	invitationNum : 待处理邀请加入工会数
	 */
	void getMyGuildDetails(UILoadingIndicator*loading = 0);
	/*
	 * getApplyInfo : 获取工会的申请信息
	 * GetApplyInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	guild_id : 工会id
	 * 	applications : ApplyPersonInfoDefine数组, 申请的详细信息
	 */
	void getApplyInfo(UILoadingIndicator*loading = 0);
	/*
	 * createNewGuild : 创建工会. guildName : 工会名称, iconIdx : 工会图表id
	 * CreateNewGuildResult : 返回结果
	 *		failed : 0成功,非0失败
	 *		guildId : 工会id
	 *		reqLv : 10
	 *		reqFame : 15 * 1000
	 *		reqCoins : 100 * 10000
	 *		curLv : 玩家当前等级
	 *		curFame : 玩家当前声望值
	 *		curCoins : 玩家当前银币数量
	 */
	void createNewGuild(char *guildName, int iconIdx, char*intro, UILoadingIndicator*loading = 0);
	/*
	 * JoinGuildRequest : 加入工会申请 guildId : 工会id
	 * JoinGuildRequestResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	maxMemberNum : 废弃
	 * 	coins : 废弃
	 * 	gold : 废弃
	 */
	void joinGuildRequest(int guildId, UILoadingIndicator*loading = 0);
	/*
	 * 根据工会id获取工会信息(列表中的或玩家当前所在工会). guildId : 工会id
	 * GetGuildDetailsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	id : 工会id
	 * 	maxMemberNum : 工会成员最大容量
	 * 	name : 工会名称
	 *		intro : 工会简介(目前编辑公告会改变这个值)
	 *		nation : 国家id
	 *		currentMemNum : 当前工会成员数
	 *		creatorId : 工会创建者id
	 *		creatorName : 创建者姓名
	 *		relations : GuildRelationDefine数组, 工会对国家的友好度
	 *		myStatus : 玩家在工会的身份(0 normal or not in this guild, 1 admin 2 creator)
	 *		myGuildId : 玩家当前所在工会
	 *		guildIcon : 工会图标id
	 */
	void getGuildDetails(int guildId, UILoadingIndicator*loading = 0);
	/*
	 * changeGuildIntro : 修改当前工会介绍(点击公告会修改) intro : 修改的内容
	 * ChangeGuildIntroResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void changeGuildIntro(char *intro, UILoadingIndicator*loading = 0);
	/*
	 * expandGuildCapacity : 扩展工会最大成员容量+1
	 * ExpandGuildCapacityResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	newCapacity : 新的工会成员容量
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	curMemberNum : 当前成员数量
	 */
	void expandGuildCapacity(UILoadingIndicator*loading = 0);
	/*
	 * getGuildMemberPermission : 获取玩家当前所在工会成员的权限信息(包括管理员,创建者,存取权限)
	 * GetGuildMemberPermissionResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	create_id : 创建者id
	 * 	creator_name : 创建者名字
	 * 	members : GuildMemberPermissionDefine数组, 工会成员的权限信息
	 */
	void getGuildMemberPermission(UILoadingIndicator*loading = 0);
	/*
	 * setGuildMemberPermission : 设置工会中成员的权限 defines : SetGuildPermissionDefine类型, 设置的工会成员的权限信息(包括管理员,创建者,存取权限)
	 * SetGuildMemberPermissionResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void setGuildMemberPermission(SetGuildPermissionDefine**defines, int n_define, UILoadingIndicator*loading = 0);
	/*
	 * getHiredCaptains : 获取已经雇佣的船长信息
	 * GetHiredCaptainsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	myCaptains : BarCaptainDefine数组, 定义船长信息
	 */
	void getHiredCaptains(UILoadingIndicator*loading = 0);
	/*
	 * dealWithGuildJoin : 管理者处理其他玩家加入工会申请. accept : 0拒绝, 1同意, application_id : 申请id, cid : 申请者id, guild_id : 工会id
	 * 	failed : 0成功,非0失败
	 * 	action : 0拒绝, 1同意
	 */
	void dealWithGuildJoin(int accept, int application_id, int cid, int guild_id, UILoadingIndicator*loading = 0);
	/*
	 * removeGuildMember : 将其他玩家从工会删除. guild_id : 工会id, member_character_id : 玩家id
	 * RemoveGuildMemberResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void removeGuildMember(int guild_id, int member_character_id, UILoadingIndicator*loading = 0);
	/*
	 * exitFromGuild : 退出工会.
	 * ExitFromGuildResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void exitFromGuild(UILoadingIndicator*loading = 0);
	/*
	 * dismissGuild : 解散工会
	 * DismissGuildResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void dismissGuild(UILoadingIndicator*loading = 0);
	/*
	 * getSearchGuildList : 根据名字查找工会. keyword : 工会名
	 * GetSearchGuildListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	guildid : 工会id
	 * 	guilds : GuildInfoDefine类型, 工会信息
	 */
	void getSearchGuildList(char *keyword, UILoadingIndicator*loading = 0);
	/*
	 * changeGuildAnnouncement : 修改工会公告(游戏界面没发现这个接口). guildId : 工会id, newAnnouncement : 修改公告内容
	 * ChangeGuildAnnouncementResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void changeGuildAnnouncement(int guildId, char *newAnnouncement, UILoadingIndicator*loading = 0);
	/*
	 * refreshCaptainList : 刷新酒馆可以雇佣的穿证列表. useGold : 花费Ｖ票数量
	 * RefreshCaptainListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	captains : BarCaptainDefine数组, 可雇佣船长的信息
	 * 	gold : V票数量
	 * 	coin : 银币数量
	 * 	costGold : 花费Ｖ票数量
	 * 	refreshInterval : 刷新时间间隔(秒)
	 * 	lastRefreshTime : 上次刷新时间(unixstamp)
	 */
	void refreshCaptainList(int useGold, UILoadingIndicator*loading = 0);
	/*
	 * GetFriendsList : 获取好友列表
	 * GetFriendsListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	friends : FriendsDefine数组, 好友列表信息
	 * 	friendsRequestNum : 当前向我申请好友的数量
	 */
	void getFriendsList(UILoadingIndicator*loading = 0);
	/*
	 * getBlackList : 获取黑名单列表
	 * GetBlackListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	friends : FriendsDefine数组, 黑名单列表信息
	 */
	void getBlackList(UILoadingIndicator*loading = 0);
	/*
	 * friendsOperation : 玩家对好友列表的操作. characterId : 好友的id, actionCode : 操作类型(0 add,1,delete,2,blacklist,3 accept friends req,4 refuse friends req,5 remove from blacklist)
	 * FriendsOperationResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	actionCode : 操作类型
	 * 	characterId : 好友的id
	 */
	void friendsOperation(int characterId, int actionCode, UILoadingIndicator*loading = 0); ////0 add,1,delete,2,blacklist,3 accept friends req,4 refuse friends req
	/*
	 * getFriendsReqList : 获取玩家的好友请求列表.
	 * GetFriendsReqListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	friends : FriendsDefine数组, 玩家好友信息
	 */
	void getFriendsReqList(UILoadingIndicator*loading = 0);
	/*
	 * fireCaptain : 解雇船长. captainId : 船长id
	 * FireCaptainResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	myCaptains : BarCaptainDefine数组, 玩家当前所雇佣的船长信息
	 */
	void fireCaptain(long captainId, UILoadingIndicator*loading = 0);
	/*
	 * destoryItems : 在玩家物品栏中删除物品. uniqueId : 物品id标识, count : 物品数量, type : 物品类型
	 * destoryItems : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	count : 删除物品的数量
	 */
	void destoryItems(int uniqueId, long count, int type, UILoadingIndicator*loading = 0);
	/*
	 * searchUserByName : 根据玩家名字查找玩家. name : 玩家姓名
	 * SearchUserByNameResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	friends : FriendsDefine类型, 所查找玩家的信息
	 */
	void searchUserByName(char*name, UILoadingIndicator*loading = 0);
	/*
	 * takeAllAttachment : 废弃
	 */
	void takeAllAttachment(int mailId, UILoadingIndicator*loading = 0);
	/*
	 * getItemsDetailInfo : 根据物品在数据库的唯一标识获取物品详细信息. itemId : 物品iid, itemType : 物品类型, uniqueId : 物品的唯一id标识
	 * GetItemsDetailInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	itemType : 物品类型
	 * 	itemId : 物品iid
	 * 	uniqueId : 物品的唯一id标识
	 * 	goods : HatchItemsDefine类型, 货物信息
	 * 	ship : ShipDefine类型, 船只信息
	 * 	equipment : EquipmentDefine类型, 装备信息
	 * 	drawing : DrawingItemsDefine类型, 图纸信息
	 * 	special : SpecialItemsDefine类型, V票信息
	 */
	void getItemsDetailInfo(int itemId, int itemType, int uniqueId, UILoadingIndicator*loading = 0);
	/*
	 * getDevGoodsInfo : 获取城市发展需要物品信息
	 * GetDevGoodsInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	goods : DevGoodsDetails数组, 城市发展物品信息
	 * 	nextCycleTime : 距离下一个阶段剩余的时间
	 * 	rewardCoins : 完成需求给予市长的银币奖励
	 * 	rewardGolds : 完成需求给予市长的Ｖ票奖励
	 * 	rewardFame : 完成需求给予市长的声望奖励
	 * 	isFirstday : 是否是第一阶段
	 * 	mayorId : 市长id
	 * 	oldDevDirection : 上一个发展方向
	 * 	newDevDirection : 新的发展方向
	 * 	skill_administration : 玩家市政厅常客技能等级(市政厅物资交易奖励声望提升X%)
	 * 	captain_skill_good_reputation : 船长交易声望技能等级(增加流行品卖出的声望0.5X%)
	 */
	void getDevGoodsInfo(UILoadingIndicator*loading = 0);
	/*
	 * ProvideDevGoods : 提供城市发展所需的物品. goodsIndex : 货物id, count : 提供数量
	 * ProvideDevGoodsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	addedCount : 增加的数量
	 * 	currentCount : 当前数量
	 * 	requiredCount : 需要的总数量
	 * 	goodsId : goodId
	 * 	addedCoins : 获得的银币数量
	 * 	addedFame : 获得声望值
	 * 	coins : 银币数量
	 * 	fame : 声望值
	 * 	addedExp : 增加的经验值
	 * 	exp : 当前经验值
	 * 	new_fame_level : 新的声望等级
	 * 	skill_administration : 玩家市政厅常客技能等级(市政厅物资交易奖励声望提升X%)
	 * 	captain_skill_good_reputation : 船长玩家交易声望技能等级(增加流行品卖出的声望0.5X%)
	 */
	void provideDevGoods(int goodsIndex, int count, UILoadingIndicator*loading = 0);
	/*
	 * getDevGoodsCount : 获取玩家可以提供的城市发展所需物品的数量. goodsIndex : 物品序号(1-3)
	 * GetDevGoodsCountResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	count : 可以提供的数量
	 * 	goodsId : 货物id
	 */
	void getDevGoodsCount(int goodsIndex, UILoadingIndicator*loading = 0);
	/*
	 * getFriendValue : 王宫中查看城市信息, 获取友好度
	 * GetFriendValueResult : 返回结果
	 * forCity 为1 代表获取当前城市的
	 */
	void getFriendValue(int forCity,UILoadingIndicator*loading = 0);
	void getUserInfoById(int cid, UILoadingIndicator*loading = 0);

	void testCreateNewGuild(UILoadingIndicator*loading = 0);
	void inviteUserToGuild(int cid, UILoadingIndicator*loading = 0);
	void getInvitationList(UILoadingIndicator*loading = 0);
	void dealWithInvitation(int invitationId, int accept, UILoadingIndicator*loading = 0);

	void getVTicketMarketItems(int type, UILoadingIndicator*loading = 0);
	void buyVTicketMarketItem(int type, int uniqueId, int count, UILoadingIndicator*loading = 0);

	void getCurrentInvestData(UILoadingIndicator*loading = 0);
	void getUserTasks(UILoadingIndicator*loading = 0);
	void useSudanMask(int64_t id, UILoadingIndicator*loading = 0);
	/*
	 * equipHero : 玩家装备人装备. headId : 帽子id, clothId : 上衣id, weaponId : 武器id, accessId : 饰品id, shoeId : 鞋子id
	 * EquipHeroResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void equipHero(int headId, int clothId, int weaponId, int accessId, int shoeId, UILoadingIndicator*loading = 0);
	/*
	 * getHeroEquip : 获取玩家人装备信息.
	 * GetHeroEquipResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	head : ItemIdPair类型, 帽子的id和iid
	 * 	clothes : ItemIdPair类型, 上衣的id和iid
	 * 	weapon : ItemIdPair类型, 武器的id和iid
	 * 	access : ItemIdPair类型, 饰品的id和iid
	 * 	shoe : ItemIdPair类型, 鞋子的id和iid
	 */
	void getHeroEquip(int cid ,UILoadingIndicator*loading = 0);
	/*
	 * buyCityLicense : 购买城市许可证
	 * BuyCityLicenseResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void buyCityLicense(UILoadingIndicator*loading = 0);
	/*
	 * deleteUser : 删除当前用户
	 * deleteType: 1,delete account,2 delete character
	 * DeleteUserResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void deleteUser(int deleteType,UILoadingIndicator*loading = 0);
	/*
	 * changePassword : 没有被使用
	 */
	void changePassword(char*password,UILoadingIndicator*loading = 0);
	/*
	 * selectHeroPositiveSkill : 选择战斗技能. slotIndex : 所选择槽的序号(1-4), skillId : 主动技能的id
	 * SelectHeroPositiveSkillResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	slotIndex : 所选择槽的序号(1-4)
	 * 	skillId : 技能id
	 * 	skillLv : 技能等级
	 */
	void selectHeroPositiveSkill(int slotIndex, int skillId, UILoadingIndicator*loading = 0);
	/*
	 * getHeroPositiveSkillList : 获取可以选择的战斗技能.
	 * GetHeroPositiveSkillListResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	skills : SkillDefine数组, 可选择战斗技能信息
	 */
	void getHeroPositiveSkillList(UILoadingIndicator*loading = 0);
	/*
	 * getSelectedPositiveSkills : 获取已经选择的战斗技能列表
	 * GetSelectedPositiveSkillsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	skills : PositiveSkill数组, 已经选择的战斗技能的信息
	 * 	skillPoints : 当前可用的技能点数
	 */
	void getSelectedPositiveSkills(UILoadingIndicator*loading = 0);
	/*
	 * getSailInfo : 获取航行信息.
	 * GetSailInfoResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	seaData : LeaveCityResult类型, 海上相关数据
	 * 	sailDays : 航行天数
	 * 	lastFightEventId : 上一个战斗的事件id
	 * 	wantedForceIds : 被通缉的国家id
	 * 	mainTaskInfo : CurrentMainTaskInfo类型
	 */
	void getSailInfo(UILoadingIndicator*loading = 0);
	/*
	 * getLeaderboard : 获取玩家排行榜. byCoins : 值非0按照玩家银币多少排行, 值为0按照玩家经验值排行
	 * GetLeaderboardResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	items : LeaderboardItem数组, 玩家排行信息
	 */
	void getLeaderboard(int byCoins, UILoadingIndicator*loading = 0);
	/*
	 * getCityStatus : 登陆时获取城市状态. cityId : 城市id
	 * GetCityStatusResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	friendValue : 是否是敌对国家
	 * 	hasLicense : 是否持有当前城市的交易许可
	 * 	cityId : 城市id
	 * 	isWanted : 是否被当前城市所在国家通缉
	 */
	void getCityStatus(int cityId, UILoadingIndicator*loading = 0);
	/*
	 * getDiscoveredCities : 获取玩家已经发现的城市
	 * GetDiscoveredCitiesResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	discoveredCityIds : 数组, 已经发现的城市的id
	 */
	void getDiscoveredCities(UILoadingIndicator*loading = 0);
	/*
	 * addDiscoveredCity : 增加已经发现的城市. cityid : 发现的城市id
	 * AddDiscoveredCityResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cityId : 发现城市的id
	 * 	addedExp : 增加的经验值
	 * 	addedFame : 增加的声望
	 * 	totalExp : 当前经验值
	 * 	totalFame : 当前声望值
	 * 	coins : 当前银币数量
	 * 	golds : 当前v票数量
	 * 	newLevel : 新等级
	 *		captains : CaptainInfo数组, 船长信息
	 *		new_fame_level : 新声望等级
	 */
	void addDiscoveredCity(int cityId, UILoadingIndicator*loading = 0);
	/*
	 * forceFightStatus : 战斗结束后更新战斗状态标志, 增加国家通缉的仇恨值
	 * ForceFightStatusResult : 返回结果
	 * 	failed : 0成功,非0失败
	 */
	void forceFightStatus(UILoadingIndicator*loading = 0);
	/*
	 * getCityProduces : 获取城市生产货物信息
	 * GetCityProducesResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cityId : 城市id
	 * 	currentTradeLevel : 当前城市的交易等级
	 * 	produces : CityProduceDefine数组, 城市生产货物信息
	 */
	void getCityProduces(UILoadingIndicator*loading = 0);
	/*
	 * getCityDemands : 获取当前城市出售货物的信息.
	 * GetCityDemandsResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	cityId :  城市id
	 * 	currentTradeLevel : 当前城市的交易等级
	 * 	prices : CityDemandsDefine数组, 城市出售货物的信息
	 */
	void getCityDemands(UILoadingIndicator*loading = 0);
	/*
	 * getMainTask : 获取主线任务
	 * GetMainTaskResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	currentTaskId : 当前任务id
	 * 	currentTaskStatus : 任务是否完成
	 * 	completedTaskPart : 任务已经完成了几个部分
	 * 	totalPartNum : 任务部分总数
	 */
	void getMainTask(UILoadingIndicator*loading = 0);
	/*
	 * completeMainTask : 完成任务
	 * CompleteMainTaskResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	currentTaskId : 当前任务id
	 * 	nextPartId : 下一个任务part id, 0表示任务完成
	 */
	void completeMainTask(int partId,UILoadingIndicator*loading = 0);
	/*
	 * getMainTaskReward : 获取任务奖励
	 * GetMainTaskRewardResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	rewardTaskId : 获取奖励的任务id
	 * 	newTaskId : 新任务的id
	 * 	exp : 经验值
	 * 	coins : 银币数量
	 * 	golds : v票数量
	 * 	fame : 声望值
	 * 	rewardCoin : 任务奖励的银币数
	 * 	rewardExp : 任务奖励的经验值
	 * 	rewardGold : 任务奖励的v票数
	 * 	rewardFame : 任务奖励的声望值
	 * 	forceId : 势力id(暂时没有用到)
	 * 	forceAffect : 暂时没有用到
	 * 	newLevel : 新的等级(暂时没有用到)
	 * 	captains : CaptainInfo数组, 船长信息(暂时没有用到)
	 * 	new_fame_level : 新的声望等级(暂时没有用到)
	 */
	void getMainTaskReward(UILoadingIndicator*loading = 0);
	/*
	 * getEventTaskList : 暂时没有实现
	 */
	void getEventTaskList(UILoadingIndicator*loading = 0);
	/*
	 * handleEventTask : 暂时没有实现
	 */
	void handleEventTask(int eventTaskId, int actionCode, UILoadingIndicator*loading = 0);
	/*
	 * getNpcScore : 获取npc的战斗力区间和玩家的战斗力. ids : npc id(值为0代表玩家), num : npc数量
	 * GetNpcScoreResult : 返回结果
	 * 	failed : 0成功,非0失败
	 * 	lowScores : 数组类型, npc和玩家的最低战斗力
	 * 	HigheScores : 数组类型, npc和玩家的最高战斗力
	 */
	void getNpcScore(int*ids, int num, UILoadingIndicator*loading = 0);
	/*
	 * getServerInfo : 废弃
	 */
	void getServerInfo();
	/*
	 * pingServer : 与服务器进行一次交流
	 * PingServerResult : 返回结果
	 * 	timeOutSeconds : 超时时间(可能和服务器交流的间隔时间)
	 */
	void pingServer();

	//从这里开始好多函数没注释

	void expandPackageSize(UILoadingIndicator*loading = 0);
	/*
	 * 获取主线任务物品，每个task只能获取一次
	 * */
	void getPretaskItems(UILoadingIndicator*loading = 0);

	void getOnsaleItems(UILoadingIndicator*loading = 0);
	void buyOnsaleItems(int uniqueid, int type, UILoadingIndicator*loading = 0);

	/*
	 * metaData的内容： app_version：%s,app_locale:%s,os:%s,os version:%s,os locale:%s,devcie_maker:%s,device_model:%s"
	 * 如果没登录，需要传入email，登录了传入null
	 * */
	void sendCustomServiceInfo(int type,char*info,char*email,UILoadingIndicator*loading = 0);

	//购买iab项目，iabItemName是对应后台iab 项目的unique 名字
	void getIABItem(char*iabItemName,UILoadingIndicator*loading = 0);

	void changeEmail(char*newEmail,UILoadingIndicator*loading = 0);
	//返回EmailLoginResult
	void tokenLogin(UILoadingIndicator*loading = 0);

	//登出服务器，google
	void logout();

	void engageInFightForTask(int npcId,UILoadingIndicator*loading = 0);

	/*
	 * 验证邮箱或者发送安全认证邮件，
	 * forSecurity = 1，为发送安全认证邮件
	 * */
	void authenticateAccount(char*newEmail,int forSecurity,UILoadingIndicator*loading = 0);

	void salvage(int x,int y,UILoadingIndicator*loading = 0);
	void getEmailVerifiedReward(UILoadingIndicator*loading = 0);
	void startSalvage(int x,int y,UILoadingIndicator*loading = 0);
	void getEmailVerifiedInfo(UILoadingIndicator*loading = 0);
	void getSalvageCondition(int x,int y,UILoadingIndicator*loading = 0);

	void getActivitiesProps(UILoadingIndicator*loading = 0);
	void getActivitiesGiftInfo(UILoadingIndicator*loading = 0);
	void getActivitiesPrayInfo(UILoadingIndicator*loading = 0);
	void getPray(int idx,UILoadingIndicator*loading = 0);
	void sendActivitiesGift(int recipientsCid,UILoadingIndicator*loading = 0);

	void getPirateAttackInfo(UILoadingIndicator*loading = 0);
	void getAttackPirateInfo(int eventid, UILoadingIndicator*loading = 0);
	void getVTicketMarketActivities(int type,UILoadingIndicator*loading = 0);
	void buyVTicketMarketActivities(int type,int uniqueId,int count,UILoadingIndicator*loading = 0);
	void finalMyExploitScore(UILoadingIndicator*loading = 0);
	/*
	 * 匹配离线玩家，每次返回一个，成功会返回玩家的基本信息
	 * */
	void findLootPlayer(UILoadingIndicator*loading = 0);
	/*
	 * findLootPlayer 调用成功之后，再调用lootplayer获取战斗需要的数据（EngageInFightResult），如果lootplayer返回失败，需要重新调用 findLootPlayer
	 * 利用EngageInFightResult进入战斗，战斗结束的时候调用endFight 获取奖励(如果胜利)
	 * */
	void lootPlayer(UILoadingIndicator*loading = 0);
	/*
	 * 获取离线委托战斗的log
	 * 如果要获取自己打劫别人的log，attackFlag传人1
	 *	displayalllog;显示近一个月的战斗log
	 * */
	void getLootPlayerLog(int attackFlag, int displayalllog, UILoadingIndicator*loading = 0);

	/*
	 * 返回 FindLootPlayerResult
	 * logId 从getLootPlayerLog中获取
	 * */
	void startRevengeOnLootPlayer(int logId,UILoadingIndicator*loading = 0);
	void getAttackPirateRankInfo(UILoadingIndicator*loading = 0);
	void getAttackPirateBossInfo(int enent, UILoadingIndicator*loading = 0);

	void buyInsurance(int64_t count,int days,int percent,int protectDelegate,UILoadingIndicator*loading = 0);
	void completeDialogs(int dialogId,UILoadingIndicator*loading = 0);
	/*
	 *
	 * */
	void obtainLottoMessage(UILoadingIndicator*loading = 0);
	void getInviteBonus(UILoadingIndicator*loading = 0);
	/*
	 * 获取小伙伴列表
	 * */
	void getCompanies(UILoadingIndicator*loading = 0);
	/*
	 * 装备小伙伴
	 * param：int headId, int clothId, int weaponId, int accessId, int shoeId ,对应的装备，unique id
	 * companyId:小伙伴唯一id，isCaptain：如果是captain就设置1
	 * */
	void equipCompany(int companyId,int isCaptain,int headId, int clothId, int weaponId, int accessId, int shoeId,UILoadingIndicator*loading = 0);
/*method 0 收集碎片解锁，1 ，金钱解锁
 * */
	void unlockCompany(int protoId,int method,UILoadingIndicator*loading = 0);
	void getCompanionsStatus(UILoadingIndicator*loading = 0);
	void getShipCompanions(UILoadingIndicator*loading = 0);
	void setShipCompanions(int companionsId, int cabinno, int  isCaptain, int shipId, UILoadingIndicator*loading = 0);
	void unlockShipCabin(int shipId, int sid,int cabinNo,UILoadingIndicator*loading = 0);
	/*
	 * 记录没有结算的战斗
	 * */
	void saveFailedFight(UILoadingIndicator*loading = 0);
	/*
	 * 战败，结算是否使用v票
	 * */
	void endFailedFightByVTicket(int useVTicket, int fighttype, UILoadingIndicator*loading = 0);

	/*
	 * w完成小伙伴剧情
	 * */
	void completeCompanionTask(int taskId,int partId,int missionFailed,UILoadingIndicator*loading = 0);

	void deductCoins(int coins,UILoadingIndicator*loading = 0);
	/**/
	void updateDataVersion(UpdateUserSelection**select,int num,UILoadingIndicator*loading = 0);

	void calFastHireCrew(UILoadingIndicator*loading = 0);
	void companionRebirth(int companionId,int isCaptain,UILoadingIndicator*loading = 0);
	//useType = //1道具2v票
	void repairEquipment(int equipId,int useType,UILoadingIndicator*loading = 0);
	void getRepairEquipmentNeed(int equipId,UILoadingIndicator*loading = 0);
	//type = 0,卖单，type = 1 买单
	void getRecentOrder(int type,UILoadingIndicator*loading = 0);
	void getDropIncreaseRateInfo(UILoadingIndicator*loading = 0);
	void getDailyActivitiesReward(int rewardType,UILoadingIndicator*loading = 0);

	void addProficiency(int proficiencyId,int count,UILoadingIndicator*loading = 0);
	void getProficiencyValues(UILoadingIndicator*loading = 0);
	void getProficiencyBook(int proficiencyId,UILoadingIndicator*loading = 0);

	void prepareCityDefense(int type,int count,UILoadingIndicator*loading = 0);
	void prepareCityAttack(int type,int count,UILoadingIndicator*loading = 0);
	void getCityPrepareStatus(UILoadingIndicator*loading = 0);
	void startStateWar(int nation,UILoadingIndicator*loading = 0);
	void getEnemyNation(UILoadingIndicator*loading = 0);
	
	void addCityRepairPool(int64_t count,UILoadingIndicator*loading = 0);
	void addCityAttackPool(int64_t count, UILoadingIndicator*loading = 0);
	void getPackageInfo(UILoadingIndicator*loading = 0);
	//海上点击时，显示城市战争页面接口
	void getNationWarEntrance(int city, UILoadingIndicator*loading = 0);
	//获取当前城市所属国家的战争排行
	void getContributionRank(UILoadingIndicator*loading = 0);
	//战斗中城市被掠夺的情况
	void getNationWarCityLost(UILoadingIndicator*loading = 0);
	//登陆补给站
	void reachDepot(UILoadingIndicator*loading = 0);
	//国战结算结果
	void reduceInNationWar(UILoadingIndicator*loading = 0);

	/*
	*对敌对国家发动战争
	*参数   nation_id:对方国家的id
	*/
	void applyStateWar(int nation_id, UILoadingIndicator*loading = 0);

	/*
	*开始国战进攻城市
	*参数  city_id;城市的id
	*/
	void startAttackCity(int city_id, UILoadingIndicator*loading = 0);
	/*
	*主动退出国战
	*参数  city_id;城市的id
	*/
	void endAttackCity(int city_id, UILoadingIndicator*loading = 0);

	//雇佣军   nationId势力ID，comsumeCoins投资的钱
	void warHiredArmy(int nationId, int64_t comsumeCoins, UILoadingIndicator*loading = 0);
	//投资V票 建造仓库
	void buildcountryWarDepot(int nationId, int32_t consumeV, UILoadingIndicator*loading = 0);
	//战斗阶段修复耐久 type V票,银币，count 数量
	
	void onWaringRepairDefense(int type, int64_t count, UILoadingIndicator*loading = 0);
	//战斗阶段增强超级火力 
	void onWaringEnhanceAttack(int city, int64_t consume_coins, UILoadingIndicator*loading = 0);
	/*
	*获取国战的状态(海上航行时提示使用)
	*/
	void getMyNationWar(UILoadingIndicator*loading = 0);

	/*
	*测试接口
	*/
	void funcForTest(UILoadingIndicator*loading = 0);

	/*
	*删除角色提示
	*/
	void checkDeleteCharactersNum(UILoadingIndicator*loading = 0);

	/**
	*获取玩家船队信息
	*/
	void getPlayerShipList(int cid , UILoadingIndicator * loading = 0);

	/**
	*获取玩家船只装备信息
	*/
	void getPlayerEquipShipInfo(int shipId, UILoadingIndicator * loading = 0);
	/**
	*解雇水手
	*/
	void tarvenFireSailors(int fireNums,UILoadingIndicator*loading = 0);
	/*
	*实时返回势力港口
	*/
	void getForceCity(int forceid, UILoadingIndicator*loading = 0);
	//获取跟新日志信息，os_type：区分安卓和ios，安卓为1，ios为2;lang为当前语言
	void getVersionChangeLog(int32_t os_type, int32_t lang,UILoadingIndicator*loading = 0,int32_t version = 0, int has_lang = 0, int has_version = 0);
	//proto示例函数
	void protoSample(int argu1,UILoadingIndicator*loading = 0);
//***************************************************************************API END  *************************************************************************************

	std::string& getInviteCode();
	EmailLoginResult*getLastLoginResult() {
		return mLastLoginResult;
	}
	GetMailListResult*getLastMailListResult() {
		return mGetMailListResult;
	}

	char*getHeroName() {
		return heroName;
	}
	int getCityIdx() {
		return cityIdx;
	}
	int64_t getCoins() {
		return coins;
	}
	int64_t getGold() {
		return gold;
	}
	int getIcon() {
		return icon;
	}
	bool isInTutorial() {
		return m_isInTutorial;
	}
	int getMainTaskStatus(){
		return m_mainTaskStatus;
	}
	CheckMailBoxResult* getLastCheckMailBoxResult() {
		return m_lastCheckMailBoxResult;
	}

	std::vector<int>& getPopularGoodsIds() {
		return popularGoodsIds;
	}
	int getNationIdx() { return nationIdx;}
	std::string getUserDefaultPrefix();
	std::string getFullKeyName(const char*key);
	void setGoogleAccountAndToken(const char*email,const char*token);
	std::string getGoogleAccount(){
		return m_googleAccount;
	}
	std::string getGoogleToken(){
		return m_googleToken;
	}

	void saveChangedEmail(const char*changedEmail);
	std::string getChangedEmail();
	void notifyGoogleLoginResult(int succ);
	int hasValidEmailAndPassword();
	void setGoogleAccountChecking( bool value) {m_googleAccountIsChecking = value;}
	bool isGoogleAccountChecking(){ return m_googleAccountIsChecking;}
    int getGoogleAccountLogResult() { return m_googleAccountLogResult;}
    bool isWaitingForGoogleToken(){ return m_isWaitingForGoogleToken;}
    void setIsWaitingForGoogleToken(bool value) { m_isWaitingForGoogleToken = value;}
    std::string getLoginEmail(){ return m_chatServerEmail;}
    void setAutoPing(int value){ m_autoPing = value;}

	/*不写注释么？*/
	void dispatchResponseCallbacks(BaseResponseMsg*baseMsg);
	/*不写注释么？*/
	void dispatchChatMessageCallbacks(const char*charMsg);
	/*不写注释么？*/
	void setLoadingRes(UILoadingIndicator*loading);

private:
	UILoadingIndicator*mLoadingLayer;
	UILoadingIndicator*mBackupLoadingLayer;

	bool isWaitingResponse;
	int m_autoPing;
	struct timeval lastPingTime;
	char*heroName;
	int cityIdx;
	int nationIdx;
	int guildId;
	int icon;
	//bool isAtSea;
	cocos2d::Point seaCord;
	int seaAreaIdx;
	int64_t coins;
	int64_t gold;
	std::string m_inviteCode;
	char*pendingEmail;
	char*pendingPassword;
	/*不写注释么？*/
	bool sendMessage(struct ProtobufCMessage* message, int type);
	/*不写注释么？*/
	bool sendMessage(const char*buffer, int size);
	ProtocolThread(void);
	std::thread* _subThreadInstance;
	std::mutex msgMutex;
	//std::mutex callbackQueueMutex;
	UserSession*mUserSession;
	std::vector<Buffer*> bufferToSend;
	std::deque<Buffer*> queueMsgBuffer;
	/*不写注释么？*/
	void sendQueueMsg();
	int idleSeconds;
	int waitingReponseSeconds;
	/*不写注释么？*/
	void ProcessSocketFunc();
	static ProtocolThread* m_pInstance;


	std::vector<ResponseCallBackPair> messageCallback;
	chatMsgCallBack chatMsgFunc;
	googleLoginCallBack m_googleLoginCallback;


	EmailLoginResult*mLastLoginResult;
	GetMailListResult*mGetMailListResult;
	struct timeval lastCheckMailTime;
	struct timeval lastSendMessageTime;
	bool gamePaused;
	bool connectionLost;
	bool selfDisconnect;
	bool chatServerOK;
	bool m_isInTutorial;
	int  m_mainTaskStatus;
	struct Arc4*m_rc4KeyEnc;
	struct Arc4*m_rc4KeyDec;
	std::vector<int> popularGoodsIds;
	CheckMailBoxResult* m_lastCheckMailBoxResult;
	long m_lastSendChatTime; // second
    unsigned int m_serverUrlHash;
    int m_serverPort;
    std::string m_serverUrl;
    std::string m_ChatIpUrl;
    int m_chatPort;

    std::string m_loginEmail;
    std::string m_googleAccount;
    std::string m_googleToken;
    std::string m_changedEmail;
    std::string m_chatServerToken;
    std::string m_chatServerEmail;
    bool m_googleAccountIsChecking;
    bool m_isWaitingForGoogleToken;
    int m_googleAccountLogResult;
	//海域划分
	int m_seaIndex;

	Buffer* m_vBuffer;
};
