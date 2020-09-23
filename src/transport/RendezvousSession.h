/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file defines the CHIP RendezvousSession object that maintains a Rendezvous session.
 *
 */

#ifndef __TRANSPORT_RENDEZVOUSSESSION_H__
#define __TRANSPORT_RENDEZVOUSSESSION_H__

#include <core/CHIPCore.h>
#include <transport/RendezvousParameters.h>
#include <transport/RendezvousSessionDelegate.h>
#include <transport/SecurePairingSession.h>

namespace chip {

/**
 * RendezvousSession establishes and maintains the first connection between
 * a commissioner and a device. This connection is used in order to
 * provide the necessary infos for a device to participate to the CHIP
 * ecosystem.
 *
 * All the information transmitted over the underlying transport are
 * encrypted upon establishment of an initial secure pairing session.
 *
 * In order to securely transmit the informations, RendezvousSession
 * requires a setupPINCode to be shared between both ends. The
 * setupPINCode can be configured using RendezvousParameters
 *
 * @dotfile dots/Rendezvous/RendezvousSessionGeneral.dot
 *
 * The state of the secure pairing session setup can be observed by passing a
 * RendezvousSessionDelegate object to RendezvousSession.
 * Both the commissioner and the device needs to bootstrap RendezvousSession
 * using RendezvousParameters.
 *
 * @dotfile dots/Rendezvous/RendezvousSessionInit.dot
 */
class RendezvousSession : public SecurePairingSessionDelegate, public RendezvousSessionDelegate
{
public:
    RendezvousSession(RendezvousSessionDelegate * delegate, const RendezvousParameters & params) :
        mDelegate(delegate), mParams(params)
    {}
    ~RendezvousSession() override;

    /**
     * @brief
     *  Initialize the underlying transport using the RendezvousParameters passed in the constructor.
     *
     * @ return CHIP_ERROR  The result of the initialization
     */
    CHIP_ERROR Init();

    /**
     * @brief
     *  Return the associated pairing session.
     *
     * @return SecurePairingSession The associated pairing session
     */
    SecurePairingSession & GetPairingSession() { return mPairingSession; }

    //////////// SecurePairingSessionDelegate Implementation ///////////////
    CHIP_ERROR SendMessage(System::PacketBuffer * msgBuf) override;
    void OnPairingError(CHIP_ERROR err) override;
    void OnPairingComplete() override;

    //////////// RendezvousSessionDelegate Implementation ///////////////
    void OnRendezvousConnectionOpened() override;
    void OnRendezvousConnectionClosed() override;
    void OnRendezvousError(CHIP_ERROR err) override;
    void OnRendezvousMessageReceived(PacketBuffer * buffer) override;

private:
    CHIP_ERROR SendPairingMessage(System::PacketBuffer * msgBug);
    CHIP_ERROR HandlePairingMessage(System::PacketBuffer * msgBug);
    CHIP_ERROR Pair(Optional<NodeId> nodeId, uint32_t setupPINCode);
    CHIP_ERROR WaitForPairing(Optional<NodeId> nodeId, uint32_t setupPINCode);

    CHIP_ERROR SendSecureMessage(System::PacketBuffer * msgBug);
    CHIP_ERROR HandleSecureMessage(System::PacketBuffer * msgBuf);

    Transport::Base * mTransport          = nullptr; ///< Underlying transport
    RendezvousSessionDelegate * mDelegate = nullptr; ///< Underlying transport events
    const RendezvousParameters mParams;              ///< Rendezvous configuration

    SecurePairingSession mPairingSession;
    SecureSession mSecureSession;
    bool mPairingInProgress      = false;
    uint32_t mSecureMessageIndex = 0;
    uint16_t mNextKeyId          = 0;
};

} // namespace chip
#endif // __TRANSPORT_RENDEZVOUSSESSION_H__
