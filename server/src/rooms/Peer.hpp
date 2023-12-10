/***************************************************************************
 *
 * Project:   ______                ______ _
 *           / _____)              / _____) |          _
 *          | /      ____ ____ ___| /     | | _   ____| |_
 *          | |     / _  |  _ (___) |     | || \ / _  |  _)
 *          | \____( ( | | | | |  | \_____| | | ( ( | | |__
 *           \______)_||_|_| |_|   \______)_| |_|\_||_|\___)
 *
 *
 * Copyright 2020-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#ifndef ASYNC_SERVER_ROOMS_PEER_HPP
#define ASYNC_SERVER_ROOMS_PEER_HPP

#include "dto/DTOs.hpp"
#include "dto/Config.hpp"
#include "rooms/File.hpp"
#include "utils/Statistics.hpp"

#include "oatpp-websocket/AsyncWebSocket.hpp"

#include "oatpp/network/ConnectionProvider.hpp"

#include "oatpp/core/async/Lock.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

class Room; // FWD

class Peer : public oatpp::websocket::AsyncWebSocket::Listener {
private:

  /**
   * Buffer for messages. Needed for multi-frame messages.
   */
  oatpp::data::stream::BufferOutputStream m_messageBuffer;

  /**
   * Lock for synchronization of writes to the web socket.
   */
  oatpp::async::Lock m_writeLock;

private:
  std::shared_ptr<AsyncWebSocket> m_socket;
  std::shared_ptr<Room> m_room;
  oatpp::String m_nickname;
  v_int64 m_peerId;
private:
  std::atomic<v_int32> m_pingPoingCounter;
  std::list<std::shared_ptr<File>> m_files;
private:

  /* Inject application components */

  OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, m_asyncExecutor);
  OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, m_objectMapper);
  OATPP_COMPONENT(oatpp::Object<ConfigDto>, m_appConfig);
  OATPP_COMPONENT(std::shared_ptr<Statistics>, m_statistics);

private:

  oatpp::async::CoroutineStarter onApiError(const oatpp::String& errorMessage);

private:

  oatpp::async::CoroutineStarter validateFilesList(const MessageDto::FilesList& filesList);
  oatpp::async::CoroutineStarter handleFilesMessage(const oatpp::Object<MessageDto>& message);
  oatpp::async::CoroutineStarter handleFileChunkMessage(const oatpp::Object<MessageDto>& message);

  oatpp::async::CoroutineStarter handleMessage(const oatpp::Object<MessageDto>& message);

public:

  Peer(const std::shared_ptr<AsyncWebSocket>& socket,
       const std::shared_ptr<Room>& room,
       const oatpp::String& nickname,
       v_int64 peerId)
    : m_socket(socket)
    , m_room(room)
    , m_nickname(nickname)
    , m_peerId(peerId)
    , m_pingPoingCounter(0)
  {}

  /**
   * Send message to peer (to user).
   * @param message
   */
  void sendMessageAsync(const oatpp::Object<MessageDto>& message);

  /**
   * Send Websocket-Ping.
   * @return - `true` - ping was sent.
   * `false` peer has not responded to the last ping, it means we have to disconnect him.
   */
  bool sendPingAsync();

  /**
   * Get room of the peer.
   * @return
   */
  std::shared_ptr<Room> getRoom();

  /**
   * Get peer nickname.
   * @return
   */
  oatpp::String getNickname();

  /**
   * Get peer peerId.
   * @return
   */
  v_int64 getPeerId();

  /**
   * Add file shared by user. (for indexing purposes)
   */
  void addFile(const std::shared_ptr<File>& file);

  /**
   * List of shared by user files.
   * @return
   */
  const std::list<std::shared_ptr<File>>& getFiles();

  /**
   * Remove circle `std::shared_ptr` dependencies
   */
  void invalidateSocket();

public: // WebSocket Listener methods

  CoroutineStarter onPing(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) override;
  CoroutineStarter onPong(const std::shared_ptr<AsyncWebSocket>& socket, const oatpp::String& message) override;
  CoroutineStarter onClose(const std::shared_ptr<AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message) override;
  CoroutineStarter readMessage(const std::shared_ptr<AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;

};


#endif //ASYNC_SERVER_ROOMS_PEER_HPP
