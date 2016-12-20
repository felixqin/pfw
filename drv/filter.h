#pragma once

#include "frule.h"
#include "arule.h"
#include "pktinfo.h"

USHORT Filter( PACKET_INFO *pInfo );
NTSTATUS Log( USHORT action, PACKET_INFO *pInfo );
