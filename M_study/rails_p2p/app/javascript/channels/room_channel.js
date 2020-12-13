import consumer from "./consumer"

window.peers = {};

// init media
(function () {
  const audioContext = new window.AudioContext()

  navigator.mediaDevices.getUserMedia({ video: true, audio: true }).then(stream => {
    window.localStream = stream
    const audioSource = audioContext.createMediaStreamSource(stream)
    const audioProcessor = audioContext.createScriptProcessor(4096, 1, 1)

    audioSource.connect(audioProcessor)
    audioProcessor.connect(audioContext.destination)

    audioProcessor.onaudioprocess = (e) => {
      if (window.recording) {
        window.connection.perform("recording", { buffer: e.inputBuffer.getChannelData(0) })
      }
    }
  })
}())

const isExistConnection = (peerIdentifier) => {
  return Object.keys(peers).includes(peerIdentifier)
}

const buildNewConnection = async (peerIdentifier, afterCandidatesMessage) => {
  const peerConnection = new RTCPeerConnection()

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
    // let track = e.track
    let stream = e.streams[0]
    document.getElementById('remote-video').srcObject = stream
  }

  peers[peerIdentifier] = {
    connection: peerConnection,
    datachannels: [],
  }

  return peerConnection
}

const receivedCallMe = (data) => {
  if (isExistConnection(data.from)) {
    return
  }

  buildNewConnection(data.from, 'Offer').then(peerConnection => {
    // DataChannel
    const datachannel = peerConnection.createDataChannel('textDataChannel')
    peers[data.from].datachannels.push(datachannel)
    // Received Event
    datachannel.onmessage = appendMessageElem

    peerConnection.createOffer()
      .then((sdp) => peerConnection.setLocalDescription(sdp))
  })
}

const receivedOffer = (data) => {
  if (isExistConnection(data.from)) {
    return
  }

  buildNewConnection(data.from, 'Answer').then(peerConnection => {
    peerConnection.ondatachannel = (e) => {
      peers[data.from].datachannels.push(e.channel)
      e.channel.onmessage = appendMessageElem
    }

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

  document.getElementById('send-message').onclick = function (e) {
    const data = JSON.stringify({
      message: document.querySelector('#sending-message textarea').value
    })
    Object.values(peers).forEach(peer => {
      peer.datachannels.forEach(datachannel => datachannel.send(data))
    })
  }

  document.getElementById('start-recording').onclick = function (e) {
    window.recording = true
  }
  document.getElementById('stop-recording').onclick = function (e) {
    window.recording = false
    connection.perform('stop_recording')
  }
}

const appendMessageElem = (e) => {
  const data = JSON.parse(e.data)
  const el = document.createElement('p')
  const txt = document.createTextNode(data.message)

  el.appendChild(txt)
  document.getElementById('received-messages').insertBefore(el, null)
}
