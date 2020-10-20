import consumer from "./consumer"

window.peers = {}

const isExistConnection = (peerIdentifier) => {
  return Object.keys(peers).includes(peerIdentifier)
}

const buildNewConnection = async (peerIdentifier, afterCandidatesMessage) => {
  const configuration = {
    offerToReceiveVideo: true
  }
  const peerConnection = new RTCPeerConnection(configuration)

  if (!window.localStream) {
    window.localStream = await navigator.mediaDevices.getUserMedia({ video: true })
  }

  window.localStream.getTracks().forEach(track => {
    peerConnection.addTrack(track, window.localStream);
  })

  peerConnection.onicecandidate = (e) => {
    if (!e.candidate) {
      connection.send({
        message: afterCandidatesMessage,
        to: peerIdentifier,
        sdp: peerConnection.localDescription
      })
    }
  }

  peerConnection.ontrack = function(e) {
    let track = e.track
    let stream = e.streams[0]
    document.getElementById('remote-video').srcObject = stream
  }

  // const datachannel = peerConnection.createDataChannel('textDataChannel')

  // datachannel.onopen = function (event) {
  //     var readyState = datachannel.readyState;
  //     if (readyState == "open") {
  //       datachannel.send("Hello");
  //     }
  //   };


  // datachannel.onmessage = (e) => {
  //   console.log(e)
  // }

  peers[peerIdentifier] = {
    connection: peerConnection,
    // datachanel: datachannel,
  }

  return peerConnection
}

const receivedCallMe = (data) => {
  debugger
  if (isExistConnection(data.from)) {
    return
  }

  buildNewConnection(data.from, 'Offer').then(peerConnection => {
    peerConnection.createOffer()
      .then((sdp) => peerConnection.setLocalDescription(sdp))
  })
}

const receivedOffer = (data) => {
  if (isExistConnection(data.from)) {
    return
  }

  buildNewConnection(data.from, 'Answer').then(peerConnection => {
    peerConnection.setRemoteDescription(new RTCSessionDescription(data.sdp))
      .then(() => peerConnection.createAnswer())
      .then((sdp) => peerConnection.setLocalDescription(sdp))
  })
}

const receivedAnswer = (data) => {
  const peerConnection = peers[data.from].connection
  peerConnection.setRemoteDescription(new RTCSessionDescription(data.sdp))
}

window.onload = function () {
  window.connection = consumer.subscriptions.create({
    channel: "RoomChannel",
    room_id: document.getElementById('room-id').innerText,
  }, {
    connected() {
      // Called when the subscription is ready for use on the server
      console.log("Connected to the room!");
    },

    disconnected() {
      // Called when the subscription has been terminated by the server
    },

    received(data) {
      // Called when there's incoming data on the websocket for this channel
      const messageTypes = [
        'CallMe',
        'Offer',
        'Answer',
      ]

      if (messageTypes.includes(data.message)) {
        eval(`received${data.message}`).call(null, data)
      }
    }
  });

  document.getElementById('call-me').onclick = function () {
    connection.send({ message: 'CallMe' })
  }
}
