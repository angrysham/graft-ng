#include "rejectpayrequest.h"
#include "requestdefines.h"

namespace graft {

Router::Status rejectPayHandler(const Router::vars_t& vars, const graft::Input& input,
                                 graft::Context& ctx, graft::Output& output)
{
    RejectPayRequest in = input.get<RejectPayRequest>();
    if (in.PaymentID.empty() || !ctx.global.hasKey(in.PaymentID + CONTEXT_KEY_STATUS))
    {
        ErrorResponse err;
        err.code = ERROR_PAYMENT_ID_INVALID;
        err.message = MESSAGE_PAYMENT_ID_INVALID;
        output.load(err);
        return Router::Status::Error;
    }
    ctx.global[in.PaymentID + CONTEXT_KEY_STATUS] = static_cast<int>(RTAStatus::RejectedByWallet);
    // TODO: Reject Pay: Add broadcast and another business logic
    RejectPayResponse out;
    out.Result = STATUS_OK;
    output.load(out);
    return Router::Status::Ok;
}

void registerRejectPayRequest(Router &router)
{
    Router::Handler3 h3(nullptr, rejectPayHandler, nullptr);
    router.addRoute("/dapi/reject_pay", METHOD_POST, h3);
}

}
