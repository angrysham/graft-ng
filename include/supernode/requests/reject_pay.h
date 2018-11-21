
#pragma once

#include "router.h"

namespace graft::supernode::request {

GRAFT_DEFINE_IO_STRUCT(RejectPayRequest,
    (std::string, PaymentID)
);

GRAFT_DEFINE_IO_STRUCT(RejectPayResponse,
    (int, Result)
);

void registerRejectPayRequest(graft::Router &router);

}
