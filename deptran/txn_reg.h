#pragma once
#include "__dep__.h"
#include "constants.h"
#include "command.h"
namespace rococo {

#define TXN_GENERAL  (0x0)
#define TXN_ONE_SHOT (0x1)

class RequestHeader;
class Executor;

typedef std::map<
    int,
    mdb::Row *> row_map_t;

class DTxn;
class TxnCommand;
class TxnRequest;
class SimpleCommand;

typedef std::function<void(Executor* exec,
                           DTxn *dtxn,
                           SimpleCommand& cmd,
                           rrr::i32 *res,
                           map<int32_t, Value> &output
)> TxnHandler;

typedef enum {
  DF_REAL,
  DF_NO,
  DF_FAKE
} defer_t;

typedef struct {
  TxnHandler txn_handler;
  defer_t defer;
} txn_handler_defer_pair_t;

typedef std::function<bool(TxnCommand *,
                           map<int32_t, Value>&)>
    PieceCallbackHandler;


struct conf_id {
  string table{};
  std::vector<int> primary_keys{};
  std::vector<int> columns{};
  int row_context_id{};

  conf_id(string t,
          vector<int> k,
          vector<int> c,
          int rc_id) : table(t),
                       primary_keys(k),
                       columns(c),
                       row_context_id(rc_id) {
  }
};

/**
* This class holds all the hard-coded transactions pieces.
*/
class TxnRegistry {
 public:

  TxnRegistry() : callbacks_() {};

  void reg(base::i32 t_type, base::i32 p_type,
           defer_t defer, const TxnHandler &txn_handler) {
    auto func_key = std::make_pair(t_type, p_type);
    auto it = all_.find(func_key);
    verify(it == all_.end());
    all_[func_key] = (txn_handler_defer_pair_t) {txn_handler, defer};
  }

  txn_handler_defer_pair_t& get(const base::i32 t_type,
                               const base::i32 p_type) {
    auto it = all_.find(std::make_pair(t_type, p_type));
    verify(it != all_.end());
    return it->second;
  }
  txn_handler_defer_pair_t& get(const SimpleCommand &);

 public:
  // prevent instance creation
  // TxnRegistry() { }
  map<txntype_t, int> txn_types_{};
  map<std::pair<base::i32, base::i32>, txn_handler_defer_pair_t> all_{};
  map<std::pair<txntype_t, innid_t>, PieceCallbackHandler> callbacks_{};
  map<std::pair<txntype_t, innid_t>,
      std::pair<string, vector<int32_t>>> sharding_input_{};
  map<txntype_t, map<innid_t, set<int32_t>>> input_vars_{};
  map<txntype_t, map<innid_t, set<int32_t>>> output_vars_{};
  map<txntype_t, std::function<void(TxnCommand * ch, TxnRequest& req)> > init_{};
  map<txntype_t, std::function<void(TxnCommand * ch)>> retry_{};
  map<txntype_t, map<innid_t, vector<conf_id>>> conflicts_{};

  // PieceCallbackMap callbacks_;
  // static map<std::pair<base::i32, base::i32>, LockSetOracle> lck_oracle_;
};

} // namespace rococo