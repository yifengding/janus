#include "all.h"

namespace deptran {

char RW_BENCHMARK_TABLE[] = "customer";

void RWPiece::reg_all() {
    reg_pieces();
    reg_lock_oracles();
}

void RWPiece::reg_pieces() {
    TxnRegistry::reg(RW_BENCHMARK_R_TXN, RW_BENCHMARK_R_TXN_0, DF_NO,
            [] (const RequestHeader& header,
                const Value *input,
                rrr::i32 input_size,
                rrr::i32* res,
                Value* output,
                rrr::i32 *output_size,
                row_map_t *row_map,
                Vertex<PieInfo> *pv, Vertex<TxnInfo> *tv, std::vector<TxnInfo *> *conflict_txns) {
        mdb::Txn *txn = DTxnMgr::get_sole_mgr()->get_mdb_txn(header);
        Value buf;
        verify(input_size == 1);
        i32 output_index = 0;

        mdb::Row *r = txn->query(txn->get_table(RW_BENCHMARK_TABLE), input[0]).next();
        if (!txn->read_column(r, 1, &buf)) {
            *res = REJECT;
            *output_size = output_index;
            return;
        }
        output[output_index++] = buf;
        verify(*output_size >= output_index);
        *output_size = output_index;
        *res = SUCCESS;
        return;
    });

    TxnRegistry::reg(RW_BENCHMARK_W_TXN, RW_BENCHMARK_W_TXN_0, DF_REAL,
            [] (const RequestHeader& header,
                const Value *input,
                rrr::i32 input_size,
                rrr::i32* res,
                Value* output,
                rrr::i32 *output_size,
                row_map_t *row_map,
                Vertex<PieInfo> *pv, Vertex<TxnInfo> *tv, std::vector<TxnInfo *> *conflict_txns) {
        mdb::Txn *txn = DTxnMgr::get_sole_mgr()->get_mdb_txn(header);
        verify(input_size == 1);
        i32 output_index = 0;
        Value buf;

        mdb::Row *r = txn->query(txn->get_table(RW_BENCHMARK_TABLE), input[0]).next();
        if (!txn->read_column(r, 1, &buf)) {
            *res = REJECT;
            *output_size = output_index;
            return;
        }
        buf.set_i64(buf.get_i64() + 1);
        if (!txn->write_column(r, 1, /*input[1]*/buf)) {
            *res = REJECT;
            *output_size = output_index;
            return;
        }
        verify(*output_size >= output_index);
        *output_size = output_index;
        *res = SUCCESS;
        return;
    });

}

void RWPiece::reg_lock_oracles() {
//    TxnRegistry::reg_lock_oracle(RW_BENCHMARK_R_TXN, RW_BENCHMARK_R_TXN_0,
//            [] (const RequestHeader& header,
//                const Value *input,
//                rrr::i32 input_size,
//                std::unordered_map<cell_locator_t, int, cell_locator_t_hash> *opset
//                ) {
//        verify(input_size == 1);
//        cell_locator_t cl(RW_BENCHMARK_TABLE, 1);
//        cl.primary_key[0] = input[0].get_blob();
//        cl.col_id = 1;
//        (*opset)[cl] |= OP_DR;
//    });
//
//    TxnRegistry::reg_lock_oracle(RW_BENCHMARK_W_TXN, RW_BENCHMARK_W_TXN_0,
//            [] (const RequestHeader& header,
//                const Value *input,
//                rrr::i32 input_size,
//                std::unordered_map<cell_locator_t, int, cell_locator_t_hash> *opset
//                ) {
//        verify(input_size == 1);
//        cell_locator_t cl(RW_BENCHMARK_TABLE, 1);
//        cl.primary_key[0] = input[0].get_blob();
//        cl.col_id = 1;
//        (*opset)[cl] |= OP_W;
//    });
//
}

}

