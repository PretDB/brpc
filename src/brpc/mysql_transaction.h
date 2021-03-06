// Copyright (c) 2019 Baidu, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Authors: Yang,Liming (yangliming01@baidu.com)

#ifndef BRPC_MYSQL_TRANSACTION_H
#define BRPC_MYSQL_TRANSACTION_H

#include "brpc/socket_id.h"
#include "brpc/channel.h"

namespace brpc {
// mysql isolation level enum
enum MysqlIsolationLevel {
    MysqlIsoRepeatableRead = 0,
    MysqlIsoReadCommitted = 1,
    MysqlIsoReadUnCommitted = 2,
    MysqlIsoSerializable = 3,
};
// mysql transaction options
struct MysqlTransactionOptions {
    // if is readonly transaction
    MysqlTransactionOptions() : readonly(false), isolation_level(MysqlIsoRepeatableRead) {}
    bool readonly;
    MysqlIsolationLevel isolation_level;
};
// mysql transaction type
class MysqlTransaction {
public:
    MysqlTransaction(Channel& channel, SocketUniquePtr& socket, ConnectionType conn_type);
    ~MysqlTransaction();
    SocketId GetSocketId() const;
    // commit transaction
    bool commit();
    // rollback transaction
    bool rollback();

private:
    bool DoneTransaction(const char* command);
    DISALLOW_COPY_AND_ASSIGN(MysqlTransaction);

private:
    Channel& _channel;
    SocketUniquePtr _socket;
    ConnectionType _conn_type;
};

inline MysqlTransaction::MysqlTransaction(Channel& channel,
                                          SocketUniquePtr& socket,
                                          ConnectionType conn_type)
    : _channel(channel), _conn_type(conn_type) {
    _socket.reset(socket.release());
}

inline MysqlTransaction::~MysqlTransaction() {
    CHECK(rollback()) << "rollback failed";
}

inline bool MysqlTransaction::commit() {
    return DoneTransaction("COMMIT");
}

inline bool MysqlTransaction::rollback() {
    return DoneTransaction("ROLLBACK");
}

typedef std::unique_ptr<MysqlTransaction> MysqlTransactionUniquePtr;

MysqlTransactionUniquePtr NewMysqlTransaction(
    Channel& channel, const MysqlTransactionOptions& opt = MysqlTransactionOptions());

}  // namespace brpc

#endif
