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

#ifndef ASYNC_SERVER_ROOMS_ROOM_HPP
#define ASYNC_SERVER_ROOMS_ROOM_HPP

#include "./File.hpp"
#include "./Peer.hpp"
#include "dto/DTOs.hpp"
#include "utils/Statistics.hpp"

#include "oatpp/core/macro/component.hpp"

#include <unordered_map>
#include <list>

class Room {
private:
  oatpp::String m_name;
  std::atomic<v_int64> m_fileIdCounter;
  std::unordered_map<v_int64, std::shared_ptr<File>> m_fileById;
  std::unordered_map<v_int64, std::shared_ptr<Peer>> m_peerById;
  std::list<oatpp::Object<MessageDto>> m_history;
  std::mutex m_peerByIdLock;
  std::mutex m_fileByIdLock;
  std::mutex m_historyLock;
private:
  OATPP_COMPONENT(oatpp::Object<ConfigDto>, m_appConfig);
  OATPP_COMPONENT(std::shared_ptr<Statistics>, m_statistics);
public:

  Room(const oatpp::String& name)
    : m_name(name)
    , m_fileIdCounter(1)
  {
    ++ m_statistics->EVENT_ROOM_CREATED;
  }

  ~Room() {
    ++ m_statistics->EVENT_ROOM_DELETED;
  }

  /**
   * Get room name.
   * @return
   */
  oatpp::String getName();

  /**
   * Add peer to the room.
   * @param peer
   */
  void addPeer(const std::shared_ptr<Peer>& peer);

  /**
   * Inform the audience about the new peer.
   * @param peer
   */
  void welcomePeer(const std::shared_ptr<Peer>& peer);

  /**
   * Send info about other peers and available chat history to peer.
   * @param peer
   */
  void onboardPeer(const std::shared_ptr<Peer>& peer);

  /**
   * Send peer left room message.
   * @param peer
   */
  void goodbyePeer(const std::shared_ptr<Peer>& peer);

  /**
   * Get peer by id.
   * @param peerId
   * @return
   */
  std::shared_ptr<Peer> getPeerById(v_int64 peerId);

  /**
   * Remove peer from the room.
   * @param peerId
   */
  void removePeerById(v_int64 peerId);

  /**
   * Add message to history.
   * @param message
   */
  void addHistoryMessage(const oatpp::Object<MessageDto>& message);

  /**
   * Get list of history messages.
   * @return
   */
  oatpp::List<oatpp::Object<MessageDto>> getHistory();

  /**
   * Share file.
   * @param hostPeerId
   * @param fileClientId
   * @param fileName
   * @param fileSize
   * @return
   */
  std::shared_ptr<File> shareFile(v_int64 hostPeerId, v_int64 clientFileId, const oatpp::String& fileName, v_int64 fileSize);

  /**
   * Get file by id.
   * @param fileId
   * @return
   */
  std::shared_ptr<File> getFileById(v_int64 fileId);

  /**
   * Send message to all peers in the room.
   * @param message
   */
  void sendMessageAsync(const oatpp::Object<MessageDto>& message);

  /**
   * Websocket-Ping all peers.
   */
  void pingAllPeers();

  /**
   * Check if room is empty (no peers in the room).
   * @return
   */
  bool isEmpty();

};

#endif //ASYNC_SERVER_ROOMS_ROOM_HPP
