#pragma once

void CALLBACK MessageQueue_CBTask_TimePublish(XENGINE_DBTIMERELEASE* pSt_DBInfo, XPVOID lParam);
void CALLBACK MessageQueue_CBTask_MemoryCache(ENUM_MEMORYCACHE_CALLBACK_TYPE enMemoryType, bool bSuccess, size_t nListCount, XENGINE_DBMESSAGEQUEUE* pSt_DBInfo, XPVOID lParam);