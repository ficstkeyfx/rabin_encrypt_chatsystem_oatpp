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

#include "Room.hpp"

oatpp::String Room::getName() {
  return m_name;
}

void Room::addPeer(const std::shared_ptr<Peer>& peer) {
  std::lock_guard<std::mutex> guard(m_peerByIdLock);
  m_peerById[peer->getPeerId()] = peer;
}

void Room::welcomePeer(const std::shared_ptr<Peer>& peer) {

  /* Inform all that peer have joined the room */

  auto joinedMessage = MessageDto::createShared();
  joinedMessage->code = MessageCodes::CODE_PEER_JOINED;
  joinedMessage->peerId = peer->getPeerId();
  joinedMessage->peerName = peer->getNickname();
  joinedMessage->message = peer->getNickname() + " - joined room";

  addHistoryMessage(joinedMessage);
  sendMessageAsync(joinedMessage);

}

void Room::onboardPeer(const std::shared_ptr<Peer>& peer) {

  auto infoMessage = MessageDto::createShared();
  infoMessage->code = MessageCodes::CODE_INFO;
  infoMessage->peerId = peer->getPeerId();
  infoMessage->peerName = peer->getNickname();

  infoMessage->peers = {};

  {
    std::lock_guard<std::mutex> guard(m_peerByIdLock);
    for (auto &it : m_peerById) {
      auto p = PeerDto::createShared();
      p->peerId = it.second->getPeerId();
      p->peerName = it.second->getNickname();
      infoMessage->peers->push_back(p);
    }
  }

  infoMessage->history = getHistory();
  peer->sendMessageAsync(infoMessage);

}

void Room::goodbyePeer(const std::shared_ptr<Peer>& peer) {

  auto message = MessageDto::createShared();
  message->code = MessageCodes::CODE_PEER_LEFT;
  message->peerId = peer->getPeerId();
  message->message = peer->getNickname() + " - left room";

  addHistoryMessage(message);
  sendMessageAsync(message);

}

std::shared_ptr<Peer> Room::getPeerById(v_int64 peerId) {
  std::lock_guard<std::mutex> guard(m_peerByIdLock);
  auto it = m_peerById.find(peerId);
  if(it != m_peerById.end()) {
    return it->second;
  }
  return nullptr;
}

void Room::removePeerById(v_int64 peerId) {

  std::lock_guard<std::mutex> guard(m_peerByIdLock);
  auto peer = m_peerById.find(peerId);

  if(peer != m_peerById.end()) {

    {
      std::lock_guard<std::mutex> guard(m_fileByIdLock);
      for (const auto &file : peer->second->getFiles()) {
        file->clearSubscribers();
        m_fileById.erase(file->getServerFileId());
      }

    }

    m_peerById.erase(peerId);

  }

}

void Room::addHistoryMessage(const oatpp::Object<MessageDto>& message) {

  if(!m_appConfig->maxRoomHistoryMessages || *m_appConfig->maxRoomHistoryMessages == 0) {
    return;
  }

  std::lock_guard<std::mutex> guard(m_historyLock);

  m_history.push_back(message);

  while(m_history.size() > m_appConfig->maxRoomHistoryMessages) {
    m_history.pop_front();
  }

}

oatpp::List<oatpp::Object<MessageDto>> Room::getHistory() {

  if(!m_appConfig->maxRoomHistoryMessages || *m_appConfig->maxRoomHistoryMessages == 0) {
    return nullptr;
  }

  auto result = oatpp::List<oatpp::Object<MessageDto>>::createShared();

  std::lock_guard<std::mutex> guard(m_historyLock);

  for(auto& message : m_history) {
    result->push_back(message);
  }

  return result;

}

std::shared_ptr<File> Room::shareFile(v_int64 hostPeerId, v_int64 clientFileId, const oatpp::String& fileName, v_int64 fileSize) {

  std::lock_guard<std::mutex> guard(m_fileByIdLock);

  auto host = getPeerById(hostPeerId);
  if(!host) throw std::runtime_error("File host not found.");

  v_int64 serverFileId = m_fileIdCounter ++;

  auto file = std::make_shared<File>(host, clientFileId, serverFileId, fileName, fileSize);
  host->addFile(file);

  m_fileById[serverFileId] = file;

  ++ m_statistics->EVENT_PEER_SHARE_FILE;

  return file;

}

std::shared_ptr<File> Room::getFileById(v_int64 fileId) {
  std::lock_guard<std::mutex> guard(m_fileByIdLock);
  auto it = m_fileById.find(fileId);
  if(it != m_fileById.end()) {
    return it->second;
  }
  return nullptr;
}

void Room::sendMessageAsync(const oatpp::Object<MessageDto>& message) {
  std::lock_guard<std::mutex> guard(m_peerByIdLock);
  for(auto& pair : m_peerById) {
    pair.second->sendMessageAsync(message);
  }
}

void Room::pingAllPeers() {
  std::lock_guard<std::mutex> guard(m_peerByIdLock);
  for(auto& pair : m_peerById) {
    auto& peer = pair.second;
    if(!peer->sendPingAsync()) {
      peer->invalidateSocket();
      ++ m_statistics->EVENT_PEER_ZOMBIE_DROPPED;
    }
  }
}

bool Room::isEmpty() {
  return m_peerById.size() == 0;
}
